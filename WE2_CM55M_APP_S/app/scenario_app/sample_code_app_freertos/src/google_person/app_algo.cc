#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "hx_drv_scu.h"

#include "WE2_device.h"
#include "board.h"
#include "console_io.h"

#include "app_platform.h"
#include "app_api.h"
#include "app_algo.h"



#include "ethosu_driver.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"


#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif


extern const unsigned char g_person_detect_model_data_vela[];
extern const int g_person_detect_model_data_vela_len;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */


constexpr int tensor_arena_size = 125*1024;
uint8_t tensor_arena[125*1024] __attribute__ ((section(".bss.tensor_arena"), aligned(16)));


#define HIMAX_INPUT_SIZE_X (96)
#define HIMAX_INPUT_SIZE_Y (96)

namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
}

//TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) {xprintf("%s", s);}//{ fprintf(stderr, "%s", s); }

#define LOCAL_FRAQ_BITS (8)
#define SC(A, B) ((A<<8)/B)
void fd_rescale(
        const uint8_t*in_image,
        const int32_t width,
        const int32_t height,
        const int32_t nwidth,
        const int32_t nheight,
        uint8_t*out_image,
        const int32_t nxfactor,
        const int32_t nyfactor) {
    int32_t x,y;
    int32_t ceil_x, ceil_y, floor_x, floor_y;

    int32_t fraction_x,fraction_y,one_min_x,one_min_y;
    int32_t pix[4];//4 pixels for the bilinear interpolation
    int32_t out_image_fix;

    for (y = 0; y < nheight; y++) {//compute new pixels
        for (x = 0; x < nwidth; x++) {
            floor_x = (x*nxfactor) >> LOCAL_FRAQ_BITS;//left pixels of the window
            floor_y = (y*nyfactor) >> LOCAL_FRAQ_BITS;//upper pixels of the window

            ceil_x = floor_x+1;//right pixels of the window
            if (ceil_x >= width) ceil_x=floor_x;//stay in image

            ceil_y = floor_y+1;//bottom pixels of the window
            if (ceil_y >= height) ceil_y=floor_y;

            fraction_x = x*nxfactor-(floor_x << LOCAL_FRAQ_BITS);//strength coefficients
            fraction_y = y*nyfactor-(floor_y << LOCAL_FRAQ_BITS);

            one_min_x = (1 << LOCAL_FRAQ_BITS)-fraction_x;
            one_min_y = (1 << LOCAL_FRAQ_BITS)-fraction_y;

            pix[0] = in_image[floor_y * width + floor_x];//store window
            pix[1] = in_image[floor_y * width + ceil_x];
            pix[2] = in_image[ceil_y * width + floor_x];
            pix[3] = in_image[ceil_y * width + ceil_x];

            //interpolate new pixel and truncate it's integer part
            out_image_fix = one_min_y*(one_min_x*pix[0]+fraction_x*pix[1])+fraction_y*(one_min_x*pix[2]+fraction_x*pix[3]);
            out_image_fix = out_image_fix >> (LOCAL_FRAQ_BITS * 2);
            out_image[nwidth*y+x] = out_image_fix;
        }
    }
}

static void _arm_npu_irq_handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

static void _arm_npu_irq_init(void)
{
	//notice when bitfile 0608.U55_IRQn = 192
	//U55_IRQn=15 before bitfile 0608
    const IRQn_Type ethosu_irqnum = (IRQn_Type)U55_IRQn;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    EPII_NVIC_SetVector(ethosu_irqnum, (uint32_t)_arm_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);

    //debug("EthosU IRQ#: %u, Handler: 0x%p\n",
    //        ethosu_irqnum, _arm_npu_irq_handler);
}

static int _arm_npu_init(void)
{
    int err = 0;

    /* If the platform has timing adapter blocks along with Ethos-U55 core
     * block, initialise them here. */
    /*if (0 != (err = _arm_npu_timing_adapter_init())) {
        return err;
    }*/

    /* Initialise the IRQ */
    _arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
    const void * ethosu_base_address = (void *)(U55_BASE);

    if (0 != (err = ethosu_init(
                            &ethosu_drv,             /* Ethos-U driver device pointer */
                            ethosu_base_address,     /* Ethos-U NPU's base address. */
                            NULL,       /* Pointer to fast mem area - NULL for U55. */
                            0, /* Fast mem region size. */
                            1,                       /* Security enable. */
                            1))) {                   /* Privilege enable. */
    	xprintf("failed to initalise Ethos-U device\n");
        return err;
    }

    xprintf("Ethos-U55 device initialised\n");
    /* Get Ethos-U version */
	struct ethosu_driver_version driver_version;
	struct ethosu_hw_info hw_info;

	ethosu_get_driver_version(&driver_version);
	ethosu_get_hw_info(&ethosu_drv, &hw_info);

	xprintf("Ethos-U version info:\n");
	xprintf("\tArch:       v%u.%u.%u\n",
									hw_info.version.arch_major_rev,
									hw_info.version.arch_minor_rev,
									hw_info.version.arch_patch_rev);
	xprintf("\tDriver:     v%u.%u.%u\n",
									driver_version.major,
									driver_version.minor,
									driver_version.patch);
	xprintf("\tMACs/cc:    %u\n", (uint32_t)(1 << hw_info.cfg.macs_per_cc));
	xprintf("\tCmd stream: %u\n", hw_info.cfg.cmd_stream_version);


    return 0;
}


int32_t app_algo_init()
{
	int ercode = 0;

	hx_drv_scu_set_U55_PORPL(SCU_U55_PORPL_PRIVILEGED);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_RESET);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_NORMAL);
	_arm_npu_init();

	error_reporter = &micro_error_reporter;
	//error_reporter->Report("start Google Example..\n");
	dbg_printf(DBG_LESS_INFO, "start Google Example...\n");

	model = ::tflite::GetModel(g_person_detect_model_data_vela);
    dbg_printf(DBG_LESS_INFO, "g_person_detect_model_data_vela @ %x\n", g_person_detect_model_data_vela);

	if (model->version() != TFLITE_SCHEMA_VERSION) {
		//error_reporter->Report(
			//"Model provided is schema version %d not equal "
			//"to supported version %d.\n",
			//model->version(), TFLITE_SCHEMA_VERSION);
		dbg_printf(DBG_LESS_INFO, "Model provided is schema version %d not equal "
								  "to supported version %d.\n",
								  model->version(), TFLITE_SCHEMA_VERSION);
	}


	static tflite::MicroMutableOpResolver<1> micro_op_resolver;
	micro_op_resolver.AddEthosU();

	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver,
										   tensor_arena, tensor_arena_size,
										   error_reporter);
	interpreter = &static_interpreter;
	if(interpreter->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}
	return ercode;
}

int32_t app_algo_run(uint32_t image_addr, uint32_t image_width, uint32_t image_height, ALGO_RESULT *algo_result)
{
	int ercode = 0;

	dbg_printf(DBG_LESS_INFO, "app_algo_run...\n");
	TfLiteTensor* input = interpreter->input(0);
	fd_rescale((uint8_t*)image_addr, image_width, image_height, HIMAX_INPUT_SIZE_X, HIMAX_INPUT_SIZE_Y,
			input->data.uint8, SC(image_width, HIMAX_INPUT_SIZE_X), SC(image_height, HIMAX_INPUT_SIZE_Y));

	// Now test with a blank image.
    for (uint32_t i = 0; i < input->bytes; ++i) {
        input->data.int8[i] = input->data.uint8[i] - 128;
    }

	dbg_printf(DBG_LESS_INFO, "invoke...\n");

	TfLiteStatus invoke_status = interpreter->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		//error_reporter->Report("invoke fail\n");
		dbg_printf(DBG_LESS_INFO, "invoke fail!\n");
	}
	//error_reporter->Report("invoke period=%d\n", tick_stop- tick_start);


	TfLiteTensor* output = interpreter->output(0);

	int8_t person_score = output->data.int8[1];

	dbg_printf(DBG_LESS_INFO, "person score: %d\n", person_score);

	if (person_score > 0)
	{
		algo_result->humanPresence = true;
		dbg_printf(DBG_LESS_INFO, "Human Presence.\n");
		algo_result->bd.num_of_detection = 1;
	}else{
		algo_result->humanPresence = false;
		dbg_printf(DBG_LESS_INFO, "No Human.\n");
		algo_result->bd.num_of_detection = 0;
	}

	return ercode;
}



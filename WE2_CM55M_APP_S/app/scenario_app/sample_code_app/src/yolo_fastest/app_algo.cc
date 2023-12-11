
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <forward_list>
#include <math.h>

#include "hx_drv_scu.h"
#include "WE2_device.h"
#include "board.h"
#include "console_io.h"
#include "app_platform.h"
#include "app_api.h"
#include "app_algo.h"
#include "ethosu_driver.h"
#include "img_proc_helium.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"

#define TFLITE_DEBUG

// Access external model byte array.

#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif


extern const unsigned char yolo_fastest_tflite[];
extern const int yolo_fastest_tflite_len;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */


// Input&Output tensor.
TfLiteTensor* input = nullptr;
TfLiteTensor* output[2] = {nullptr,nullptr};
static int sort_class;

constexpr int tensor_arena_size = 399*1024;

uint16_t g_sensor_width = 0;
uint16_t g_sensor_height = 0;

uint8_t tensor_arena[tensor_arena_size] __attribute__ ((section(".bss.tensor_arena"), aligned(16)));


#define HIMAX_INPUT_SIZE_X (160)
#define HIMAX_INPUT_SIZE_Y (160)

static uint8_t resize_imaage_temp_buffer[HIMAX_INPUT_SIZE_X*HIMAX_INPUT_SIZE_Y];
uint8_t* resized_img =  resize_imaage_temp_buffer;

typedef struct branch {
    int resolution;
    int num_box;
    float *anchor;
    int8_t *tf_output;
    float scale;
    int zero_point;
    size_t size;
    float scale_x_y;
} branch;

typedef struct network {
    int input_w;
    int input_h;
    int num_classes;
    int num_branch;
    branch *branchs;
    int topN;
} network;

typedef struct box {
    float x, y, w, h;
} box;

typedef struct detection{
    box bbox;
    float *prob;
    float objectness;
} detection;

typedef struct boxabs {
    float left, right, top, bot;
} boxabs;

namespace {
    tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = nullptr;
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
}


static network model_inference_pre_processing(TfLiteTensor* out_ten, TfLiteTensor* out2_ten);
static void model_inference_post_processing(network* net, ALGO_RESULT *algoresult);
static network model_inference(tflite::MicroInterpreter* static_interpreter, uint8_t* input_img);

//TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) {xprintf("%s", s);}//{ fprintf(stderr, "%s", s); }

#define LOCAL_FRAQ_BITS (8)
#define SC(A, B) ((A<<8)/B)

void free_dets(std::forward_list<detection> &dets){
    std::forward_list<detection>::iterator it;
    for ( it = dets.begin(); it != dets.end(); ++it ){
        free(it->prob);
    }
}

float sigmoid(float x)
{
    return 1.f/(1.f + exp(-x));
} 

bool det_objectness_comparator(detection &pa, detection &pb)
{
    return pa.objectness < pb.objectness;
}

void insert_topN_det(std::forward_list<detection> &dets, detection det){
    std::forward_list<detection>::iterator it;
    std::forward_list<detection>::iterator last_it;
    for ( it = dets.begin(); it != dets.end(); ++it ){
        if(it->objectness > det.objectness)
            break;
        last_it = it;
    }
    if(it != dets.begin()){
        dets.emplace_after(last_it, det);
        free(dets.begin()->prob);
        dets.pop_front();
    }
    else{
        free(det.prob);
    }
}
std::forward_list<detection> get_network_boxes(network *net, int image_w, int image_h, float thresh, int *num)
{
    std::forward_list<detection> dets;
    int i;
    int num_classes = net->num_classes;
    *num = 0;
    // 2 branchs 
    for (i = 0; i < net->num_branch; ++i) {
        int height  = net->branchs[i].resolution; //net->branchs[0].resolution: 5, net->branchs[1].resolution: 10
        int width = net->branchs[i].resolution;
        int channel  = net->branchs[i].num_box*(5 + num_classes); // For each channel, it has 3 anchor box (num_box=3), 1 for class probability(num_classes=1)

        // for each branchs, resolution is 5 and 10
        for (int h = 0; h < net->branchs[i].resolution; h++) {
            for (int w = 0; w < net->branchs[i].resolution; w++) {
                for (int anc = 0; anc < net->branchs[i].num_box; anc++) {
                    
                    // objectness score
                    int bbox_obj_offset = h * width * channel + w * channel + anc * (num_classes + 5) + 4;
                    float objectness = sigmoid(((float)net->branchs[i].tf_output[bbox_obj_offset] - net->branchs[i].zero_point) * net->branchs[i].scale);

                    if(objectness > thresh){
                        detection det;
                        det.prob = (float*)calloc(num_classes, sizeof(float));
                        det.objectness = objectness;
                        //get bbox prediction data for each anchor, each feature point
                        int bbox_x_offset = bbox_obj_offset -4;
                        int bbox_y_offset = bbox_x_offset + 1;
                        int bbox_w_offset = bbox_x_offset + 2;
                        int bbox_h_offset = bbox_x_offset + 3;
                        int bbox_scores_offset = bbox_x_offset + 5;
                        //int bbox_scores_step = 1;
                        det.bbox.x = ((float)net->branchs[i].tf_output[bbox_x_offset] - net->branchs[i].zero_point) * net->branchs[i].scale;
                        det.bbox.y = ((float)net->branchs[i].tf_output[bbox_y_offset] - net->branchs[i].zero_point) * net->branchs[i].scale;
                        det.bbox.w = ((float)net->branchs[i].tf_output[bbox_w_offset] - net->branchs[i].zero_point) * net->branchs[i].scale;
                        det.bbox.h = ((float)net->branchs[i].tf_output[bbox_h_offset] - net->branchs[i].zero_point) * net->branchs[i].scale;

                        float bbox_x, bbox_y;

                        // Eliminate grid sensitivity trick involved in YOLOv4
                        bbox_x = sigmoid(det.bbox.x); //* net->branchs[i].scale_x_y - (net->branchs[i].scale_x_y - 1) / 2;
                        bbox_y = sigmoid(det.bbox.y); //* net->branchs[i].scale_x_y - (net->branchs[i].scale_x_y - 1) / 2;
                        det.bbox.x = (bbox_x + w) / width;
                        det.bbox.y = (bbox_y + h) / height;

                        det.bbox.w = exp(det.bbox.w) * net->branchs[i].anchor[anc*2] / net->input_w;
                        det.bbox.h = exp(det.bbox.h) * net->branchs[i].anchor[anc*2+1] / net->input_h;
                        
                        for (int s = 0; s < num_classes; s++) {
                            det.prob[s] = sigmoid(((float)net->branchs[i].tf_output[bbox_scores_offset + s] - net->branchs[i].zero_point) * net->branchs[i].scale)*objectness;
                            det.prob[s] = (det.prob[s] > thresh) ? det.prob[s] : 0;
                        }

                        //correct_yolo_boxes 
                        det.bbox.x *= image_w;
                        det.bbox.w *= image_w;
                        det.bbox.y *= image_h;
                        det.bbox.h *= image_h;

                        if (*num < net->topN || net->topN <=0){
                            dets.emplace_front(det);
                            *num += 1;
                        }
                        else if(*num ==  net->topN){
                            dets.sort(det_objectness_comparator);
                            insert_topN_det(dets,det);
                            *num += 1;
                        }else{
                            insert_topN_det(dets,det);
                        }
                    }
                }
            }
        }
    }
    if(*num > net->topN)
        *num -=1;
    return dets;
}

// init part

branch create_brach(int resolution, int num_box, float *anchor, int8_t *tf_output, size_t size, float scale, int zero_point){
    branch b;
    b.resolution = resolution;
    b.num_box = num_box;
    b.anchor = anchor;
    b.tf_output = tf_output;
    b.size = size;
    b.scale = scale;
    b.zero_point = zero_point;
    return b;
}

network creat_network(int input_w, int input_h, int num_classes, int num_branch, branch* branchs, int topN){
    network net;
    net.input_w = input_w;
    net.input_h = input_h;
    net.num_classes = num_classes;
    net.num_branch = num_branch;
    net.branchs = branchs;
    net.topN = topN;
    return net;
}

// NMS part

float overlap(float x1, float w1, float x2, float w2)
{
    float l1 = x1 - w1/2;
    float l2 = x2 - w2/2;
    float left = l1 > l2 ? l1 : l2;
    float r1 = x1 + w1/2;
    float r2 = x2 + w2/2;
    float right = r1 < r2 ? r1 : r2;
    return right - left;
}

float box_intersection(box a, box b)
{
    float w = overlap(a.x, a.w, b.x, b.w);
    float h = overlap(a.y, a.h, b.y, b.h);
    if(w < 0 || h < 0) return 0;
    float area = w*h;
    return area;
}

float box_union(box a, box b)
{
    float i = box_intersection(a, b);
    float u = a.w*a.h + b.w*b.h - i;
    return u;
}

float box_iou(box a, box b)
{
    float I = box_intersection(a, b);
    float U = box_union(a, b);
    if (I == 0 || U == 0) {
        return 0;
    }
    return I / U;
}

bool det_comparator(detection &pa, detection &pb)
{
    return pa.prob[sort_class] > pb.prob[sort_class];
}

void do_nms_sort(std::forward_list<detection> &dets, int classes, float thresh)
{
    int k;
    
    for (k = 0; k < classes; ++k) {
        sort_class = k;
        dets.sort(det_comparator);
        
        for (std::forward_list<detection>::iterator it=dets.begin(); it != dets.end(); ++it){
            if (it->prob[k] == 0) continue;
            for (std::forward_list<detection>::iterator itc=std::next(it, 1); itc != dets.end(); ++itc){
                if (itc->prob[k] == 0) continue;
                if (box_iou(it->bbox, itc->bbox) > thresh) {
                    itc->prob[k] = 0;
                }
            }
        }
    }
}


boxabs box_c(box a, box b) {
    boxabs ba;//
    ba.top = 0;
    ba.bot = 0;
    ba.left = 0;
    ba.right = 0;
    ba.top = fmin(a.y - a.h / 2, b.y - b.h / 2);
    ba.bot = fmax(a.y + a.h / 2, b.y + b.h / 2);
    ba.left = fmin(a.x - a.w / 2, b.x - b.w / 2);
    ba.right = fmax(a.x + a.w / 2, b.x + b.w / 2);
    return ba;
}


float box_diou(box a, box b)
{
    boxabs ba = box_c(a, b);
    float w = ba.right - ba.left;
    float h = ba.bot - ba.top;
    float c = w * w + h * h;
    float iou = box_iou(a, b);
    if (c == 0) {
        return iou;
    }
    float d = (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
    float u = pow(d / c, 0.6);
    float diou_term = u;

    return iou - diou_term;
}


void diounms_sort(std::forward_list<detection> &dets, int classes, float thresh)
{
    int k;

    for (k = 0; k < classes; ++k) {
        sort_class = k;
        dets.sort(det_comparator);
        
        for (std::forward_list<detection>::iterator it=dets.begin(); it != dets.end(); ++it){
            if (it->prob[k] == 0) continue;
            for (std::forward_list<detection>::iterator itc=std::next(it, 1); itc != dets.end(); ++itc){
                if (itc->prob[k] == 0) continue;
                if (box_diou(it->bbox, itc->bbox) > thresh) {
                    itc->prob[k] = 0;
                }
            }
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


//extern "C" int app_algo_init()
int32_t app_algo_init()
{
	int ercode = 0;

	// vela
	hx_drv_scu_set_U55_PORPL(SCU_U55_PORPL_PRIVILEGED);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_RESET);
	hx_drv_scu_set_u55_state(SCU_U55_STATE_NORMAL);
	_arm_npu_init();

	error_reporter = &micro_error_reporter;
	error_reporter->Report("yolo_fastest algo init...\n");

    // Select model
	// vela
	model = ::tflite::GetModel(yolo_fastest_tflite);
    dbg_printf(DBG_LESS_INFO, "yolo_coco_tflite_vela @ %x\n", yolo_fastest_tflite);
            
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model->version(), TFLITE_SCHEMA_VERSION);
	}

	// Add operator 
	static tflite::MicroMutableOpResolver<1> micro_op_resolver;
	micro_op_resolver.AddEthosU();

	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver,
										   tensor_arena, tensor_arena_size,
										   error_reporter);
	interpreter = &static_interpreter;
	
	// Allocate memory for operation.
	if(interpreter->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}

	// Assign input. 
  	input = interpreter->input(0);

	return ercode;
}

static network model_inference_pre_processing(TfLiteTensor* out_ten, TfLiteTensor* out2_ten)
{
	int input_w = HIMAX_INPUT_SIZE_X;
	int input_h = HIMAX_INPUT_SIZE_Y;
	int num_classes = 1;
	int num_branch = 2;
	int topN = 0;		
	branch* branchs = (branch*)calloc(num_branch, sizeof(branch));
	
	// Anchor box def for YOLO COCO
	static float anchor1[] = {32, 93, 62,106, 113, 132};
    static float anchor2[] = {4, 9, 10, 25, 23, 49};
	
	branchs[0] = create_brach(input_w/32, 3, anchor1, out_ten->data.int8, out_ten->bytes , out_ten->params.scale, out_ten->params.zero_point);
	branchs[1] = create_brach(input_w/16, 3, anchor2, out2_ten->data.int8,  out2_ten->bytes, out2_ten->params.scale, out2_ten->params.zero_point);

	network net = creat_network(input_w, input_h, num_classes, num_branch, branchs,topN);
	#ifdef TFLITE_DEBUG
	xprintf("input_w:%d,input_h:%d,num_classes:%d,num_branch:%d,num_anchor:%d\n",net.input_w,net.input_h,net.num_classes,net.num_branch,net.branchs[1].num_box);
	#endif
	return net;
}

static void model_inference_post_processing(network* net, ALGO_RESULT *algoresult)
{
	int nboxes=0;
	float thresh = .80;
	float nms = .4;//.2;
	uint8_t counter = 0;

	std::forward_list<detection> dets = get_network_boxes(net, g_sensor_width, g_sensor_height, thresh, &nboxes);
	#ifdef TFLITE_DEBUG
	xprintf("box:%d\n",nboxes);
	#endif
	
	// do nms
	diounms_sort(dets, net->num_classes, nms);

	for (std::forward_list<detection>::iterator it=dets.begin(); it != dets.end(); ++it){
		float ymin = it->bbox.y - it->bbox.h / 2.;
		float xmin = it->bbox.x - ((it->bbox.w * 1.2) / 2.);
		float xmax = it->bbox.x + ((it->bbox.w * 1.2) / 2.);
		float ymax = it->bbox.y + ((it->bbox.h * 1.2) / 2.);

		if (xmin < 0) xmin = 0;
		if (ymin < 0) ymin = 0;
		if (xmax > g_sensor_width) xmax = g_sensor_width;
		if (ymax > g_sensor_height) ymax = g_sensor_height;

		float bx = xmin;
		float by = ymin;
		float bw = xmax - xmin;
		float bh = ymax - ymin;
		for (int j = 0; j < net->num_classes; ++j) {
			if (it->prob[j] > 0 && counter < MAX_TRACKED_ALGO_RES) {
				#ifdef TFLITE_DEBUG
				xprintf("{%d \"bbox\":[%d, %d, %d, %d], \"score\":%d},\n", j, (int)(bx), (int)(by), (int)(bw), (int)(bh), (int)(it->prob[j]*100));
				xprintf("xmin: %d, ymin:%d, xmax: %d, ymax: %d\n",(int)(bx), (int)(by), (int)(bx+bw), (int)(by+bh));
				#endif
					
				algoresult->bd.info[counter].score = (uint32_t)(it->prob[j]*100);
                algoresult->bd.info[counter].box.x = (uint32_t)(bx);
                algoresult->bd.info[counter].box.y = (uint32_t)(by);
                algoresult->bd.info[counter].box.width = (uint32_t)(bw);
                algoresult->bd.info[counter].box.height = (uint32_t)(bh);

				counter++;
			}
		}
		#ifdef TFLITE_DEBUG
		//xprintf("bbox_quantity: %d\r\n",counter);
		#endif
	}
	free_dets(dets);
	free((net->branchs));
	
	
	
    uint8_t  bbox_quantity = counter;
    xprintf("bbox_quantity = %d\n", bbox_quantity);

	if(bbox_quantity > 0)
	{
		algoresult->humanPresence = true;
		algoresult->bd.num_of_detection = bbox_quantity;
	}
	else
	{
		algoresult->humanPresence = false;
		algoresult->bd.num_of_detection = 0;
	}
	
	return;
}

static network model_inference(tflite::MicroInterpreter* static_interpreter, uint8_t* input_img)
{
	TfLiteTensor* in_ten = static_interpreter->input(0);
	TfLiteTensor* out_ten = static_interpreter->output(0); 
	TfLiteTensor* out2_ten = static_interpreter->output(1);
	#ifdef TFLITE_DEBUG
	xprintf("input size=%d,type=%d,addr of data=%x\n",in_ten->bytes,in_ten->type,in_ten->data.data);
	#endif
	network net = model_inference_pre_processing(out_ten, out2_ten);
	for (uint32_t i = 0; i < in_ten->bytes; ++i) {
        in_ten->data.int8[i] = (int8_t)(input_img[i]-128);
    }   
			
	TfLiteStatus invoke_status = static_interpreter->Invoke(); 

	if(invoke_status != kTfLiteOk)
	{
		xprintf("invoke fail\n");
	}
	#ifdef TFLITE_DEBUG
	else
		xprintf("invoke pass\n");
	#endif
	
	#ifdef TFLITE_DEBUG
	xprintf("output1 size=%d, output2 size=%d\n",out_ten->bytes,out2_ten->bytes);	
	#endif
	return net;
}

int32_t app_algo_run(uint32_t image_addr, uint32_t image_width, uint32_t image_height, ALGO_RESULT *algo)
{
	int ercode = 0;
	network net;

	dbg_printf(DBG_LESS_INFO, "app_algo_run...\n");
		
	// Resize image to fit model input
	hx_lib_image_resize((uint8_t*)image_addr, resized_img, image_width, image_height, 1, HIMAX_INPUT_SIZE_X, HIMAX_INPUT_SIZE_Y);		
		
	g_sensor_width = image_width;
	g_sensor_height = image_height;
		
	net = model_inference(interpreter, (uint8_t*)resized_img);	
	model_inference_post_processing(&net, algo);		

	return ercode;
}

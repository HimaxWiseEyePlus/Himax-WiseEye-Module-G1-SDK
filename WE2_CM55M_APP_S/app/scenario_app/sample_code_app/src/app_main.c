#include "app_platform.h"
#include "app_api.h"
#include "app_sys_info_cmd.h"
#include "app_datapath.h"
#include "app_board.h"
#include "app_xdma_cfg.h"
#include "app_i2c_cmd.h"
#if defined(UART_PROTOCOL) && defined(WE2_SINGLE_CORE)
#include "app_uart_cmd.h"
#endif

#ifdef TFLITE_ALGO_ENABLED
#include "app_algo.h"
#endif
#ifdef FLASH_AS_SRAM
#include "spi_eeprom_comm.h"
#endif

#include "hx_drv_pmu.h"
#include "hx_drv_scu.h"
#include "hx_drv_scu_export.h"

/* local variables */
static uint16_t data_types = TX_DATA_JPEG | TX_DATA_META;
#if defined(UART_PROTOCOL) && defined(WE2_SINGLE_CORE)
static uint32_t uart_id = UART_1_ID;
#endif
static volatile uint8_t g_wdma1_data[0/*WDMA1_SIZE*/] = {};
static volatile uint8_t g_wdma2_data[WDMA2_SLOT_SIZE*WDMA2_CYCLIC_BUF_CNT] = {};
static volatile uint8_t g_wdma3_data[WDMA3_SIZE] = {};
static volatile uint8_t g_jpeg_autofill_data[JPEG_AUTOFILL_SIZE] = {};

#ifdef TFLITE_ALGO_ENABLED
static ALGO_RESULT algo_result;
#endif

static void priv_i2c_cstm_cmd_handler(uint8_t cmd_id, uint8_t *cmd_payload, uint16_t cmd_payload_len);

#ifdef FLASH_AS_SRAM
static void priv_init_xip()
{
    uint8_t flash_info[3] = {0};
    int32_t ret = 0;

    dbg_printf(DBG_LESS_INFO, "priv_init_xip...\n");
    ret = hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    if (ret != 0) {
        dbg_printf(DBG_LESS_INFO, "hx_lib_spi_eeprom_open failed!\n");
    } else {
        dbg_printf(DBG_LESS_INFO, "hx_lib_spi_eeprom_open OK.\n");
    }
    hx_lib_spi_eeprom_read_ID(USE_DW_SPI_MST_Q, flash_info);
    ret = hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
    if (ret != 0) {
       dbg_printf(DBG_LESS_INFO, "hx_lib_spi_eeprom_enable_XIP failed!\n");
    } else {
        dbg_printf(DBG_LESS_INFO, "hx_lib_spi_eeprom_enable_XIP OK.\n");
    }
    dbg_printf(DBG_LESS_INFO, "priv_init_xip done.\n");
}
#endif

void app_init()
{
    app_dp_cfg_t app_dp_init_cfg;
    APP_SPI_CFG_T spi_cfg;

#ifdef TFLITE_ALGO_ENABLED
#ifdef FLASH_AS_SRAM
    priv_init_xip();
#endif
    dbg_printf(DBG_LESS_INFO, "app_algo_init...\n");
    app_algo_init();
#endif

    /* PINMUX. */
    app_board_pinmux_settings();

    /* SPI. */
    #ifdef SPI_MASTER_SEND /*master*/
    spi_cfg.mst_id = SPI_MST_1; /**< id*/
    spi_cfg.mst_freq = 5000000;  /**< frequency*/
    #endif

    #ifdef SPI_SLAVE_SEND /*slave*/
    spi_cfg.slv_handshake_pin = HX_GPIO_2;         /**< handshake gpio pin number*/
    spi_cfg.slv_handshake_actv_lvl = HX_GPIO_HIGH;     /**< handshake gpio pin active level*/

    dbg_printf(DBG_LESS_INFO, "HX_GPIO_2 %02x\n", HX_GPIO_2);
    dbg_printf(DBG_LESS_INFO, "HX_GPIO_HIGH %u\n", HX_GPIO_HIGH);

    dbg_printf(DBG_LESS_INFO, "spi_cfg.slv_handshake_pin %02x\n", spi_cfg.slv_handshake_pin);
    dbg_printf(DBG_LESS_INFO, "spi_cfg.slv_handshake_actv_lvl %u\n", spi_cfg.slv_handshake_actv_lvl);
    #endif

	#if defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND)
    app_spi_init(&spi_cfg);
    #endif

    /* UART. */
	#if defined(UART_PROTOCOL) && defined(WE2_SINGLE_CORE)
	if(API_SUCC != app_uart_open(uart_id, 921600))
	{
		dbg_printf(DBG_LESS_INFO, "app_uart_open fail\n");
	}
    #endif

    /* I2C Slave. */
    app_i2c_cmd_init(USE_DW_IIC_SLV_0);
    app_i2c_cmd_reg_cstm_feature(I2C_CMD_CSTM_FEATURE_1, (I2C_CMD_CSTM_CB)priv_i2c_cstm_cmd_handler);

    /* Datapath. */
    app_dp_get_def_init_cfg(&app_dp_init_cfg);
    app_dp_init_cfg.wdma1 = (uint32_t)g_wdma1_data;
    app_dp_init_cfg.wdma2 = (uint32_t)g_wdma2_data;
    app_dp_init_cfg.wdma3 = (uint32_t)g_wdma3_data;
    app_dp_init_cfg.jpeg_auto_fill_data = (uint32_t)g_jpeg_autofill_data;
    app_dp_init_cfg.wdma2_cyclic_buffer_cnt = (uint8_t)WDMA2_CYCLIC_BUF_CNT;
    app_dp_init(&app_dp_init_cfg);
}

void app_main()
{
	app_dp_cap_t app_dp_cap_set;
	uint32_t wakeup_event;
	uint32_t wakeup_event1;

	dbg_printf(DBG_LESS_INFO, "sample_code_app.\n");
    
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
    dbg_printf(DBG_LESS_INFO, "wakeup_event=0x%x,WakeupEvt1=0x%x\n", wakeup_event, wakeup_event1);

#ifdef WE2_DUAL_CORE
    dbg_printf(DBG_LESS_INFO, "CM55M_ENABLE_CM55S \n\n\n");
    hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
    hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
    hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);

    dbg_printf(DBG_LESS_INFO, "app_main start(dual core).\n");
#endif

    app_init();

    /* Set DP settings */
    app_dp_get_def_cap_settings(&app_dp_cap_set);

    while(1)
    {
        uint32_t raw_img_addr = 0, img_width = 0, img_heigh = 0;
        uint32_t jpg_addr = 0, jpg_size = 0;

        app_i2c_cmd_polling(USE_DW_IIC_SLV_0);
#if defined(UART_PROTOCOL) && defined(WE2_SINGLE_CORE)
        app_uart_cmd_polling(uart_id);
#endif

        /* Capture jpeg and raw image */
        dbg_printf(DBG_LESS_INFO, "[app_dp_capture_frame]\n");
        app_dp_capture_frame(&app_dp_cap_set);

        /* Capture sensor output to jpeg and raw */
        app_dp_get_frame(&raw_img_addr, &img_width, &img_heigh, &jpg_addr, &jpg_size);
        dbg_printf(DBG_LESS_INFO, "[app_dp_get_frame] raw_img_addr = %x img_width = %d img_height = %d, jpg_addr = %x, jpg_size = %d\n", raw_img_addr, img_width, img_heigh, jpg_addr, jpg_size);

#ifdef TFLITE_ALGO_ENABLED
        /* Execute Algorithm */
        memset(&algo_result, 0, sizeof(algo_result));
        app_algo_run(raw_img_addr, img_width, img_heigh, &algo_result);
#endif


        /* Send out JPG */
        if (TX_DATA_JPEG == (data_types & TX_DATA_JPEG)) {
            #if defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND)
            if (jpg_addr != 0)
                app_spi_write(jpg_addr, jpg_size, DATA_TYPE_JPG);
            #else defined(UART_PROTOCOL) && defined(WE2_SINGLE_CORE)
            if (jpg_addr != 0)
                app_uart_tx(jpg_addr, jpg_size, DATA_TYPE_JPG);
            #endif
        }

        /* Send out Un-compressed Image */
        if (TX_DATA_RAW == (data_types & TX_DATA_RAW)) {
            #if defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND)
            if(raw_img_addr != 0)
                app_spi_write(raw_img_addr, img_width*img_heigh, DATA_TYPE_RAW_IMG);
            #else defined(UART_PROTOCOL) && defined(WE2_SINGLE_CORE)
            if(raw_img_addr != 0)
                app_uart_tx(raw_img_addr, img_width*img_heigh, DATA_TYPE_RAW_IMG);
            #endif
        }

        /* Send out Metadata */
        if (TX_DATA_META == (data_types & TX_DATA_META)) {
            #if defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND)
            app_spi_write((uint32_t)&algo_result, sizeof(algo_result), DATA_TYPE_META_DATA);
            #else defined(UART_PROTOCOL) && defined(WE2_SINGLE_CORE)
            app_uart_tx((uint32_t)&algo_result, sizeof(algo_result), DATA_TYPE_META_DATA);
            #endif
        }

    }
}

int8_t app_sys_info_get_formal_version(INFO_FORMAL_VERSION *formal_version)
{
    formal_version->svn_version = 123456;
    strncpy((char *)formal_version->customer_name, "HMX", 5);
    formal_version->customer_version = 1;
    return 0;
}

int8_t app_sys_info_get_tx_protocol(INFO_TX_PROTOCOL *tx_protocol)
{
    tx_protocol->intf_type = TX_IF_SPI_M;
    tx_protocol->speed = 5000000;
    tx_protocol->data_types = data_types;
    return 0;
}

int8_t app_sys_info_get_raw_setting(INFO_RAW_SETTING *raw_setting)
{
    raw_setting->format = RAW_YUV400;
    raw_setting->width = DP_JPEG_ENC_WIDTH;
    raw_setting->height = DP_JPEG_ENC_HEIGHT;
    return 0;
}

typedef enum
{
    CSTM_CMD_1 = 0x00,
    CSTM_CMD_2 = 0x01,
    CSTM_CMD_3 = 0x02,
} CSTM_I2C_CMDS;

/*
* i2c data[0] = I2C_CMD_CSTM_FEATURE_1 (0x81)
* i2c data[1] = CSTM_I2C_CMD (0x00 = CSTM_CMD_1, 0x01 = CSTM_CMD_2, ...)
* i2c data[2] = length of command payload (LSB)
* i2c data[3] = length of command payload (MSB)
* i2c data[4 ~ n] = payload (only present when payload length > 0)
* e.g. IR Cut enable = [0x81, 0x00, 0x01, 0x00, 0x01]
*/
static void priv_i2c_cstm_cmd_handler(uint8_t cmd_id, uint8_t *cmd_payload, uint16_t cmd_payload_len)
{
    dbg_printf(DBG_LESS_INFO, "priv_i2c_cstm_cmd_handler executed. (cmd_id = 0x%02x, cmd_payload_len = %d)\n", cmd_id, cmd_payload_len);

    switch (cmd_id)
    {
        case CSTM_CMD_1:
        {
            break;
        }
        case CSTM_CMD_2:
        {
            break;
        }
        case CSTM_CMD_3:
        {
            break;
        }
        break;
    }
}


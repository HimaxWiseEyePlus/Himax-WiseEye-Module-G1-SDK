#include "app_platform.h"
#include "app_sys_info_cmd.h"
#include "app_adcc.h"

/* local variables */
static uint16_t data_types = TX_DATA_JPEG;

void app_init()
{
	app_adcc_init();
}

void app_main()
{
	APP_ADCC_CFG_T adcc_cfg;
	
	app_init();
	app_adcc_get_def_cfg(&adcc_cfg);

	adcc_cfg.analog_vol = ADCC_VOL_1P8V;
	adcc_cfg.enable = 1;
	adcc_cfg.single_shot_enable = 0;
	//adcc_cfg.channel_mask = 0; /*Enable CH0 CH1 CH2 CH3*/
	adcc_cfg.channel_mask = ADCC_CH1 | ADCC_CH2 | ADCC_CH3; /*Enable CH0*/
	//adcc_cfg.channel_mask = ADCC_CH0 | ADCC_CH2 | ADCC_CH3; /*Enable CH1*/
	//adcc_cfg.channel_mask = ADCC_CH0 | ADCC_CH1 | ADCC_CH3; /*Enable CH2*/
	//adcc_cfg.channel_mask = ADCC_CH0 | ADCC_CH1 | ADCC_CH2; /*Enable CH3*/
	//adcc_cfg.channel_mask = ADCC_CH0 | ADCC_CH1 | ADCC_CH2 | ADCC_CH3; /*Disable CH0 CH1 CH2 CH3*/
	adcc_cfg.sample_rate[0] = 3;
	adcc_cfg.sample_rate[1] = 3;
	adcc_cfg.sample_rate[2] = 3;
	adcc_cfg.sample_rate[3] = 3;
	adcc_cfg.data_type[0] = ADCC_DATA_AVERAGE;
	adcc_cfg.data_type[1] = ADCC_DATA_AVERAGE;
	adcc_cfg.data_type[2] = ADCC_DATA_AVERAGE;
	adcc_cfg.data_type[3] = ADCC_DATA_AVERAGE;
	adcc_cfg.offset[0] = 0;
	adcc_cfg.offset[1] = 0;
	adcc_cfg.offset[2] = 0;
	adcc_cfg.offset[3] = 0;
	adcc_cfg.high_threshold[0] = 0x900;
	adcc_cfg.high_threshold[1] = 0x900;
	adcc_cfg.high_threshold[2] = 0x900;
	adcc_cfg.high_threshold[3] = 0x900;
	adcc_cfg.low_threshold[0] = 0x300;
	adcc_cfg.low_threshold[1] = 0x300;
	adcc_cfg.low_threshold[2] = 0x300;
	adcc_cfg.low_threshold[3] = 0x300;
	adcc_cfg.int_delay = 0;
	
	//adcc_cfg.int_mask = ADCC_CH0_INT_MASK | ADCC_CH1_INT_MASK | ADCC_CH2_INT_MASK | ADCC_CH3_INT_MASK | ADCC_CONSTRAIN_INT_MASK | ADCC_SG_INT_MASK; /*Enable CH0 CH1 CH2 CH3*/
	adcc_cfg.int_mask = ADCC_CH0_INT_MASK | ADCC_CONSTRAIN_INT_MASK | ADCC_SG_INT_MASK; /*Enable CH0*/
	//adcc_cfg.int_mask = ADCC_CH1_INT_MASK | ADCC_CONSTRAIN_INT_MASK | ADCC_SG_INT_MASK; /*Enable CH0*/
	//adcc_cfg.int_mask = ADCC_CH2_INT_MASK | ADCC_CONSTRAIN_INT_MASK | ADCC_SG_INT_MASK; /*Enable CH0*/
	//adcc_cfg.int_mask = ADCC_CH3_INT_MASK | ADCC_CONSTRAIN_INT_MASK | ADCC_SG_INT_MASK; /*Enable CH0*/
	adcc_cfg.data_select[0] = ADCC_DATA_SEL_MUX;
	adcc_cfg.data_select[1] = ADCC_DATA_SEL_MUX;
	adcc_cfg.data_select[2] = ADCC_DATA_SEL_MUX;
	adcc_cfg.data_select[3] = ADCC_DATA_SEL_MUX;
	
	app_adcc_set_cfg(&adcc_cfg);
	
    while(1)
    {
    	APP_ADCC_STATUS_T adcc_status;
		
		memset(&adcc_status, 0, sizeof(APP_ADCC_STATUS_T));
		app_adcc_read_status(ADCC_CHANNEL0, &adcc_status);
		board_delay_ms(500);
		
		memset(&adcc_status, 0, sizeof(APP_ADCC_STATUS_T));
		app_adcc_read_status(ADCC_CHANNEL1, &adcc_status);
		board_delay_ms(500);
		
		memset(&adcc_status, 0, sizeof(APP_ADCC_STATUS_T));
		app_adcc_read_status(ADCC_CHANNEL2, &adcc_status);
		board_delay_ms(500);
		
		memset(&adcc_status, 0, sizeof(APP_ADCC_STATUS_T));
		app_adcc_read_status(ADCC_CHANNEL3, &adcc_status);
		board_delay_ms(500);
		
		app_adcc_dump_reg();
		board_delay_ms(1000);
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
    tx_protocol->speed = 10000000;
    tx_protocol->data_types = data_types;
    return 0;
}

int8_t app_sys_info_get_raw_setting(INFO_RAW_SETTING *raw_setting)
{
    raw_setting->format = RAW_YUV420;
    raw_setting->width = 640;
    raw_setting->height = 480;
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


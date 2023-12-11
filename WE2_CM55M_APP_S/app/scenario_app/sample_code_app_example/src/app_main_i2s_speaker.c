#include "app_platform.h"
#include "app_sys_info_cmd.h"
#include "app_api.h"
#include "app_speaker.h"
#include "hx_drv_scu_export.h"
#include "spi_eeprom_comm.h"

#define FRAME_SIZE_SAMPLES  0x10000					/*64KB*/

volatile uint8_t pcm_data[] __attribute__ ((section(".data.pcm_data"), aligned(4))) = {
#include "hello.in"
};

/* local variables */
static uint16_t data_types = TX_DATA_JPEG;

static void priv_init_xip()
{
    int32_t ret = 0;

    dbg_printf(DBG_LESS_INFO, "priv_init_xip...\n");
    ret = hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    if (ret != 0) {
        dbg_printf(DBG_LESS_INFO, "hx_lib_spi_eeprom_open failed!\n");
    } else {
        dbg_printf(DBG_LESS_INFO, "hx_lib_spi_eeprom_open OK.\n");
    }
    ret = hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
    if (ret != 0) {
       dbg_printf(DBG_LESS_INFO, "hx_lib_spi_eeprom_enable_XIP failed!\n");
    } else {
        dbg_printf(DBG_LESS_INFO, "hx_lib_spi_eeprom_enable_XIP OK.\n");
    }
    dbg_printf(DBG_LESS_INFO, "priv_init_xip done.\n");
}

void app_init()
{
	priv_init_xip();
	app_board_pinmux_settings();
	
	hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_I2C_M_SCL);
	hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_I2C_M_SDA);
	
	/*i2s master mode*/
    #ifdef NSC
    veneer_sys_set_PB6_pinmux(SCU_PB6_PINMUX_I2S_M_SCLK); /* I2S Master, SCLK */
	veneer_sys_set_PB7_pinmux(SCU_PB7_PINMUX_I2S_M_WS); /* I2S Master, WS */
	veneer_sys_set_PB8_pinmux(SCU_PB8_PINMUX_I2S_M_SDO); /* I2S Master, SDO */
    #else
    hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_I2S_M_SCLK); /* I2S Master, SCLK */
	hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_I2S_M_WS); /* I2S Master, WS */
	hx_drv_scu_set_PB8_pinmux(SCU_PB8_PINMUX_I2S_M_SDO);/* I2S Master, SDO */
    #endif
	
	app_speaker_i2s_init();
	app_speaker_init();

}

void app_main()
{
	app_init();

	while(1)
	{
		if(app_speaker_get_tx_status() == 0)
		{
			dbg_printf(DBG_LESS_INFO, "addr = 0x%x, sizeof(pcm_data) = 0x%x\n", pcm_data, sizeof(pcm_data));
			app_speaker_output((uint32_t)pcm_data, (uint32_t)sizeof(pcm_data), FRAME_SIZE_SAMPLES);
		}
		
		dbg_printf(DBG_LESS_INFO, ".");
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


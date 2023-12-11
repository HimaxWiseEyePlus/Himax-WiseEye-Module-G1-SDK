#include "app_platform.h"
#include "app_sys_info_cmd.h"
#include "app_api.h"
#include "app_pdm.h"
#include "hx_drv_scu_export.h"

/* local variables */
static uint16_t data_types = TX_DATA_JPEG;

#define PDM_BLK_NUM     16
#define PDM_BLK_SIZE    4096

static volatile uint8_t g_pdm_rec_buf[PDM_BLK_NUM*PDM_BLK_SIZE] __attribute__ ((aligned(32))) = {};

void priv_pmu_audio_cb(void* pcont)
{
	//app_pdm_config_t *ppdm_cfg = (app_pdm_config_t*)pcont;

	//dbg_printf(DBG_LESS_INFO, "priv_pmu_audio_cb\n");
}

void app_init()
{
	APP_SPI_CFG_T spi_cfg = {0};
	app_pdm_config_t app_pdm_cfg = {0};
	#if 1
	app_board_pinmux_settings();

	#ifdef SPI_MASTER_SEND /*master*/
	spi_cfg.mst_id = SPI_MST_1; /**< id*/
	spi_cfg.mst_freq = 10000000;	/**< frequency*/
	//spi_cfg.mst_freq = 5000000;  /**< frequency*/
	#endif
	
	#ifdef SPI_SLAVE_SEND /*slave*/
	spi_cfg.slv_handshake_pin = HX_AON_GPIO_0;		   /**< handshake gpio pin number*/
	spi_cfg.slv_handshake_actv_lvl = HX_GPIO_HIGH;	   /**< handshake gpio pin active level*/
	
	dbg_printf(DBG_LESS_INFO, "HX_AON_GPIO_0 %02x\n", HX_AON_GPIO_0);
	dbg_printf(DBG_LESS_INFO, "HX_GPIO_HIGH %u\n", HX_GPIO_HIGH);
	
	dbg_printf(DBG_LESS_INFO, "spi_cfg.slv_handshake_pin %02x\n", spi_cfg.slv_handshake_pin);
	dbg_printf(DBG_LESS_INFO, "spi_cfg.slv_handshake_actv_lvl %u\n", spi_cfg.slv_handshake_actv_lvl);
	#endif
	
	app_spi_init(&spi_cfg);
	#endif
	app_pdm_cfg.sample_rate = PDM_PCM_FREQ_16K;
	app_pdm_cfg.buffer_addr = g_pdm_rec_buf;
	app_pdm_cfg.block_num = PDM_BLK_NUM;
	app_pdm_cfg.block_sz = PDM_BLK_SIZE;
	app_pdm_cfg.cb_evt_blk = 2;
	app_pdm_cfg.g_audio_rx_cb_fn = priv_pmu_audio_cb;

	app_pdm_init(&app_pdm_cfg);
}

void app_main()
{
	uint32_t block_size;
	uint32_t pdm_buf_addr;
	uint32_t block;
	
	#ifdef WE2_DUAL_CORE
	dbg_printf(DBG_LESS_INFO, "CM55M_ENABLE_CM55S \n\n\n");
    hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
    hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
    hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);
    
    dbg_printf(DBG_LESS_INFO, "app_main start(dual core).\n");
	#endif
	
	app_init();
	app_pdm_rec_start();
	
	block_size = hx_lib_audio_get_block_size();
	
	while(1)
	{
		if(true == app_pdm_is_buf_ready(&pdm_buf_addr, &block))
		{
			if(block > 0)
			{
				dbg_printf(DBG_LESS_INFO, "pdm_buf_addr = 0x%x\n", pdm_buf_addr);
				dbg_printf(DBG_LESS_INFO, "block = 0x%x\n", block);
				
				app_spi_write((uint32_t)pdm_buf_addr, (uint32_t)block*block_size, DATA_TYPE_PDM);
				
				app_pdm_proc_buf_done(block);
			}
		}

		board_delay_ms(10);
	}

	app_pdm_rec_stop();
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


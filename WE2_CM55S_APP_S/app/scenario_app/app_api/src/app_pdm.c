#include "WE2_debug.h"

#include "app_platform.h"
#include "app_pdm.h"
#include "hx_drv_pdm_rx.h"
#include "hx_drv_scu_export.h"

#define CPNT_HINT "\033[1;34m"
#define CPNT_END "\033[m"

app_pdm_config_t aud_pdm_cfg = {0};

static void pdm_rx_callback_fun(uint32_t status)
{
	//dbg_printf(DBG_LESS_INFO, "pdm_rx_callback_fun.\n");

    if(aud_pdm_cfg.g_audio_rx_cb_fn != NULL)
        aud_pdm_cfg.g_audio_rx_cb_fn((void*)&aud_pdm_cfg);
}

void app_pdm_init(app_pdm_config_t *app_pdm_cfg)
{
    audio_config_t aud_lib_cfg;
    
	dbg_printf(DBG_LESS_INFO, "app_pdm_init.\n");
	
	#ifdef NSC
	veneer_sys_set_PB9_pinmux(SCU_PB9_PINMUX_PDM_CLK_1); /* PDM_PIN_CLK*/
	veneer_sys_set_PB10_pinmux(SCU_PB10_PINMUX_PDM_DATA0_1); /*PDM_PIN_DATA_0*/
	veneer_sys_set_PB11_pinmux(SCU_PB11_PINMUX_PDM_DATA1_1); /*PDM_PIN_DATA_1*/
	veneer_sys_set_PC3_pinmux(SCU_PC3_PINMUX_PDM_DATA2); /*PDM_PIN_DATA_2*/
	veneer_sys_set_PC4_pinmux(SCU_PC4_PINMUX_PDM_DATA3); /*PDM_PIN_DATA_3*/
	#else
	hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_PDM_CLK_1); /*PDM_PIN_CLK*/
	hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_PDM_DATA0_1); /*PDM_PIN_DATA_0*/
	hx_drv_scu_set_PB11_pinmux(SCU_PB11_PINMUX_PDM_DATA1_1); /*PDM_PIN_DATA_1*/
	hx_drv_scu_set_PC3_pinmux(SCU_PC3_PINMUX_PDM_DATA2); /*PDM_PIN_DATA_2*/
	hx_drv_scu_set_PC4_pinmux(SCU_PC4_PINMUX_PDM_DATA3); /*PDM_PIN_DATA_3*/
	#endif

    aud_lib_cfg.interface = AUDIO_IF_PDM;
    aud_lib_cfg.rx_mode = AUDIO_DMA; /*AUDIO_INT*/

    aud_lib_cfg.buffer.addr = app_pdm_cfg->buffer_addr;
    aud_lib_cfg.buffer.block_num = app_pdm_cfg->block_num;
    aud_lib_cfg.buffer.block_sz = app_pdm_cfg->block_sz;
    aud_lib_cfg.buffer.cb_evt_blk = app_pdm_cfg->cb_evt_blk ;

    aud_lib_cfg.pdm_config.reg_addr = HIMAX_PDM_BASE_ADDR;
    aud_lib_cfg.pdm_config.rx_fifo_threshold = 5;
    aud_lib_cfg.pdm_config.oversampling_ratio = 8; 
    aud_lib_cfg.pdm_config.cic_stages = 0;
    aud_lib_cfg.pdm_config.cic_delay = 0;
    aud_lib_cfg.pdm_config.dc_removal = 6;
    aud_lib_cfg.pdm_config.bit_range_shift = 5;
    aud_lib_cfg.pdm_config.pdm_clk_src = PDM_CLKSRC_LSCREF;
    aud_lib_cfg.pdm_config.sample_rate = app_pdm_cfg->sample_rate;
    aud_lib_cfg.pdm_config.data_in_0_en = PDM_DATA_IN_ENABLE;
    aud_lib_cfg.pdm_config.data_in_1_en = PDM_DATA_IN_DISABLE;
    aud_lib_cfg.pdm_config.data_in_2_en = PDM_DATA_IN_DISABLE;
    aud_lib_cfg.pdm_config.data_in_3_en = PDM_DATA_IN_DISABLE;
    aud_lib_cfg.pdm_config.capture_channel = PDM_CAPTURE_CHANNEL_LEFT_ONLY;
    aud_lib_cfg.pdm_config.dma_ch = PDM_USE_DMA2_CH_1; /*PDM_USE_DMA2_CH_0*/
    
    hx_lib_audio_init(&aud_lib_cfg);

    aud_pdm_cfg.sample_rate = app_pdm_cfg->sample_rate;
    aud_pdm_cfg.buffer_addr = app_pdm_cfg->buffer_addr;
    aud_pdm_cfg.block_num = app_pdm_cfg->block_num;
    aud_pdm_cfg.block_sz = app_pdm_cfg->block_sz;
    aud_pdm_cfg.cb_evt_blk = app_pdm_cfg->cb_evt_blk;
    aud_pdm_cfg.g_audio_rx_cb_fn = app_pdm_cfg->g_audio_rx_cb_fn;

	// register the callback function
	hx_lib_audio_register_evt_cb(pdm_rx_callback_fun);
}

void app_pdm_deinit(void)
{
	dbg_printf(DBG_LESS_INFO, "app_pdm_deinit.\n");
	
    hx_lib_audio_deinit();
}

void app_pdm_rec_start()
{
	dbg_printf(DBG_LESS_INFO, "app_pdm_rec_start.\n");
	
	hx_lib_audio_start();
}

void app_pdm_rec_stop()
{
	dbg_printf(DBG_LESS_INFO, "app_pdm_rec_stop.\n");
	
    hx_lib_audio_stop();
}

void app_pdm_rec_read_buf(uint8_t *buf, uint32_t buf_size, uint32_t *read_size)
{
    uint32_t pdm_buf_addr;
    uint32_t pdm_buf_size;
    uint32_t block;
    uint32_t block_size;
	
	dbg_printf(DBG_LESS_INFO, "app_pdm_rec_read_buf\n");
    
	*read_size = 0;
	
	hx_lib_audio_request_read(&pdm_buf_addr, &block);
    dbg_printf(DBG_LESS_INFO, "addr:0x%08x, block:%d\n", pdm_buf_addr, block);
	
	if(block == 0)
	{
		//dbg_printf(DBG_LESS_INFO, "block == 0\n");
		return;
	}
	
	if(buf == NULL || buf_size == 0)
	{
		dbg_printf(DBG_LESS_INFO, "buf == NULL || buf_size == 0\n");
		return;
	}
	
	block_size = hx_lib_audio_get_block_size();
	
	if(buf_size < block_size)
	{
		dbg_printf(DBG_LESS_INFO, "buf_size:0x%08x\n", buf_size);
		dbg_printf(DBG_LESS_INFO, "block_size:0x%08x\n", block_size);
		dbg_printf(DBG_LESS_INFO, "buf_size < block_size\n");
		return;
	}
	
    pdm_buf_size = block * block_size;
	
    if(buf_size < pdm_buf_size)
    {
    	block = buf_size/block_size;
		pdm_buf_size = block * block_size;
    }
	
	memcpy(buf, (uint8_t *)pdm_buf_addr, pdm_buf_size);
    *read_size = pdm_buf_size;
    hx_lib_audio_update_idx(&block);
}

void app_pdm_get_cfg(app_pdm_config_t *app_pdm_cfg)
{
    app_pdm_cfg->sample_rate = aud_pdm_cfg.sample_rate;
    app_pdm_cfg->buffer_addr = aud_pdm_cfg.buffer_addr;
    app_pdm_cfg->block_num = aud_pdm_cfg.block_num;
    app_pdm_cfg->block_sz = aud_pdm_cfg.block_sz;
    app_pdm_cfg->cb_evt_blk = aud_pdm_cfg.cb_evt_blk;
    app_pdm_cfg->g_audio_rx_cb_fn = aud_pdm_cfg.g_audio_rx_cb_fn;
}

uint8_t app_pdm_is_buf_ready()
{
	uint32_t pdm_buf_addr;
	uint32_t block;
	
	hx_lib_audio_request_read(&pdm_buf_addr, &block);
	
	if(block > 0)
		return true;
	
	return false;
}


//#include "hardware_config.h"
//#include "hx_drv_uart.h"

#include "WE2_debug.h"

#include "app_api.h"
#include "app_adcc.h"

#include "hx_drv_scu_export.h"

APP_ADCC_CFG_T g_app_adcc_cfg ={
    DEFAULT_ADCC_VOL, \
	DEFAULT_ADCC_ENABLE,\
	DEFAULT_ADCC_SINGLE_SHOT,\
	DEFAULT_ADCC_CH_MASK,\
	{DEFAULT_ADCC_SR, DEFAULT_ADCC_SR, DEFAULT_ADCC_SR, DEFAULT_ADCC_SR},\
	{DEFAULT_ADCC_DATA_TYPE, DEFAULT_ADCC_DATA_TYPE, DEFAULT_ADCC_DATA_TYPE, DEFAULT_ADCC_DATA_TYPE},\
	{DEFAULT_ADCC_OFFSET, DEFAULT_ADCC_OFFSET, DEFAULT_ADCC_OFFSET, DEFAULT_ADCC_OFFSET},\
	{DEFAULT_ADCC_HIGH_TH, DEFAULT_ADCC_HIGH_TH, DEFAULT_ADCC_HIGH_TH, DEFAULT_ADCC_HIGH_TH},\
	{DEFAULT_ADCC_LOW_TH, DEFAULT_ADCC_LOW_TH, DEFAULT_ADCC_LOW_TH, DEFAULT_ADCC_LOW_TH},\
	DEFAULT_ADCC_INT_DELAY,\
	DEFAULT_ADCC_INT_MASK,\
	{DEFAULT_ADCC_DATA_SELECT, DEFAULT_ADCC_DATA_SELECT, DEFAULT_ADCC_DATA_SELECT, DEFAULT_ADCC_DATA_SELECT},\
};


/***************************************************
 * Function Implementation
 **************************************************/
static void app_adcc_callback_ch_0(uint32_t status)
{
    dbg_printf(DBG_LESS_INFO, "ADCC CB_FUN_CH0 : 0x%x\n", status);

    if ( status & ADCC_CH0_HIGH_OUT_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH0 High OUT : 0x%x\n", status);

    if ( status & ADCC_CH0_HIGH_IN_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH0 High IN : 0x%x\n", status);

    if ( status & ADCC_CH0_LOW_OUT_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH0 Low OUT : 0x%x\n", status);

    if ( status & ADCC_CH0_LOW_IN_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH0 Low IN : 0x%x\n", status);

    if ( status & ADCC_CH0_HIGH_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH0 Init HIGH INT : 0x%x\n", status);

    if ( status & ADCC_CH0_LOW_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH0 Init LOW INT : 0x%x\n", status);

    return;
}

static void app_adcc_callback_ch_1(uint32_t status)
{
    dbg_printf(DBG_LESS_INFO, "ADCC CB_FUN_CH1 : 0x%x\n", status);

    if ( status & ADCC_CH1_HIGH_OUT_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH1 High OUT\n");

    if ( status & ADCC_CH1_HIGH_IN_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH1 High IN\n");

    if ( status & ADCC_CH1_LOW_OUT_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH1 Low OUT\n");

    if ( status & ADCC_CH1_LOW_IN_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH1 Low IN\n");

    if ( status & ADCC_CH1_HIGH_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH1 Init HIGH INT\n");

    if ( status & ADCC_CH1_LOW_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH1 Init LOW INT\n");

    return;
}

static void app_adcc_callback_ch_2(uint32_t status)
{
    dbg_printf(DBG_LESS_INFO, "ADCC CB_FUN_CH2 : 0x%x\n", status);

    if ( status & ADCC_CH2_HIGH_OUT_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH2 High OUT\n");

    if ( status & ADCC_CH2_HIGH_IN_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH2 High IN\n");

    if ( status & ADCC_CH2_LOW_OUT_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH2 Low OUT\n");

    if ( status & ADCC_CH2_LOW_IN_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH2 Low IN\n");

    if ( status & ADCC_CH2_HIGH_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH2 Init HIGH INT\n");

    if ( status & ADCC_CH2_LOW_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH2 Init LOW INT\n");

    return;
}

static void app_adcc_callback_ch_3(uint32_t status)
{
    dbg_printf(DBG_LESS_INFO, "ADCC CB_FUN_CH3 : 0x%x\n", status);

    if ( status & ADCC_CH3_HIGH_OUT_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH3 High OUT\n");

    if ( status & ADCC_CH3_HIGH_IN_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH3 High IN\n");

    if ( status & ADCC_CH3_LOW_OUT_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH3 Low OUT\n");

    if ( status & ADCC_CH3_LOW_IN_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH3 Low IN\n");

    if ( status & ADCC_CH3_HIGH_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH3 Init HIGH INT\n");

    if ( status & ADCC_CH3_LOW_INT_BIT )
        dbg_printf(DBG_LESS_INFO, "ADCC CH3 Init LOW INT\n");

    return;
}

static void app_adcc_constrain_cb(uint32_t status)
{
    dbg_printf(DBG_LESS_INFO, "ADCC Constrain CB_FUN\n");
    dbg_printf(DBG_LESS_INFO, "Interrupt Status Code : %x\n", status);
    return;
}

static void app_adcc_sg_cb(uint32_t status)
{
    dbg_printf(DBG_LESS_INFO, "ADCC Single Shot CB_FUN\n");
    dbg_printf(DBG_LESS_INFO, "Interrupt Status Code : %x\n", status);
    return;
}

int8_t app_adcc_init(void)
{
    uint32_t base_addr = BASE_ADDR_APB_ADC_CTRL_LV_ALIAS;
    ADCC_CFG_T adcc_cfg;
    uint32_t int_status;

    hx_drv_adcc_get_int_status(&int_status);
    dbg_printf(DBG_LESS_INFO, "before clear adcc INT status : 0x%x\n", int_status);
    hx_drv_adcc_set_int_clear(int_status);
    hx_drv_adcc_get_int_status(&int_status);
    dbg_printf(DBG_LESS_INFO, "after clear adcc INT status : 0x%x\n", int_status);
    
    #ifndef NSC
    SCU_PDLSC_ADCCKIN_CFG_T adc_clk_cfg;
    adc_clk_cfg.adcckin_src = 1;    // (0: RC24M1M, 1:XTAL24M)
    adc_clk_cfg.adcckindiv = 5;     //  24M/(1+5)x4 = 1MHz
    hx_drv_scu_set_pdlsc_adcckinclk_cfg(adc_clk_cfg);
    #endif
    
    hx_drv_adcc_get_defCfg(&adcc_cfg);
    adcc_cfg.analog_vol = ADCC_VOL_1P8V;
    hx_drv_adcc_init(base_addr, &adcc_cfg);
    
    hx_drv_adcc_register_cb(ADCC_CH0_CB, app_adcc_callback_ch_0);
	hx_drv_adcc_register_cb(ADCC_CH1_CB, app_adcc_callback_ch_1);
	hx_drv_adcc_register_cb(ADCC_CH2_CB, app_adcc_callback_ch_2);
	hx_drv_adcc_register_cb(ADCC_CH3_CB, app_adcc_callback_ch_3);
	hx_drv_adcc_register_cb(ADCC_CONSTRAIN_CB, app_adcc_constrain_cb);
	hx_drv_adcc_register_cb(ADCC_SG_CB, app_adcc_sg_cb);
    
    return 0;
}

int8_t app_adcc_deinit(void)
{
    hx_drv_adcc_deinit();
    
    return 0;
}

/* set ADCC enable */
int8_t app_adcc_enable(uint8_t enable)
{
	hx_drv_adcc_set_enable(enable);
    
    return 0;
}

int8_t app_adcc_get_def_cfg(APP_ADCC_CFG_T *padcc_cfg)
{
    memcpy(padcc_cfg, &g_app_adcc_cfg, sizeof(APP_ADCC_CFG_T));

    return 0;
}

int8_t app_adcc_set_cfg(APP_ADCC_CFG_T *padcc_cfg)
{
    uint8_t channel;
	uint16_t data_type, data_sel;
    
    hx_drv_adcc_set_enable(0); /*disable*/
    
    hx_drv_adcc_set_single_shot(padcc_cfg->single_shot_enable);
    hx_drv_adcc_set_channel_mask(padcc_cfg->channel_mask);

    for(channel = 0; channel < 4; channel++)
    {
		hx_drv_adcc_set_sample_rate(channel, padcc_cfg->sample_rate[channel]);
		hx_drv_adcc_set_data_offset(channel, padcc_cfg->offset[channel]);
		hx_drv_adcc_set_high_threshold(channel, padcc_cfg->high_threshold[channel]);
		hx_drv_adcc_set_low_threshold(channel, padcc_cfg->low_threshold[channel]);
    }

    hx_drv_adcc_set_int_delay(padcc_cfg->int_delay);
    hx_drv_adcc_set_int_mask(padcc_cfg->int_mask);

	data_type = (padcc_cfg->data_type[0]<<ADCC_CH0_DATA_TYPE_BIT_SFT) | (padcc_cfg->data_type[1]<<ADCC_CH1_DATA_TYPE_BIT_SFT) | \
				(padcc_cfg->data_type[2]<<ADCC_CH2_DATA_TYPE_BIT_SFT) | (padcc_cfg->data_type[3]<<ADCC_CH3_DATA_TYPE_BIT_SFT);
	hx_drv_adcc_set_data_type(data_type);

	data_sel = (padcc_cfg->data_select[0]<<ADCC_CH0_DATA_SEL_BIT_SFT) | (padcc_cfg->data_select[1]<<ADCC_CH1_DATA_SEL_BIT_SFT) | \
				(padcc_cfg->data_select[2]<<ADCC_CH2_DATA_SEL_BIT_SFT) | (padcc_cfg->data_select[3]<<ADCC_CH3_DATA_SEL_BIT_SFT);
    dbg_printf(DBG_LESS_INFO, "data_sel : 0x%02x\n", data_sel);
	hx_drv_adcc_set_data_select(data_sel);
    
    hx_drv_adcc_set_enable(padcc_cfg->enable); /*enable*/
    
    return 0;
 
}

int8_t app_adcc_read_status(uint8_t ch, APP_ADCC_STATUS_T *padcc_status)
{
    padcc_status->ch_data = 0;
    padcc_status->int_status = 0;
    
	hx_drv_adcc_get_int_status(&padcc_status->int_status);
	dbg_printf(DBG_LESS_INFO, "adcc INT status : 0x%08x\n", padcc_status->int_status);

    hx_drv_adcc_get_sample_data(ch, &padcc_status->ch_data);
    dbg_printf(DBG_LESS_INFO, "Channel[%u] Data = 0x%04x\n", ch, padcc_status->ch_data);
    
    return 0;
}

/** dumpReg **/
int8_t app_adcc_dump_reg(void)
{
	int i;
	uint32_t *adc_lv_aip_addr, *adc_lv_ctrl_addr, *adc_hv_aip_addr;
	
	dbg_printf(DBG_LESS_INFO, "[%s]\n", __FUNCTION__);

	adc_hv_aip_addr  = (uint32_t *) BASE_ADDR_APB_ADC_AIP_HV_ALIAS;
	adc_lv_aip_addr  = (uint32_t *) BASE_ADDR_APB_ADC_AIP_LV_ALIAS;
	adc_lv_ctrl_addr = (uint32_t *) BASE_ADDR_APB_ADC_CTRL_LV_ALIAS;

	dbg_printf(DBG_LESS_INFO, "LV_AIP_addr:0x%08x, LV_Ctrl_addr:0x%08x, SB_AIP_addr:0x%08x \n", adc_lv_aip_addr, adc_lv_ctrl_addr, adc_hv_aip_addr);

	dbg_printf(DBG_LESS_INFO, "-------------------------------------------- \n");
	for (i = 0; i < 7; i++)
	{
		dbg_printf(DBG_LESS_INFO, "ADC_HV_AIP_REG[0x%04x] = 0x%02x\n", adc_hv_aip_addr+i, *(adc_hv_aip_addr+i));
	}
	dbg_printf(DBG_LESS_INFO, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
	for (i = 0; i < 10; i++)
	{
		dbg_printf(DBG_LESS_INFO, "ADC_AIP_REG[0x%04x] = 0x%02x\n", adc_lv_aip_addr+i, *(adc_lv_aip_addr+i));
	}
	dbg_printf(DBG_LESS_INFO, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
	for (i = 0; i < 27; i++)
	{
		dbg_printf(DBG_LESS_INFO, "ADC_CTRL_REG[0x%04x] = 0x%02x\n", adc_lv_ctrl_addr+i, *(adc_lv_ctrl_addr+i));
	}
	dbg_printf(DBG_LESS_INFO, "-------------------------------------------- \n");
	dbg_printf(DBG_LESS_INFO, "\n");

	return 0;
}


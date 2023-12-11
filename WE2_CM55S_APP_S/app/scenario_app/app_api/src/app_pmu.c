#include "board.h"

#include "hx_drv_pmu.h"
#include "hx_drv_cdm.h"
#include "powermode.h"

#include "WE2_debug.h"

#include "app_pmu.h"

static void priv_pmu_set_clk_cfg(SCU_SYSCLKSRC_HW_E clksrcsel, SCU_LSC_CLK_CFG_T *lsc_cfg, SCU_PDHSC_HSCCLK_CFG_T *hsc_cfg)
{
	switch(clksrcsel)
	{
		case SCU_SYSCLKSRC_HW_RC24MRC32K:// /**< CLK Source HW RC(RC24M and RC32K) */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC24M1M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_1;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_1;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC24M1M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;	
		case SCU_SYSCLKSRC_HW_RC24MRC48MRC32K://, /**< CLK Source HW RC24M RC48M and RC32K)  */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC96M48M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_1;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_1;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC96M48M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_RC24MRC96MRC32K://, /**< CLK Source HW RC24M RC96M and RC32K)  */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC96M48M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_2;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_2;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC96M48M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_XTAL24MXTAL32K://, /**< CLK Source HW ALL XTAL */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_XTAL24M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_1;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_1;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_XTAL24M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_XTAL24MRC48MXTAL32K://, /**< CLK Source HW ALL XTAL RC48M*/
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC96M48M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_1;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_1;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC96M48M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_XTAL24MRC96MXTAL32K://, /**< CLK Source HW ALL XTAL RC96M*/
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC96M48M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_2;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_2;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC96M48M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_XTAL24MRC32K://, /**< CLK Source XTAL24M and RC32K */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_XTAL24M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_1;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_1;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_XTAL24M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_XTAL24MRC48MRC32K://, /**< CLK Source XTAL24M, RC48M and RC32K */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC96M48M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_1;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_1;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC96M48M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_XTAL24MRC96MRC32K://, /**< CLK Source XTAL24M, RC96M and RC32K */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC96M48M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_2;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_2;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC96M48M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_RC24M_XTAL32K://, /**< CLK Source HW RC(RC24M and XTAL32K) */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC24M1M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_1;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_1;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC24M1M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_RC24MRC48M_XTAL32K://, /**< CLK Source HW RC(RC48M and XTAL32K)  */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC96M48M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_1;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_1;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC96M48M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
		case SCU_SYSCLKSRC_HW_RC24MRC96M_XTAL32K://, /**< CLK Source HW RC(RC96M and XTAL32K)  */
			hsc_cfg->hscclk.hscclksrc = SCU_HSCCLKSRC_RC96M48M;
			hsc_cfg->hscclk.hscclkdiv = SCU_HSCCLKDIV_1;
			hsc_cfg->hscd12clksrc = SCU_HSCD12CLKSRC_HSC;
			hsc_cfg->i3chcdiv = SCU_HSCI3CHCLKDIV_2;
			hsc_cfg->sdiodiv = SCU_HSCSDIOCLKDIV_2;
			lsc_cfg->lscclksrc = SCU_LSCCLKSRC_RC96M48M;
			lsc_cfg->lscclkdiv = SCU_LSCCLKDIV_1;
			break;
	}
	return;
}

static void priv_pmu_gpio_cb(uint8_t group, uint8_t idx)
{
}

void app_pmu_enter_sleep1_aos(AppCfgCustGpio_t gpio_cfg, uint32_t sensor_rtc_ms, uint32_t sys_rtc_ms, uint8_t quickboot)
{
    PM_PD_VIDPRE_CFG_T cfg;
    SCU_LSC_CLK_CFG_T lsc_cfg;
    SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;
    
    hx_lib_pm_get_defcfg_bymode(&cfg, PM_MODE_PS_VID_ONLY_PREROLLING);
    cfg.bootromspeed.bootromclkfreq = SCU_PLL_FREQ_ENABLE;
    cfg.bootromspeed.pll_freq = 400000000;
    cfg.bootromspeed.cm55m_div = SCU_HSCCLKDIV_1;
    cfg.bootromspeed.cm55s_div = SCU_LSCCLKDIV_4;
    //cfg.bootromspeed.bootromclkfreq = SCU_PLL_FREQ_DISABLE;
    //cfg.bootromspeed.pll_freq = 0;
    //cfg.bootromspeed.cm55m_div = SCU_HSCCLKDIV_1;
    //cfg.bootromspeed.cm55s_div = SCU_LSCCLKDIV_1;
    
    cfg.cm55s_reset = SWREG_AON_PMUWAKE_CM55S_RERESET_YES;
    cfg.pmu_senint_mask = PM_IP_INT_MASK_ALL_UNMASK;
    cfg.sensor_type = PM_SENSOR_TIMING_FVLDLVLD_SHIFT;

    //cfg.support_debugdump = 1;
    
    #if 1
    cfg.pmu_pad_pa01_mask = PM_IP_INT_MASK;
    cfg.pmu_pad_pa23_mask = PM_IP_INT_MASK;
    
    for(uint32_t i = 0; i < gpio_cfg.act_wakupCPU_pin_cnt; i++)
    {
        if(gpio_cfg.wakeupCPU_int_pin[i] == HX_AON_GPIO_0 || gpio_cfg.wakeupCPU_int_pin[i] == HX_AON_GPIO_1)
            cfg.pmu_pad_pa01_mask = PM_IP_INT_MASK_ALL_UNMASK;
        else if(gpio_cfg.wakeupCPU_int_pin[i] == HX_SB_GPIO_0 || gpio_cfg.wakeupCPU_int_pin[i] == HX_SB_GPIO_1)
            cfg.pmu_pad_pa23_mask = PM_IP_INT_MASK_ALL_UNMASK;
    }
    #endif
    
    cfg.sensor_timer = sensor_rtc_ms;
    cfg.wdt_timer = sensor_rtc_ms+500;

    if(quickboot == 0)
    {
        cfg.skip_bootflow.sec_mem_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.first_bl_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_s_app_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_ns_app_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_s_app_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_ns_app_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_model_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_model_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_appcfg_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_appcfg_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.secure_debug_flag = SWREG_AON_NO_RETENTION;
    }
    
    #if 1
    dbg_printf(DBG_LESS_INFO, "bootromclkfreq=%u\n", cfg.bootromspeed.bootromclkfreq);
    dbg_printf(DBG_LESS_INFO, "pll_freq=%u\n", cfg.bootromspeed.pll_freq);
    dbg_printf(DBG_LESS_INFO, "cm55mdiv=%u\n", cfg.bootromspeed.cm55m_div);
    dbg_printf(DBG_LESS_INFO, "cm55sdiv=%u\n", cfg.bootromspeed.cm55s_div);
    dbg_printf(DBG_LESS_INFO, "sensor_timer=%u\n", cfg.sensor_timer);
    dbg_printf(DBG_LESS_INFO, "wdt_timer=%u\n", cfg.wdt_timer);
    dbg_printf(DBG_LESS_INFO, "nframeend_ctrl=%u\n", cfg.nframeend_ctrl);
    dbg_printf(DBG_LESS_INFO, "cm55s_reset=%u\n", cfg.cm55s_reset);
    dbg_printf(DBG_LESS_INFO, "pmu_rtc_mask=%u\n", cfg.pmu_rtc_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_pad_pa01_mask=%u\n", cfg.pmu_pad_pa01_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_pad_pa23_mask=%u\n", cfg.pmu_pad_pa23_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_i2cw_mask=%u\n", cfg.pmu_i2cw_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_timer_mask=%u\n", cfg.pmu_timer_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_cmp_mask=%u\n", cfg.pmu_cmp_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_ts_mask=%u\n", cfg.pmu_ts_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_senint_mask=%u\n", cfg.pmu_senint_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_mipii2c_noack_mask=%u\n", cfg.pmu_mipii2c_noack_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_anti_mask=%u\n", cfg.pmu_anti_mask);
    
    dbg_printf(DBG_LESS_INFO, "tcm_retention=%u\n", cfg.tcm_retention);
    dbg_printf(DBG_LESS_INFO, "hscsram_retention[0]=%u\n", cfg.hscsram_retention[0]);
    dbg_printf(DBG_LESS_INFO, "hscsram_retention[1]=%u\n", cfg.hscsram_retention[1]);
    dbg_printf(DBG_LESS_INFO, "hscsram_retention[2]=%u\n", cfg.hscsram_retention[2]);
    dbg_printf(DBG_LESS_INFO, "hscsram_retention[3]=%u\n", cfg.hscsram_retention[3]);
    dbg_printf(DBG_LESS_INFO, "lscsram_retention=%u\n", cfg.lscsram_retention);
    
    dbg_printf(DBG_LESS_INFO, "sec_mem_flag=%u\n", cfg.skip_bootflow.sec_mem_flag);
    dbg_printf(DBG_LESS_INFO, "first_bl_flag=%u\n", cfg.skip_bootflow.first_bl_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_s_app_flag=%u\n", cfg.skip_bootflow.cm55m_s_app_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_ns_app_flag=%u\n", cfg.skip_bootflow.cm55m_ns_app_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_s_app_flag=%u\n", cfg.skip_bootflow.cm55s_s_app_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_ns_app_flag=%u\n", cfg.skip_bootflow.cm55s_ns_app_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_model_flag=%u\n", cfg.skip_bootflow.cm55m_model_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_model_flag=%u\n", cfg.skip_bootflow.cm55s_model_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_appcfg_flag=%u\n", cfg.skip_bootflow.cm55m_appcfg_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_appcfg_flag=%u\n", cfg.skip_bootflow.cm55s_appcfg_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_s_app_rwdata_flag=%u\n", cfg.skip_bootflow.cm55m_s_app_rwdata_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_ns_app_rwdata_flag=%u\n", cfg.skip_bootflow.cm55m_ns_app_rwdata_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_s_app_rwdata_flag=%u\n", cfg.skip_bootflow.cm55s_s_app_rwdata_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_ns_app_rwdata_flag=%u\n", cfg.skip_bootflow.cm55s_ns_app_rwdata_flag);
    dbg_printf(DBG_LESS_INFO, "secure_debug_flag=%u\n", cfg.skip_bootflow.secure_debug_flag);

    dbg_printf(DBG_LESS_INFO, "fast_vpr=%u\n", cfg.fast_vpr);
    dbg_printf(DBG_LESS_INFO, "pmu_dcdc_outpin=%u\n", cfg.pmu_dcdc_outpin);
    dbg_printf(DBG_LESS_INFO, "mipi_lane_en=%u\n", cfg.mipi_lane_en);
    dbg_printf(DBG_LESS_INFO, "sensor_type=%u\n", cfg.sensor_type);
    dbg_printf(DBG_LESS_INFO, "support_debugdump=%u\n", cfg.support_debugdump);;
    #endif
    
    hx_lib_pm_cfg_set(&cfg, NULL, PM_MODE_PS_VID_ONLY_PREROLLING);
    
    dbg_printf(DBG_LESS_INFO, "gpio_cfg.act_wakupCPU_pin_cnt=%u\n", gpio_cfg.act_wakupCPU_pin_cnt);

    for(uint32_t i = 0; i < gpio_cfg.act_wakupCPU_pin_cnt; i++)
    {
        dbg_printf(DBG_LESS_INFO, "gpio_cfg.wakeupCPU_int_pin[%u]=0x%x\n", i, gpio_cfg.wakeupCPU_int_pin[i]);
        
        hx_drv_gpio_set_int_enable(gpio_cfg.wakeupCPU_int_pin[i], 0);
        hx_drv_gpio_set_int_type(gpio_cfg.wakeupCPU_int_pin[i], GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
        hx_drv_gpio_set_irq_handle(gpio_cfg.wakeupCPU_int_pin[i], GPIO_IRQ_HANDLE_CLR_INT);
        hx_drv_gpio_set_input(gpio_cfg.wakeupCPU_int_pin[i]);
        hx_drv_gpio_cb_register(gpio_cfg.wakeupCPU_int_pin[i], priv_pmu_gpio_cb);
        hx_drv_gpio_set_int_enable(gpio_cfg.wakeupCPU_int_pin[i], 1);
    }

    if(sys_rtc_ms > 0)
    {
        TIMER_CFG_T timer_cfg = {0};

        timer_cfg.period = sys_rtc_ms;
        timer_cfg.mode = TIMER_MODE_ONESHOT;
        timer_cfg.ctrl = TIMER_CTRL_PMU;
        timer_cfg.state = TIMER_STATE_PMU;
        
        dbg_printf(DBG_LESS_INFO, "hx_drv_timer_hw_stop\n");
        hx_drv_timer_hw_stop(APP_PMU_SYS_RTC_ID);
        
        dbg_printf(DBG_LESS_INFO, "hx_drv_timer_hw_start\n");
        hx_drv_timer_hw_start(APP_PMU_SYS_RTC_ID, &timer_cfg, NULL);
    }

    /*Use PMU lib to control HSC_CLK and LSC_CLK so set thoes parameter to 0*/
    memset(&hsc_cfg, 0, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
    memset(&lsc_cfg, 0, sizeof(SCU_LSC_CLK_CFG_T));
    
    /*Trigger to PMU mode*/
    #if 0
    hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
    #else
    priv_pmu_set_clk_cfg(SCU_SYSCLKSRC_HW_XTAL24MRC32K, &lsc_cfg, &hsc_cfg);

    dbg_printf(DBG_LESS_INFO, "hx_lib_pm_trigger\n");
    hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYAPP);
    #endif
}

static void priv_pmu_enter_novidpre(AppCfgCustGpio_t gpio_cfg, uint32_t sys_rtc_ms, uint8_t quickboot, uint8_t retention)
{
    PM_PD_NOVIDPRE_CFG_T cfg;
    SCU_LSC_CLK_CFG_T lsc_cfg;
    SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;

    hx_lib_pm_get_defcfg_bymode(&cfg, PM_MODE_PS_NOVID_PREROLLING);
    
    cfg.bootromspeed.bootromclkfreq = SCU_PLL_FREQ_ENABLE;
    cfg.bootromspeed.pll_freq = 400000000;
    cfg.bootromspeed.cm55m_div = SCU_HSCCLKDIV_1;
    cfg.bootromspeed.cm55s_div = SCU_LSCCLKDIV_4;
    //cfg.bootromspeed.bootromclkfreq = SCU_PLL_FREQ_DISABLE;
    //cfg.bootromspeed.pll_freq = 0;
    //cfg.bootromspeed.cm55m_div = SCU_HSCCLKDIV_1;
    //cfg.bootromspeed.cm55s_div = SCU_LSCCLKDIV_1;
    
    //cfg.mipi_lane_en = PMU_MIPI_LANE_ALL_DISABLE;
    //cfg.simo_pd_onoff = PM_SIMO_PD_ONOFF_OFF;
    //cfg.ioret = PM_CFG_PD_IORET_OFF;
    
    //cfg.support_debugdump = 1;

    #if 1
    cfg.pmu_pad_pa01_mask = PM_IP_INT_MASK;
    cfg.pmu_pad_pa23_mask = PM_IP_INT_MASK;

    for(uint32_t i = 0; i < gpio_cfg.act_wakupCPU_pin_cnt; i++)
    {
        if(gpio_cfg.wakeupCPU_int_pin[i] == HX_AON_GPIO_0 || gpio_cfg.wakeupCPU_int_pin[i] == HX_AON_GPIO_1)
            cfg.pmu_pad_pa01_mask = PM_IP_INT_MASK_ALL_UNMASK;
        else if(gpio_cfg.wakeupCPU_int_pin[i] == HX_SB_GPIO_0 || gpio_cfg.wakeupCPU_int_pin[i] == HX_SB_GPIO_1)
            cfg.pmu_pad_pa23_mask = PM_IP_INT_MASK_ALL_UNMASK;
    }
    #endif
    
    if(retention == 0)
    {
        cfg.tcm_retention = PM_MEM_RET_NO;
        cfg.hscsram_retention[0] = PM_MEM_RET_NO;
        cfg.hscsram_retention[1] = PM_MEM_RET_NO;
        cfg.hscsram_retention[2] = PM_MEM_RET_NO;
        cfg.hscsram_retention[3] = PM_MEM_RET_NO;
        cfg.lscsram_retention = PM_MEM_RET_NO;
    }

    if(quickboot == 0)
    {
        cfg.skip_bootflow.sec_mem_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.first_bl_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_s_app_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_ns_app_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_s_app_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_ns_app_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_model_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_model_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_appcfg_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_appcfg_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;
        cfg.skip_bootflow.secure_debug_flag = SWREG_AON_NO_RETENTION;
    }

    #if 1
    dbg_printf(DBG_LESS_INFO, "bootromclkfreq=%u\n", cfg.bootromspeed.bootromclkfreq);
    dbg_printf(DBG_LESS_INFO, "pll_freq=%u\n", cfg.bootromspeed.pll_freq);
    dbg_printf(DBG_LESS_INFO, "cm55mdiv=%u\n", cfg.bootromspeed.cm55m_div);
    dbg_printf(DBG_LESS_INFO, "cm55sdiv=%u\n", cfg.bootromspeed.cm55s_div);
    dbg_printf(DBG_LESS_INFO, "nframeend_ctrl=%u\n", cfg.nframeend_ctrl);
    dbg_printf(DBG_LESS_INFO, "cm55s_reset=%u\n", cfg.cm55s_reset);
    dbg_printf(DBG_LESS_INFO, "pmu_rtc_mask=%u\n", cfg.pmu_rtc_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_pad_pa01_mask=%u\n", cfg.pmu_pad_pa01_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_pad_pa23_mask=%u\n", cfg.pmu_pad_pa23_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_i2cw_mask=%u\n", cfg.pmu_i2cw_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_timer_mask=%u\n", cfg.pmu_timer_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_cmp_mask=%u\n", cfg.pmu_cmp_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_ts_mask=%u\n", cfg.pmu_ts_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_anti_mask=%u\n", cfg.pmu_anti_mask);
    
    dbg_printf(DBG_LESS_INFO, "tcm_retention=%u\n", cfg.tcm_retention);
    dbg_printf(DBG_LESS_INFO, "hscsram_retention[0]=%u\n", cfg.hscsram_retention[0]);
    dbg_printf(DBG_LESS_INFO, "hscsram_retention[1]=%u\n", cfg.hscsram_retention[1]);
    dbg_printf(DBG_LESS_INFO, "hscsram_retention[2]=%u\n", cfg.hscsram_retention[2]);
    dbg_printf(DBG_LESS_INFO, "hscsram_retention[3]=%u\n", cfg.hscsram_retention[3]);
    dbg_printf(DBG_LESS_INFO, "lscsram_retention=%u\n", cfg.lscsram_retention);
    
    dbg_printf(DBG_LESS_INFO, "sec_mem_flag=%u\n", cfg.skip_bootflow.sec_mem_flag);
    dbg_printf(DBG_LESS_INFO, "first_bl_flag=%u\n", cfg.skip_bootflow.first_bl_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_s_app_flag=%u\n", cfg.skip_bootflow.cm55m_s_app_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_ns_app_flag=%u\n", cfg.skip_bootflow.cm55m_ns_app_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_s_app_flag=%u\n", cfg.skip_bootflow.cm55s_s_app_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_ns_app_flag=%u\n", cfg.skip_bootflow.cm55s_ns_app_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_model_flag=%u\n", cfg.skip_bootflow.cm55m_model_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_model_flag=%u\n", cfg.skip_bootflow.cm55s_model_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_appcfg_flag=%u\n", cfg.skip_bootflow.cm55m_appcfg_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_appcfg_flag=%u\n", cfg.skip_bootflow.cm55s_appcfg_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_s_app_rwdata_flag=%u\n", cfg.skip_bootflow.cm55m_s_app_rwdata_flag);
    dbg_printf(DBG_LESS_INFO, "cm55m_ns_app_rwdata_flag=%u\n", cfg.skip_bootflow.cm55m_ns_app_rwdata_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_s_app_rwdata_flag=%u\n", cfg.skip_bootflow.cm55s_s_app_rwdata_flag);
    dbg_printf(DBG_LESS_INFO, "cm55s_ns_app_rwdata_flag=%u\n", cfg.skip_bootflow.cm55s_ns_app_rwdata_flag);
    dbg_printf(DBG_LESS_INFO, "secure_debug_flag=%u\n", cfg.skip_bootflow.secure_debug_flag);

    dbg_printf(DBG_LESS_INFO, "support_bootwithcap=%u\n", cfg.support_bootwithcap);
    dbg_printf(DBG_LESS_INFO, "pmu_dcdc_outpin=%u\n", cfg.pmu_dcdc_outpin);
    dbg_printf(DBG_LESS_INFO, "mipi_lane_en=%u\n", cfg.mipi_lane_en);
    dbg_printf(DBG_LESS_INFO, "ioret=%u\n", cfg.ioret);
    dbg_printf(DBG_LESS_INFO, "sensor_type=%u\n", cfg.sensor_type);
    dbg_printf(DBG_LESS_INFO, "simo_pd_onoff=%u\n", cfg.simo_pd_onoff);
    dbg_printf(DBG_LESS_INFO, "support_debugdump=%u\n", cfg.support_debugdump);;
    #endif

    hx_lib_pm_cfg_set(&cfg, NULL, PM_MODE_PS_NOVID_PREROLLING);
    
    dbg_printf(DBG_LESS_INFO, "gpio_cfg.act_wakupCPU_pin_cnt=%u\n", gpio_cfg.act_wakupCPU_pin_cnt);
    
    for(uint32_t i = 0; i < gpio_cfg.act_wakupCPU_pin_cnt; i++)
    {
        dbg_printf(DBG_LESS_INFO, "gpio_cfg.wakeupCPU_int_pin[%u]=0x%x\n", i, gpio_cfg.wakeupCPU_int_pin[i]);
        
        hx_drv_gpio_set_int_enable(gpio_cfg.wakeupCPU_int_pin[i], 0);
        hx_drv_gpio_set_int_type(gpio_cfg.wakeupCPU_int_pin[i], GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
        hx_drv_gpio_set_irq_handle(gpio_cfg.wakeupCPU_int_pin[i], GPIO_IRQ_HANDLE_CLR_INT);
        hx_drv_gpio_set_input(gpio_cfg.wakeupCPU_int_pin[i]);
        hx_drv_gpio_cb_register(gpio_cfg.wakeupCPU_int_pin[i], priv_pmu_gpio_cb);
        hx_drv_gpio_set_int_enable(gpio_cfg.wakeupCPU_int_pin[i], 1);
    }

    if(sys_rtc_ms > 0)
    {
        TIMER_CFG_T timer_cfg = {0};

        timer_cfg.period = sys_rtc_ms;
        timer_cfg.mode = TIMER_MODE_ONESHOT;
        timer_cfg.ctrl = TIMER_CTRL_PMU;
        timer_cfg.state = TIMER_STATE_PMU;
        
        dbg_printf(DBG_LESS_INFO, "hx_drv_timer_hw_stop\n");
        hx_drv_timer_hw_stop(APP_PMU_SYS_RTC_ID);
        
        dbg_printf(DBG_LESS_INFO, "hx_drv_timer_hw_start\n");
        hx_drv_timer_hw_start(APP_PMU_SYS_RTC_ID, &timer_cfg, NULL);
    }

    /*Use PMU lib to control HSC_CLK and LSC_CLK so set thoes parameter to 0*/
	memset(&hsc_cfg, 0, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
	memset(&lsc_cfg, 0, sizeof(SCU_LSC_CLK_CFG_T));
    
	/*Trigger to PMU mode*/
    #if 0
    hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
    #else
    priv_pmu_set_clk_cfg(SCU_SYSCLKSRC_HW_XTAL24MRC32K, &lsc_cfg, &hsc_cfg);
    
    dbg_printf(DBG_LESS_INFO, "hx_lib_pm_trigger\n");
    hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYAPP);
    #endif
}

void app_pmu_enter_sleep1(AppCfgCustGpio_t gpio_cfg, uint32_t sys_rtc_ms, uint8_t quickboot)
{
    priv_pmu_enter_novidpre(gpio_cfg, sys_rtc_ms, quickboot, 1);
}

void app_pmu_enter_sleep2(AppCfgCustGpio_t gpio_cfg, uint32_t sys_rtc_ms)
{
    priv_pmu_enter_novidpre(gpio_cfg, sys_rtc_ms, 0, 0);
}

void app_pmu_enter_shutdown(AppCfgCustGpio_t gpio_cfg)
{
    PM_DPD_CFG_T cfg;
    SCU_LSC_CLK_CFG_T lsc_cfg;
    SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;
    
    hx_lib_pm_get_defcfg_bymode(&cfg, PM_MODE_PS_DPD);
    
    cfg.bootromspeed.bootromclkfreq = SCU_PLL_FREQ_ENABLE;
    cfg.bootromspeed.pll_freq = 400000000;
    cfg.bootromspeed.cm55m_div = SCU_HSCCLKDIV_1;
    cfg.bootromspeed.cm55s_div = SCU_LSCCLKDIV_4;
    //cfg.bootromspeed.bootromclkfreq = SCU_PLL_FREQ_DISABLE;
    //cfg.bootromspeed.pll_freq = 0;
    //cfg.bootromspeed.cm55m_div = SCU_HSCCLKDIV_1;
    //cfg.bootromspeed.cm55s_div = SCU_LSCCLKDIV_1;
    
    //cfg.support_debugdump = 1;
    
    cfg.pmu_pad_pa0_mask = PM_IP_INT_MASK;
    cfg.pmu_pad_pa1_mask = PM_IP_INT_MASK;

    dbg_printf(DBG_LESS_INFO, "gpio_cfg.act_wakupCPU_pin_cnt=%u\n", gpio_cfg.act_wakupCPU_pin_cnt);
    
    for(uint32_t i = 0; i < gpio_cfg.act_wakupCPU_pin_cnt; i++)
    {
        dbg_printf(DBG_LESS_INFO, "gpio_cfg.wakeupCPU_int_pin[%u]=0x%x\n", i, gpio_cfg.wakeupCPU_int_pin[i]);
        
        if(gpio_cfg.wakeupCPU_int_pin[i] == HX_AON_GPIO_0)
            cfg.pmu_pad_pa0_mask = PM_IP_INT_MASK_ALL_UNMASK;
        else if(gpio_cfg.wakeupCPU_int_pin[i] == HX_AON_GPIO_1)
            cfg.pmu_pad_pa1_mask = PM_IP_INT_MASK_ALL_UNMASK;
    }
    
    #if 1
    dbg_printf(DBG_LESS_INFO, "bootromclkfreq=%u\n", cfg.bootromspeed.bootromclkfreq);
    dbg_printf(DBG_LESS_INFO, "pll_freq=%u\n", cfg.bootromspeed.pll_freq);
    dbg_printf(DBG_LESS_INFO, "cm55mdiv=%u\n", cfg.bootromspeed.cm55m_div);
    dbg_printf(DBG_LESS_INFO, "cm55sdiv=%u\n", cfg.bootromspeed.cm55s_div);
    dbg_printf(DBG_LESS_INFO, "cm55s_reset=%u\n", cfg.cm55s_reset);
    dbg_printf(DBG_LESS_INFO, "pmu_rtc_mask=%u\n", cfg.pmu_rtc_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_anti_mask=%u\n", cfg.pmu_anti_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_pad_pa0_mask=%u\n", cfg.pmu_pad_pa0_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_pad_pa1_mask=%u\n", cfg.pmu_pad_pa1_mask);
    dbg_printf(DBG_LESS_INFO, "pmu_dcdc_outpin=%u\n", cfg.pmu_dcdc_outpin);
    dbg_printf(DBG_LESS_INFO, "support_debugdump=%u\n", cfg.support_debugdump);;
    #endif

    hx_lib_pm_cfg_set(&cfg, NULL, PM_MODE_PS_DPD);

    /*Use PMU lib to control HSC_CLK and LSC_CLK so set thoes parameter to 0*/
	memset(&hsc_cfg, 0, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
	memset(&lsc_cfg, 0, sizeof(SCU_LSC_CLK_CFG_T));
    
	/*Trigger to PMU mode*/
    #if 0
    hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
    #else
    priv_pmu_set_clk_cfg(SCU_SYSCLKSRC_HW_XTAL24MRC32K, &lsc_cfg, &hsc_cfg);

    dbg_printf(DBG_LESS_INFO, "hx_lib_pm_trigger\n");
    hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYAPP);
    #endif
}


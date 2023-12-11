#ifdef HM11B1_MONO

#include "hx_drv_hxautoi2c_mst.h"

//#include "hx_drv_CIS_common.h"
//#include "sensor_dp_lib.h"

#include "app_datapath.h"
#include "app_cisdp_cfg.h"

#include "WE2_debug.h"

static HX_CIS_SensorSetting_t HM11B1_init_setting[] = {
//#include "HM11B1_setA_1296x816-320x200@60_setB_320x200-80x50@240_GatedByFrame_CXT_MIPI384_MIPI_FW+_R1.i"
//#include "HM11B1_setA_1296x816-640x400@30_setB_640x400-320x200@60_GatedByFrame_CXT_MIPI384_MIPI_FW+_R1.i"
//#include "HM11B1_setA_1296x816-640x400@30_setB_640x400-320x200@60_GatedByFrame_CXT_MIPI384_MIPI_FW+_TP_R1.i"
#include "HM11B1_setA_1296x816-640x400@30_setB_640x400-320x200@60_GatedByFrame_CXT_MIPI384_MIPI_FW+_AEon_R1.i"
};
//
static HX_CIS_SensorSetting_t HM11B1_init_setting_aos[] = {
#include "HM11B1_setA_Bin2_640x400_setB_Bin8_160x100_Serial_FRC_On_FW+_R1.i"
};

static HX_CIS_SensorSetting_t HM11B1_stream_on[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x01},
};

static HX_CIS_SensorSetting_t HM11B1_stream_off[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x00},
};

static HX_CIS_SensorSetting_t HM11B1_stream_standby[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x02},
};

#if (CIS_ENABLE_HX_AUTOI2C != 0x00)
void app_cisdp_set_hxautoi2c(APP_INIT_TYPE_E type)
{
	//set to synopsys IP, change to Himax IP
	dbg_printf(DBG_LESS_INFO, "INIT AUTOI2C\n");
	HXAUTOI2CHC_STATIC_CFG_T scfg;
	HXAUTOI2CHC_INT_CFG_T icfg;
	HXAUTOI2CHC_CMD_CFG_T trig_cfg;
	HXAUTOI2CHC_CMD_CFG_T stop_cfg;

	scfg.slaveid = CIS_I2C_ID;

	scfg.clkdiv = HXAUTOI2C_SCL_CLK_DIV_60;
	if(type == APP_INIT_TYPE_AOS)
		scfg.cmdtype = HXAUTOI2CHC_CMD_TRIG_STOP;
	else
		scfg.cmdtype = HXAUTOI2CHC_CMD_TRIG_STOP;

	scfg.trig_ctrl_sw = HXAUTOI2CHC_CMD_CTRL_BY_HW;
	scfg.stop_ctrl_sw = HXAUTOI2CHC_CMD_CTRL_BY_HW;

	icfg.trig_int_loc = HXAUTOI2CHC_INT_LOC_AFT_CMD1;
	icfg.stop_int_loc = HXAUTOI2CHC_INT_LOC_AFT_CMD1;
	icfg.trig_delay = 0;
	icfg.stop_delay = 0;

	trig_cfg.cmd_num = HXAUTOI2CHC_CMD_NUM_1;
	trig_cfg.cmd1_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd2_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd3_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd4_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.delay01 = 0;
	trig_cfg.delay12 = 0x100;
	trig_cfg.delay23 = 0x100;
	trig_cfg.delay34 = 0x100;

	stop_cfg.cmd_num = HXAUTOI2CHC_CMD_NUM_1;
	stop_cfg.cmd1_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd2_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd3_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd4_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.delay01 = 0;
	stop_cfg.delay12 = 0x100;
	stop_cfg.delay23 = 0x100;
	stop_cfg.delay34 = 0x100;

	sensordplib_autoi2c_cfg(scfg, icfg, trig_cfg, stop_cfg);


	HXAUTOI2CHC_CMD_T trig_cmd1, trig_cmd2, trig_cmd3, trig_cmd4;

	if(type == APP_INIT_TYPE_AOS)
	{
		trig_cmd1.byte1 = (HM11B1_stream_on[0].RegAddree >> 8 & 0xFF);
		trig_cmd1.byte2 = (HM11B1_stream_on[0].RegAddree & 0xFF);
		trig_cmd1.byte3 = HM11B1_stream_on[0].Value;
		trig_cmd1.byte4 = 0x00;
	}
	else
	{
		trig_cmd1.byte1 = (HM11B1_stream_on[0].RegAddree >> 8 & 0xFF);
		trig_cmd1.byte2 = (HM11B1_stream_on[0].RegAddree & 0xFF);
		trig_cmd1.byte3 = HM11B1_stream_on[0].Value;
		trig_cmd1.byte4 = 0x00;
	}

	trig_cmd2.byte1 = trig_cmd2.byte2 = trig_cmd2.byte3 = trig_cmd2.byte4 = 0x11;
	trig_cmd3.byte1 = trig_cmd3.byte2 = trig_cmd3.byte3 = trig_cmd3.byte4 = 0x22;
	trig_cmd4.byte1 = trig_cmd4.byte2 = trig_cmd4.byte3 = trig_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_trigcmd(trig_cmd1, trig_cmd2, trig_cmd3, trig_cmd4);

	HXAUTOI2CHC_CMD_T stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4;

	if(type == APP_INIT_TYPE_AOS)
	{
		stop_cmd1.byte1 = 0x00;
		stop_cmd1.byte2 = 0x00;
		stop_cmd1.byte3 = 0x00;
		stop_cmd1.byte4 = 0x00;
	}
	else
	{
		stop_cmd1.byte1 = (HM11B1_stream_off[0].RegAddree >> 8 & 0xFF);
		stop_cmd1.byte2 = (HM11B1_stream_off[0].RegAddree & 0xFF);
		stop_cmd1.byte3 = HM11B1_stream_off[0].Value;
		stop_cmd1.byte4 = 0x00;
	}

	stop_cmd2.byte1 = stop_cmd2.byte2 = stop_cmd2.byte3 = stop_cmd2.byte4 = 0x11;
	stop_cmd3.byte1 = stop_cmd3.byte2 = stop_cmd3.byte3 = stop_cmd3.byte4 = 0x22;
	stop_cmd4.byte1 = stop_cmd4.byte2 = stop_cmd4.byte3 = stop_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_stopcmd(stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4);
}
#endif

int app_cisdp_datapath_init(const app_dp_cfg_t* dp_init_cfg)
{
    #if (CIS_ENABLE_MIPI_INF != 0x00)
    //setup MIPI RX
	app_set_mipi_csirx_enable();
    #endif
    
    if(dp_init_cfg->init_type == APP_INIT_TYPE_VIDEO_STREAM)
    {
        #if ((CIS_ENABLE_MIPI_INF != 0x00) && (IC_VERSION >= 30))
        {
    	    sensordplib_set_sensorctrl_inp_wi_crop_bin(dp_init_cfg->sensor_type, dp_init_cfg->stream_type,
    			dp_init_cfg->sensor_width, dp_init_cfg->sensor_height, dp_init_cfg->inp_subsample_type, dp_init_cfg->crop, dp_init_cfg->inp_bin);
        }
        #else
        {
        	sensordplib_set_sensorctrl_inp(dp_init_cfg->sensor_type, dp_init_cfg->stream_type, 
                dp_init_cfg->sensor_width, dp_init_cfg->sensor_height, dp_init_cfg->inp_subsample_type);
        	hx_drv_inp1bitparser_set_tg2utg(dp_init_cfg->sclk_utg);
        }
        #endif

    }
    else //AOS
    {
    	sensordplib_set_sensorctrl_inp(dp_init_cfg->sensor_type, dp_init_cfg->stream_type, dp_init_cfg->sensor_width,
            dp_init_cfg->sensor_height, dp_init_cfg->inp_subsample_type);
    	hx_drv_inp1bitparser_set_tg2utg(dp_init_cfg->sclk_utg);
    }
    
    return 0;
}

int app_cisdp_sensor_init(const app_dp_cfg_t* dp_init_cfg)
{
    dbg_printf(DBG_LESS_INFO, "cis_hm11b1_init \r\n");

    /*
     * common CIS init
     */
    hx_drv_cis_init((CIS_XHSHUTDOWN_INDEX_E)dp_init_cfg->xshutdown_pin, dp_init_cfg->mclk_div);
    dbg_printf(DBG_LESS_INFO, "xshutdown_pin=%d, mclk_div=%d\n",dp_init_cfg->xshutdown_pin, dp_init_cfg->mclk_div);
    //hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_NONAOS);
    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
    hx_drv_sensorctrl_set_xSleep(1);
    dbg_printf(DBG_LESS_INFO, "hx_drv_sensorctrl_set_xSleep(1)\n");

    /*
     * off stream before init sensor
     */
    if(hx_drv_cis_setRegTable(HM11B1_stream_off, HX_CIS_SIZE_N(HM11B1_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 off by app fail \r\n");
        return -1;
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 off by app \n");
        return -1;
    }

    if(dp_init_cfg->init_type == APP_INIT_TYPE_VIDEO_STREAM)
    {
    	if(hx_drv_cis_setRegTable(HM11B1_init_setting, HX_CIS_SIZE_N(HM11B1_init_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    	{
    		dbg_printf(DBG_LESS_INFO, "HM11B1 Init Stream by app fail \r\n");
    		return -1;
    	}
    	else
    	{
    		dbg_printf(DBG_LESS_INFO, "HM11B1 Init Stream by app \n");
    	}
    }
    else
    {
    	if(hx_drv_cis_setRegTable(HM11B1_init_setting_aos, HX_CIS_SIZE_N(HM11B1_init_setting_aos, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    	{
    		dbg_printf(DBG_LESS_INFO, "HM11B1 Init AOS by app fail \r\n");
    		return -1;
    	}
    	else
    	{
    		dbg_printf(DBG_LESS_INFO, "HM11B1 Init AOS by app \n");
    	}
    }

    HX_CIS_SensorSetting_t HM11B1_mirror_setting[] = {
            {HX_CIS_I2C_Action_W, 0x0101, CIS_MIRROR_SETTING},
    };

    if(hx_drv_cis_setRegTable(HM11B1_mirror_setting, HX_CIS_SIZE_N(HM11B1_mirror_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 Init Mirror 0x%02X by app fail \r\n", HM11B1_mirror_setting[0].Value);
        return -1;
    }
    else
    {
        #if (CIS_MIRROR_SETTING == 0x01)
    	dbg_printf(DBG_LESS_INFO, "HM11B1 Init Horizontal Mirror by app \n");
        #elif (CIS_MIRROR_SETTING == 0x02)
    	dbg_printf(DBG_LESS_INFO, "HM11B1 Init Vertical Mirror by app \n");
        #elif (CIS_MIRROR_SETTING == 0x03)
    	dbg_printf(DBG_LESS_INFO, "HM11B1 Init Horizontal & Vertical Mirror by app \n");
        #else
    	dbg_printf(DBG_LESS_INFO, "HM11B1 Init Mirror Off by app \n");
        #endif
    }

    return 0;
}

void app_cisdp_sensor_start(void)
{
    /*
     * Default Stream On
     */
    if(hx_drv_cis_setRegTable(HM11B1_stream_on, HX_CIS_SIZE_N(HM11B1_stream_on, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 on by app fail \r\n");
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 on by app \n");
    }
}

void app_cisdp_sensor_start_AOS(void)
{
    /*
     * Default Stream StandBy
     */
    if(hx_drv_cis_setRegTable(HM11B1_stream_standby, HX_CIS_SIZE_N(HM11B1_stream_standby, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 Standby by app fail \r\n");
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 Standby by app \n");
    }
}

void app_cisdp_sensor_stop()
{
    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();

    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
    hx_drv_sensorctrl_set_xSleep(1);
    if(hx_drv_cis_setRegTable(HM11B1_stream_off, HX_CIS_SIZE_N(HM11B1_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 off by app fail \r\n");
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "HM11B1 off by app \n");
    }
    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_SC);
    hx_drv_sensorctrl_set_xSleep(0);

	#if (CIS_ENABLE_HX_AUTOI2C != 0x00)
    //disable hxauotoi2c
    sensordplib_autoi2c_disable();
    dbg_printf(DBG_LESS_INFO, "hxauto i2c disable \n");
	#endif

    #if (CIS_ENABLE_MIPI_INF != 0x00)
    app_set_mipi_csirx_disable();
    #endif
}

#endif //HM11B1_MONO


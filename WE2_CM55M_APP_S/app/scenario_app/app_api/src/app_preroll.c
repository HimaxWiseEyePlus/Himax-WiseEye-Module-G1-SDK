#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "WE2_debug.h"

#include "hx_drv_edm.h"
#include "hx_drv_pmu.h"
#include "hx_drv_timer.h"
#include "sensor_dp_lib.h"

#include "powermode.h"

#include "app_api.h"
#include "app_pmu.h"
#include "app_datapath.h"
#include "app_cisdp_cfg.h"
#include "app_preroll.h"

#include "app_xdma_cfg.h"

#define DBG_MSG

static volatile app_auto_preroll_config_t g_preroll_config __attribute__ ((section(".preroll_data_arena"), aligned(4)))= {0};
static volatile app_auto_preroll_status_t g_auto_preroll_status __attribute__ ((section(".preroll_data_arena"), aligned(4)))= {0};

int8_t priv_auto_preroll_load_preroll_status(void);

#if defined(AUTO_PREROLL_CDM)
static uint32_t g_frame_ready = 0;

static void priv_brief_dp_cb(SENSORDPLIB_STATUS_E event)
{
    switch(event)
    {
        case SENSORDPLIB_STATUS_XDMA_FRAME_READY:
            dbg_printf(DBG_LESS_INFO, "FRAME_READY\n");
            g_frame_ready = 1;
            break;
        default:
            dbg_printf(DBG_LESS_INFO, "event = %d\n", event);
            break;
    }
}
#endif

static void priv_dump_preroll_status(void)
{
    dbg_printf(DBG_LESS_INFO, "\033[33m\n\n\n");
    dbg_printf(DBG_LESS_INFO, "slot_size=0x%x\n", g_auto_preroll_status.slot_size);
    dbg_printf(DBG_LESS_INFO, "total_slot=%u\n", g_auto_preroll_status.total_slot);
    dbg_printf(DBG_LESS_INFO, "first_frame_cap=%u\n", g_auto_preroll_status.first_frame_cap);
    dbg_printf(DBG_LESS_INFO, "next_frame_no=%u\n", g_auto_preroll_status.next_frame_no);
    dbg_printf(DBG_LESS_INFO, "start_frame_no=%u\n", g_auto_preroll_status.start_frame_no);
    dbg_printf(DBG_LESS_INFO, "loop_flag=%u\n", g_auto_preroll_status.loop_flag);
    
    for(uint8_t i=0;i<g_auto_preroll_status.total_slot;i++)
    {
        uint32_t data_addr;
        uint32_t *data_size;

        data_addr = (uint32_t)(g_auto_preroll_status.preroll_data.wdma2_data + (i*g_auto_preroll_status.slot_size));
        data_size = (uint32_t*)(g_auto_preroll_status.preroll_data.jpeg_auto_fill_data + (i*4));
        
        dbg_printf(DBG_LESS_INFO, "[%d] data_size=0x%x,data_addr=0x%x\n", i, *data_size, data_addr);
    }
    dbg_printf(DBG_LESS_INFO, "\033[0m\n\n\n");
}

int8_t app_auto_preroll_init(app_auto_preroll_config_t *config)
{
    g_preroll_config.wdma1_data = config->wdma1_data;
    g_preroll_config.wdma2_data = config->wdma2_data;
    g_preroll_config.wdma3_data = config->wdma3_data;
    g_preroll_config.jpeg_auto_fill_data = config->jpeg_auto_fill_data;
    g_preroll_config.slot_size = config->slot_size;
    g_preroll_config.slot_num = config->slot_num;
	
    hx_CleanDCache_by_Addr((volatile void *)&g_preroll_config, sizeof(app_auto_preroll_config_t));

	priv_auto_preroll_load_preroll_status();

    return API_SUCC;
}

void app_auto_preroll_set_datapath(void)
{
    uint32_t sensor_init_required = 0;
    app_dp_cfg_t app_dp_cfg;
    
    #ifdef DBG_MSG
    dbg_printf(DBG_LESS_INFO, "app_auto_preroll_set_datapath\n");
    #endif

	/*take control back from SoC, do reset*/
	sensordplib_stop_capture();
	sensordplib_start_swreset();
	sensordplib_stop_swreset_WoSensorCtrl();
    
    app_dp_get_def_init_cfg(&app_dp_cfg);
    app_cisdp_sensor_init(&app_dp_cfg);
    app_cisdp_sensor_start();
	
    #if defined(AUTO_PREROLL_JPEG) /*jpeg*/
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T jpeg_cfg;
    
    hx_drv_hw5x5_get_defCfg(&hw5x5_cfg);
    hw5x5_cfg.hw5x5_path = DP_HW5X5_PATH;
    hw5x5_cfg.demos_color_mode = DP_HW5X5_DEMOS_COLORMODE;
    hw5x5_cfg.demos_pattern_mode = DP_HW5X5_DEMOS_PATTERN;
    hw5x5_cfg.demoslpf_roundmode = DP_HW5X5_DEMOSLPF_ROUNDMODE;
    hw5x5_cfg.demos_bndmode = DP_HW5X5_DEMOS_BNDMODE;
    hw5x5_cfg.hw55_crop_stx = DP_HW5X5_CROP_START_X;
    hw5x5_cfg.hw55_crop_sty = DP_HW5X5_CROP_START_Y;
    hw5x5_cfg.hw55_in_width = DP_HW5X5_CROP_WIDTH;
    hw5x5_cfg.hw55_in_height = DP_HW5X5_CROP_HEIGHT;

    hx_drv_jpeg_get_defCfg(&jpeg_cfg);
    jpeg_cfg.jpeg_path = DP_JPEG_PATH;
    jpeg_cfg.enc_width = DP_JPEG_ENC_WIDTH;
    jpeg_cfg.enc_height = DP_JPEG_ENC_HEIGHT;
    jpeg_cfg.jpeg_enctype = DP_JPEG_ENCTYPE;
    jpeg_cfg.jpeg_encqtable = DP_JPEG_ENCQTABLE;
    
    sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
    
    sensordplib_set_hw5x5_jpeg_wdma2_start_no(
                hw5x5_cfg,
                jpeg_cfg,
                g_preroll_config.slot_num,
                g_auto_preroll_status.next_frame_no,
                NULL);
    #elif 0 //defined(AUTO_PREROLL_RAW)/*raw*/

    sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
    
    sensordplib_set_raw_wdma2_start_no(
                app_dp_cfg.inp_out_width,
                app_dp_cfg.inp_out_height,
                g_preroll_config.slot_num,
                g_auto_preroll_status.next_frame_no,
                NULL);
    #elif defined(AUTO_PREROLL_CDM)/*CDM*/
    HW2x2_CFG_T hw2x2_cfg;
    CDM_CFG_T cdm_cfg;
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T jpeg_cfg;

    hx_drv_hw2x2_get_defCfg(&hw2x2_cfg);
    hw2x2_cfg.hw2x2_path = DP_HW2X2_PATH;
    hw2x2_cfg.hw_22_process_mode = HW2x2_MODE_BINNING;
    hw2x2_cfg.hw_22_mono_round_mode = HW2x2_ROUNDMODE_FLOOR;
    hw2x2_cfg.hw_22_crop_stx = DP_HW2X2_CROP_START_X;
    hw2x2_cfg.hw_22_crop_sty = DP_HW2X2_CROP_START_Y;
    hw2x2_cfg.hw_22_in_width = DP_HW2X2_CROP_WIDTH;
    hw2x2_cfg.hw_22_in_height = DP_HW2X2_CROP_HEIGHT;

    hx_drv_cdm_get_defCfg(&cdm_cfg);
    cdm_cfg.cdm_enable = DP_CDM_ENABLE;
    cdm_cfg.init_map_flag = DP_CDM_INIT_MAP_FLAG;
    cdm_cfg.cpu_activeflag = DP_CDM_CPU_ACTIVEFLAG;
    cdm_cfg.meta_dump = DP_CDM_META_DUMP;
    cdm_cfg.ht_packing = DP_CDM_HT_PACKING;
    cdm_cfg.cdm_min_allow_dis = DP_CDM_MIN_ALLOW_DIS;
    cdm_cfg.cdm_tolerance = DP_CDM_TOLERANCE;
    cdm_cfg.cdm_reactance = DP_CDM_REACTANCE;
    cdm_cfg.cdm_relaxation = DP_CDM_RELAXATION;
    cdm_cfg.cdm_eros_th = DP_CDM_EROS_TH;
    cdm_cfg.cdm_num_ht_th = DP_CDM_NUM_HT_TH;
    cdm_cfg.cdm_num_ht_vect_th_x = DP_CDM_NUM_HT_VECT_TH_X;
    cdm_cfg.cdm_num_ht_vect_th_y = DP_CDM_NUM_HT_VECT_TH_Y;
    cdm_cfg.cdm_num_cons_ht_bin_th_x = DP_CDM_NUM_CONS_HT_BIN_TH_X;
    cdm_cfg.cdm_num_cons_ht_bin_th_y = DP_CDM_NUM_CONS_HT_BIN_TH_Y;
    cdm_cfg.cdm_crop_stx = DP_CDM_IN_START_X;
    cdm_cfg.cdm_crop_sty = DP_CDM_IN_START_Y;
    cdm_cfg.cdm_in_width = DP_CDM_IN_WIDTH;
    cdm_cfg.cdm_in_height = DP_CDM_IN_HEIGHT;

    hx_drv_hw5x5_get_defCfg(&hw5x5_cfg);
    hw5x5_cfg.hw5x5_path = DP_HW5X5_PATH;
    hw5x5_cfg.demos_color_mode = DP_HW5X5_DEMOS_COLORMODE;
    
    hw5x5_cfg.demos_bndmode = DP_HW5X5_DEMOS_BNDMODE;
    hw5x5_cfg.hw55_crop_stx = DP_HW5X5_CROP_START_X;
    hw5x5_cfg.hw55_crop_sty = DP_HW5X5_CROP_START_Y;
    hw5x5_cfg.hw55_in_width = DP_HW5X5_CROP_WIDTH;
    hw5x5_cfg.hw55_in_height = DP_HW5X5_CROP_HEIGHT;
    //hw5x5_cfg.wdma3_ch1_only = true;
    
    hx_drv_jpeg_get_defCfg(&jpeg_cfg);
    jpeg_cfg.jpeg_path = DP_JPEG_PATH;
    jpeg_cfg.enc_width = DP_JPEG_ENC_WIDTH;
    jpeg_cfg.enc_height = DP_JPEG_ENC_HEIGHT;
    jpeg_cfg.jpeg_enctype = DP_JPEG_ENCTYPE;
    jpeg_cfg.jpeg_encqtable = DP_JPEG_ENCQTABLE;
    
    g_frame_ready = 0;
    
    sensordplib_set_INT1_HWACC_start_no(
        hw2x2_cfg,
        cdm_cfg,
        hw5x5_cfg,
        jpeg_cfg,
        g_preroll_config.slot_num,
        g_auto_preroll_status.next_frame_no,
        priv_brief_dp_cb);

	sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
    sensordplib_set_sensorctrl_start();
    
    while(1)
    {
        //#define TIMEOUT_CNT 300
        //uint32_t count = 0;
        
        if(g_frame_ready == 1)
        {
            uint8_t frame_no = 0;
            uint8_t next_frame_no = 0;
            uint8_t buffer_no = 0;
            uint32_t jpeg_enc_addr = 0;
            uint32_t jpeg_enc_filesize = 0;
            uint32_t jpeg_enc_filesize_real = 0;
			
            hx_drv_xdma_get_WDMA2_bufferNo(&buffer_no);
            hx_drv_xdma_get_WDMA2NextFrameIdx(&next_frame_no);
            dbg_printf(DBG_LESS_INFO, "buffer_no=%u, next_frame_no=%u\n",buffer_no, next_frame_no);
			
            frame_no = (buffer_no + next_frame_no - 1) % buffer_no;
			dbg_printf(DBG_LESS_INFO, "frame_no=%u\n",frame_no);
            
            hx_drv_jpeg_get_EncOutRealMEMSize(&jpeg_enc_filesize_real);
            dbg_printf(DBG_LESS_INFO, "jpeg_enc_filesize_real=0x%x\n", jpeg_enc_filesize_real);
            
			hx_drv_jpeg_get_FillFileSizeToMem(frame_no, (uint32_t)g_preroll_config.jpeg_auto_fill_data, &jpeg_enc_filesize);
			dbg_printf(DBG_LESS_INFO, "jpeg_size=0x%x\n",jpeg_enc_filesize);
			
            hx_drv_jpeg_get_MemAddrByFrameNo(frame_no, (uint32_t)g_preroll_config.wdma2_data, &jpeg_enc_addr);
            dbg_printf(DBG_LESS_INFO, "addr=0x%x\n", jpeg_enc_addr);
            
			//uint32_t *pjpeg_enc_filesize = 0;
            //pjpeg_enc_filesize = (uint32_t *)(g_preroll_config.jpeg_auto_fill_data + (4 * frame_no));
            //*pjpeg_enc_filesize = jpeg_enc_filesize_real;
			
			if( jpeg_enc_filesize_real != jpeg_enc_filesize)
			{
				dbg_printf(DBG_LESS_INFO, "jpeg_enc_filesize_real(0x%08X) != jpeg_enc_filesize(0x%08X)\n", jpeg_enc_filesize_real, jpeg_enc_filesize);

				hx_drv_jpeg_set_FillFileSizeToMem(next_frame_no, (uint32_t)g_preroll_config.jpeg_auto_fill_data);
				hx_CleanDCache_by_Addr((volatile void *)g_preroll_config.jpeg_auto_fill_data, JPEG_AUTOFILL_SIZE/*100*/*sizeof(uint8_t));
			}
			
			app_auto_preroll_retain_frame_updadte_status(); /*retain this latest one jpeg frame in buffer and update next_frame_no*/
            break;
        }
        #if 0
        else if(count++ > TIMEOUT_CNT)
        {
            dbg_printf(DBG_LESS_INFO, "get frame ready timeout\n");
            break;
        }
        #endif
        dbg_printf(DBG_LESS_INFO, ".");
        board_delay_ms(10);
    }

    dbg_printf(DBG_LESS_INFO,"set init map off and CPU on \n");

    if(hx_drv_cdm_set_initmapFlag(CDM_INIMAP_FLAG_OFF) != CDM_NO_ERROR)
    {
        dbg_printf(DBG_LESS_INFO,"hx_drv_cdm_set_initmapFlag fail\n");
    }

    if(hx_drv_cdm_set_CPUActiveFlag(CDM_CPU_ACTFLAG_SLEEP) != CDM_NO_ERROR)
    {
        dbg_printf(DBG_LESS_INFO,"hx_drv_cdm_set_CPUActiveFlag fail\n");
    }
    #endif
}

int8_t priv_auto_preroll_load_preroll_status(void)
{
    uint8_t loop_flag;
    uint8_t next_frame_no;
    uint8_t total_slot;
    uint8_t first_frame_cap;
    
    #ifdef DBG_MSG
    dbg_printf(DBG_LESS_INFO, "priv_auto_preroll_load_preroll_status\n");
    #endif

    #ifdef DBG_MSG
    priv_dump_preroll_status();
    #endif
    
    hx_drv_xdma_get_WDMA2_bufferNo(&total_slot);
    hx_drv_xdma_get_WDMA2FirstFrameCapflag(&first_frame_cap);
    hx_drv_xdma_get_WDMA2NextFrameIdx(&next_frame_no);
    hx_drv_xdma_get_WDMA2_loopflag(&loop_flag);
    
    #ifdef DBG_MSG
    dbg_printf(DBG_LESS_INFO, "loop_flag=%d,next_frame_no=%d,total_slot=%d,first_frame_cap=%d\n", loop_flag, next_frame_no, total_slot, first_frame_cap);
    #endif

    g_auto_preroll_status.slot_size = g_preroll_config.slot_size;
    g_auto_preroll_status.total_slot = g_preroll_config.slot_num;
    
    g_auto_preroll_status.preroll_data.wdma2_data = g_preroll_config.wdma2_data;
    g_auto_preroll_status.preroll_data.jpeg_auto_fill_data = g_preroll_config.jpeg_auto_fill_data;
    
    if(first_frame_cap == 0) //no update
        return API_SUCC;

    g_auto_preroll_status.first_frame_cap = first_frame_cap;

    if(g_auto_preroll_status.loop_flag)
    {
        g_auto_preroll_status.start_frame_no = next_frame_no;
    }
    else if(loop_flag || next_frame_no == g_auto_preroll_status.next_frame_no)
    {
        g_auto_preroll_status.start_frame_no = next_frame_no;
        g_auto_preroll_status.loop_flag = 1;
    }
    else if(g_auto_preroll_status.next_frame_no > g_auto_preroll_status.start_frame_no)
    {
        if((next_frame_no >= g_auto_preroll_status.start_frame_no) && (next_frame_no < g_auto_preroll_status.next_frame_no))
        {
            g_auto_preroll_status.start_frame_no = next_frame_no;
            g_auto_preroll_status.loop_flag = 1;
        }
    }
    else if(g_auto_preroll_status.next_frame_no < g_auto_preroll_status.start_frame_no)
    {
        if(next_frame_no >= g_auto_preroll_status.start_frame_no)
        {
            g_auto_preroll_status.start_frame_no = next_frame_no;
            g_auto_preroll_status.loop_flag = 1;
        }
    }

    g_auto_preroll_status.next_frame_no = next_frame_no;
	
    hx_CleanDCache_by_Addr((volatile void *)&g_auto_preroll_status, sizeof(app_auto_preroll_status_t));
    
    #ifdef DBG_MSG
    priv_dump_preroll_status();
    #endif
    
    return API_SUCC;
}

int8_t app_auto_preroll_get_status(app_auto_preroll_status_t *status)
{
    #ifdef DBG_MSG
    dbg_printf(DBG_LESS_INFO, "app_auto_preroll_get_status\n");
    #endif
    
    status->total_slot = g_auto_preroll_status.total_slot;
    status->slot_size = g_auto_preroll_status.slot_size;
    status->first_frame_cap = g_auto_preroll_status.first_frame_cap;
    status->next_frame_no = g_auto_preroll_status.next_frame_no;
    status->start_frame_no = g_auto_preroll_status.start_frame_no;
    status->loop_flag = g_auto_preroll_status.loop_flag;
    status->preroll_data.wdma2_data = g_auto_preroll_status.preroll_data.wdma2_data;
    status->preroll_data.jpeg_auto_fill_data = g_auto_preroll_status.preroll_data.jpeg_auto_fill_data;

    #if 1//def DBG_MSG
    priv_dump_preroll_status();
    #endif
    
    return API_SUCC;
}

int8_t app_auto_preroll_reset_status(void)
{
    #ifdef DBG_MSG
    dbg_printf(DBG_LESS_INFO, "app_auto_preroll_reset_status\n");
    #endif
    
    memset((void *)g_preroll_config.jpeg_auto_fill_data, 0, JPEG_AUTOFILL_SIZE*sizeof(uint8_t));
    hx_CleanDCache_by_Addr((volatile void *)g_preroll_config.jpeg_auto_fill_data, JPEG_AUTOFILL_SIZE*sizeof(uint8_t));
    
    g_auto_preroll_status.total_slot = g_preroll_config.slot_num;
    g_auto_preroll_status.slot_size = g_preroll_config.slot_size;
    g_auto_preroll_status.first_frame_cap = 0;
    g_auto_preroll_status.next_frame_no = 0;
    g_auto_preroll_status.start_frame_no = 0;
    g_auto_preroll_status.loop_flag = 0;
    g_auto_preroll_status.preroll_data.wdma2_data = g_preroll_config.wdma2_data;
    g_auto_preroll_status.preroll_data.jpeg_auto_fill_data = g_preroll_config.jpeg_auto_fill_data;

	hx_CleanDCache_by_Addr((volatile void *)&g_auto_preroll_status, sizeof(app_auto_preroll_status_t));

	#ifdef DBG_MSG
    priv_dump_preroll_status();
    #endif
	
    return API_SUCC;
}

int8_t app_auto_preroll_retain_frame_updadte_status(void)
{
    #ifdef DBG_MSG
    dbg_printf(DBG_LESS_INFO, "app_auto_preroll_retain_frame_updadte_status\n");
    #endif
    
    hx_drv_xdma_get_WDMA2NextFrameIdx(&g_auto_preroll_status.next_frame_no);
    
    if(g_auto_preroll_status.next_frame_no == g_auto_preroll_status.start_frame_no)
    {
        g_auto_preroll_status.loop_flag = 1;
    }
    if(g_auto_preroll_status.loop_flag == 1)
    {
        g_auto_preroll_status.start_frame_no = g_auto_preroll_status.next_frame_no;
    }
    g_auto_preroll_status.first_frame_cap = 1;
	
    hx_CleanDCache_by_Addr((volatile void *)&g_auto_preroll_status, sizeof(app_auto_preroll_status_t));

	#ifdef DBG_MSG
    priv_dump_preroll_status();
    #endif
    
    return API_SUCC;
}


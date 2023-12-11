#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "WE2_debug.h"

#include "hx_drv_edm.h"
#include "hx_drv_pmu.h"
#include "sensor_dp_lib.h"

#include "powermode.h"

#include "app_api.h"
#include "app_pmu.h"
#include "app_datapath.h"
#include "app_cisdp_cfg.h"

typedef struct {
    uint32_t frame_width;    /**< image width, assigned by Himax driver */
    uint32_t frame_height;   /**< image height, assigned by Himax driver */
    uint32_t jpeg_address; /**< JPEG image address, assigned by Himax driver */
    uint32_t jpeg_size;    /**< JPEG image size, assigned by Himax driver */
    uint32_t raw_address;  /**< RAW image address, assigned by Himax driver */
    //uint32_t raw_size;     /**< RAW image size, assigned by Himax driver */
} priv_dp_frame_result_t;

static uint8_t 	g_xdma_abnormal = 0;
static uint8_t 	g_rs_abnormal = 0;
static uint8_t 	g_hog_abnormal = 0;
static uint8_t 	g_md_detect = 0;
static uint8_t 	g_cdm_fifoerror = 0;
static uint8_t 	g_wdt1_timeout = 0;
static uint8_t 	g_wdt2_timeout = 0;
static uint8_t 	g_wdt3_timeout = 0;
static uint8_t 	g_hxautoi2c_error = 0;
static int32_t 	g_inp1bitparer_abnormal = 0;
static uint32_t g_dp_event = 0;
static uint8_t 	g_frame_ready = 0;
static uint8_t 	g_rs_frame_ready = 0;
static uint8_t 	g_hog_frame_ready = 0;
static uint32_t g_cur_jpegenc_frame = 0;
static uint8_t 	g_time = 0;
static uint32_t g_first_frame_done = 0;

//static volatile SENSORDPLIB_STREAM_E g_stream_type = SENCTRL_STREAM_TYPE;
//static volatile SENSORDPLIB_SENSOR_E g_sensor_type = SENCTRL_SENSOR_TYPE;
//static volatile INP_SUBSAMPLE_E g_inp_subsample_type = DP_INP_SUBSAMPLE;

static volatile SENSORDPLIB_PATH_E g_datapath_case = SENSORDPLIB_PATH_NO;

static volatile app_dp_cfg_t g_app_dp_cur_cfg = {0};
static volatile app_dp_cap_t g_app_dp_cap_set = {0};
static volatile app_dp_jpg_enc_t g_app_dp_jpg_enc_set = {0};
static volatile app_dp_jpg_dec_t g_app_dp_jpg_dec_set = {0};

static void app_dp_check_over_frame_ready(priv_dp_frame_result_t *pframe_result)
{
    uint32_t de0_count, de1_count, de2_count;
    uint32_t convde_count;
    uint16_t af_framecnt;
    uint16_t be_framecnt;
    
    while(1)
    {
        if(g_inp1bitparer_abnormal == 1 || g_wdt1_timeout == 1 || g_wdt2_timeout == 1 || g_wdt3_timeout == 1
			|| g_cdm_fifoerror == 1 || g_xdma_abnormal == 1 || g_hxautoi2c_error == 1)
    	{
    		app_cisdp_sensor_stop();

    		g_frame_ready = 0;

    		dbg_printf(DBG_LESS_INFO, "Stop All Steam \n");
    	}
        
        if(g_frame_ready == 1)
        {
            if((g_datapath_case == SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG) ||
                (g_datapath_case == SENSORDPLIB_PATH_INP_HW5x5_JPEG) ||
                (g_datapath_case == SENSORDPLIB_PATH_INT1))
            {
                uint8_t frame_no = 0;
                uint8_t next_frame_no = 0;
                uint8_t buffer_no = 0;
                uint32_t jpeg_enc_addr = 0;
                uint32_t jpeg_enc_filesize = 0;
                uint32_t jpeg_enc_filesize_real = 0;
                
                hx_drv_xdma_get_WDMA2_bufferNo(&buffer_no);
                hx_drv_xdma_get_WDMA2NextFrameIdx(&next_frame_no);

                g_app_dp_cap_set.frame_no = next_frame_no;
                
                dbg_printf(DBG_LESS_INFO, "buffer_no=%u, next_frame_no=%u\n",buffer_no, next_frame_no);
                frame_no = (buffer_no + next_frame_no - 1) % buffer_no;
                
                hx_drv_jpeg_get_EncOutRealMEMSize(&jpeg_enc_filesize_real);
                dbg_printf(DBG_LESS_INFO, "jpeg_enc_filesize_real=0x%x\n", jpeg_enc_filesize_real);
                
                hx_drv_jpeg_get_FillFileSizeToMem(frame_no, (uint32_t)g_app_dp_cur_cfg.jpeg_auto_fill_data, &jpeg_enc_filesize);
                hx_drv_jpeg_get_MemAddrByFrameNo(frame_no, (uint32_t)g_app_dp_cur_cfg.wdma2, &jpeg_enc_addr);

                if(jpeg_enc_filesize_real != jpeg_enc_filesize)
                {
                    dbg_printf(DBG_LESS_INFO, "jpeg_enc_filesize_real(0x%08X) != jpeg_enc_filesize(0x%08X)\n", jpeg_enc_filesize_real, jpeg_enc_filesize);

                    hx_drv_jpeg_set_FillFileSizeToMem(next_frame_no, (uint32_t)g_app_dp_cur_cfg.jpeg_auto_fill_data);
					SCB_CleanDCache_by_Addr((volatile void *)g_app_dp_cur_cfg.jpeg_auto_fill_data, 100/*JPEG_AUTOFILL_SIZE*/*sizeof(uint8_t));
					
                    //uint32_t *pjpeg_enc_filesize = 0;
                    //pjpeg_enc_filesize = (uint32_t *)(g_app_dp_cur_cfg.jpeg_auto_fill_data + (4 * frame_no));
                    //*pjpeg_enc_filesize = jpeg_enc_filesize_real;

                    //change value
                    jpeg_enc_filesize = jpeg_enc_filesize_real;
                }
                
                dbg_printf(DBG_LESS_INFO, "frame_no=%u, jpeg_size=0x%x,addr=0x%x\n",frame_no,jpeg_enc_filesize,jpeg_enc_addr);

                pframe_result->jpeg_address = jpeg_enc_addr;
                pframe_result->jpeg_size = jpeg_enc_filesize;
                pframe_result->raw_address = (uint32_t)g_app_dp_cur_cfg.wdma3;
                if(g_datapath_case == SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG || (g_datapath_case == SENSORDPLIB_PATH_INT1))
                {
                    pframe_result->frame_width = g_app_dp_cap_set.hw55_in_width;
                    pframe_result->frame_height = g_app_dp_cap_set.hw55_in_height;
                }
                else if(g_datapath_case == SENSORDPLIB_PATH_INP_HW5x5_JPEG)
                {
                    pframe_result->frame_width = 0;
                    pframe_result->frame_height = 0;
                }
            }
            else if (g_datapath_case == SENSORDPLIB_PATH_JPEGDEC)
            {
                pframe_result->jpeg_address = 0;
                pframe_result->jpeg_size = 0;
                pframe_result->raw_address = (uint32_t)g_app_dp_cur_cfg.wdma3;
                pframe_result->frame_width = g_app_dp_jpg_dec_set.dec_width;
                pframe_result->frame_height = g_app_dp_jpg_dec_set.dec_height;
            }
            else if (g_datapath_case == SENSORDPLIB_PATH_TPG_JPEGENC)
            {
                uint8_t frame_no = 0;
                uint8_t next_frame_no = 0;
                uint8_t buffer_no = 0;
                uint32_t jpeg_enc_addr = 0;
                uint32_t jpeg_enc_filesize = 0;
                uint32_t jpeg_enc_filesize_real = 0;
                
                hx_drv_xdma_get_WDMA2_bufferNo(&buffer_no);
                hx_drv_xdma_get_WDMA2NextFrameIdx(&next_frame_no);

                g_app_dp_jpg_enc_set.frame_no = next_frame_no;
                
                dbg_printf(DBG_LESS_INFO, "buffer_no=%u, next_frame_no=%u\n",buffer_no, next_frame_no);
                frame_no = (buffer_no + next_frame_no - 1) % buffer_no;
                
                hx_drv_jpeg_get_EncOutRealMEMSize(&jpeg_enc_filesize_real);
                dbg_printf(DBG_LESS_INFO, "jpeg_enc_filesize_real=0x%x\n", jpeg_enc_filesize_real);
                
                hx_drv_jpeg_get_FillFileSizeToMem(frame_no, (uint32_t)g_app_dp_cur_cfg.jpeg_auto_fill_data, &jpeg_enc_filesize);
                hx_drv_jpeg_get_MemAddrByFrameNo(frame_no, (uint32_t)g_app_dp_cur_cfg.wdma2, &jpeg_enc_addr);

                if( jpeg_enc_filesize_real != jpeg_enc_filesize)
                {
                    dbg_printf(DBG_LESS_INFO, "jpeg_enc_filesize_real(0x%08X) != jpeg_enc_filesize(0x%08X)\n", jpeg_enc_filesize_real, jpeg_enc_filesize);

                    hx_drv_jpeg_set_FillFileSizeToMem(next_frame_no, (uint32_t)g_app_dp_cur_cfg.jpeg_auto_fill_data);
					SCB_CleanDCache_by_Addr((volatile void *)g_app_dp_cur_cfg.jpeg_auto_fill_data, 100/*JPEG_AUTOFILL_SIZE*/*sizeof(uint8_t));
					
                    //uint32_t *pjpeg_enc_filesize = 0;
                    //pjpeg_enc_filesize = (uint32_t *)(g_app_dp_cur_cfg.jpeg_auto_fill_data + (4 * frame_no));
                    //*pjpeg_enc_filesize = jpeg_enc_filesize_real;

                    //change value
                    jpeg_enc_filesize = jpeg_enc_filesize_real;
                }
                
                dbg_printf(DBG_LESS_INFO, "frame_no=%u, jpeg_size=0x%x,addr=0x%x\n",frame_no,jpeg_enc_filesize,jpeg_enc_addr);
                
                pframe_result->jpeg_address = jpeg_enc_addr;
                pframe_result->jpeg_size = jpeg_enc_filesize;
                pframe_result->raw_address = 0;
                pframe_result->frame_width = 0;
                pframe_result->frame_height = 0;
            }
            else if (g_datapath_case == SENSORDPLIB_PATH_INP_WDMA2)
            {
                uint8_t next_frame_no = 0;
                
                hx_drv_xdma_get_WDMA2NextFrameIdx(&next_frame_no);
                g_app_dp_cap_set.frame_no = next_frame_no;
                
                #if defined(HM0435) /*yuv422*/
                app_yuv_planar_t yuv_planar;
                uint32_t yuv_img_sz = ((g_app_dp_cur_cfg.inp_out_width * g_app_dp_cur_cfg.inp_out_height) << 1);
                
                uint32_t yuv_planar_y_addr = (uint32_t)(g_app_dp_cur_cfg.wdma3);
                uint32_t yuv_planar_u_addr = (uint32_t)(yuv_planar_y_addr + (yuv_img_sz >> 1));
                uint32_t yuv_planar_v_addr = (uint32_t)(yuv_planar_u_addr + (yuv_img_sz >> 2));
                
                yuv_planar.pY = (uint8_t *)yuv_planar_y_addr;
                yuv_planar.pU = (uint8_t *)yuv_planar_u_addr;
                yuv_planar.pV = (uint8_t *)yuv_planar_v_addr;

                app_yuv422packed2planar2((uint8_t*)g_app_dp_cur_cfg.wdma2, &yuv_planar, yuv_img_sz);
                
                pframe_result->jpeg_address = 0;
                pframe_result->jpeg_size = 0;
                pframe_result->raw_address = g_app_dp_cur_cfg.wdma3;
                pframe_result->frame_width = g_app_dp_cur_cfg.inp_out_width;
                pframe_result->frame_height = g_app_dp_cur_cfg.inp_out_height;
                #else /*RGB*/
                pframe_result->jpeg_address = 0;
                pframe_result->jpeg_size = 0;
                pframe_result->raw_address = g_app_dp_cur_cfg.wdma2;
                pframe_result->frame_width = g_app_dp_cur_cfg.inp_out_width;
                pframe_result->frame_height = g_app_dp_cur_cfg.inp_out_height;
                #endif
                
            }

            //hx_drv_edm_get_frame_count(&af_framecnt, &be_framecnt);
            //dbg_printf(DBG_LESS_INFO, "af_framecnt=%d,be_framecnt=%d\n",af_framecnt,be_framecnt);
            
            g_frame_ready = 0;
            g_first_frame_done = 1;
            break;
            /////////End//////////////////////
        }//g_frame_ready == 1
    }
}
static void app_dp_cb(SENSORDPLIB_STATUS_E event)
{
    //dbg_printf(DBG_LESS_INFO, "event = %d\n", event);

    switch(event)
    {
    case SENSORDPLIB_STATUS_EDM_WDT1_TIMEOUT:
        dbg_printf(DBG_LESS_INFO, "CB WDT1 Timeout\n");
        g_wdt1_timeout = 1;
        break;
    case SENSORDPLIB_STATUS_EDM_WDT2_TIMEOUT:
        dbg_printf(DBG_LESS_INFO, "CB WDT2 Timeout\n");
        g_wdt2_timeout = 1;
        break;
    case SENSORDPLIB_STATUS_EDM_WDT3_TIMEOUT:
        dbg_printf(DBG_LESS_INFO, "CB WDT3 Timeout\n");
        g_wdt3_timeout = 1;
        break;
    case SENSORDPLIB_STATUS_SENSORCTRL_WDT_OUT:
        /*
         * TODO error happen need check sensor
         * 1. SWRESET Sensor Control & DP
         * 2. restart streaming flow
         */
        dbg_printf(DBG_LESS_INFO, "WDT OUT %d\n", event);
        break;

    case SENSORDPLIB_STATUS_CDM_FIFO_OVERFLOW:
    case SENSORDPLIB_STATUS_CDM_FIFO_UNDERFLOW:
        dbg_printf(DBG_LESS_INFO, "CDM_FIFO_ERROR %d\n", event);

        g_xdma_abnormal = 1;
        break;

    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL1:
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL2:
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL3:
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL4:
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL5:
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL6:
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL7:
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL8:
    case SENSORDPLIB_STATUS_XDMA_WDMA1_ABNORMAL9:

    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL1:
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL2:
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL3:
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL4:
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL5:
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL6:
    case SENSORDPLIB_STATUS_XDMA_WDMA2_ABNORMAL7:

    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL1:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL2:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL3:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL4:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL5:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL6:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL7:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL8:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_ABNORMAL9:

        dbg_printf(DBG_LESS_INFO, "WDMA123 abnormal %d\n", event);
        g_xdma_abnormal = 1;
        break;

    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL1:
    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL2:
    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL3:
    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL4:
    case SENSORDPLIB_STATUS_XDMA_RDMA_ABNORMAL5:
        dbg_printf(DBG_LESS_INFO, "RDMA abnormal %d\n", event);
        g_xdma_abnormal = 1;
        break;

    case SENSORDPLIB_STATUS_RSDMA_ABNORMAL1:
    case SENSORDPLIB_STATUS_RSDMA_ABNORMAL2:
    case SENSORDPLIB_STATUS_RSDMA_ABNORMAL3:
    case SENSORDPLIB_STATUS_RSDMA_ABNORMAL4:
    case SENSORDPLIB_STATUS_RSDMA_ABNORMAL5:
    case SENSORDPLIB_STATUS_RSDMA_ABNORMAL6:
    case SENSORDPLIB_STATUS_RSDMA_ABNORMAL7:
        /*
         *  error happen need
         * 1. SWRESET RS & RS DMA
         * 2. Re-run flow again
         */
        dbg_printf(DBG_LESS_INFO, "RSDMA abnormal %d\n", event);
        g_rs_abnormal = 1;
        break;

    case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL1:
    case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL2:
    case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL3:
    case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL4:
    case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL5:
    case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL6:
    case SENSORDPLIB_STATUS_HOGDMA_ABNORMAL7:
        /*
         *  error happen need
         * 1. SWRESET HOG & HOG DMA
         * 2. Re-run flow again
         */
        dbg_printf(DBG_LESS_INFO, "HOGDMA abnormal %d\n", event);
        g_hog_abnormal = 1;
        break;

    case SENSORDPLIB_STATUS_CDM_MOTION_DETECT:
        /*
         * app anything want to do
         * */
        g_md_detect = 1;
        dbg_printf(DBG_LESS_INFO, "Motion Detect\n");
        break;
    case SENSORDPLIB_STATUS_XDMA_FRAME_READY:
        g_frame_ready = 1;
        break;
    case SENSORDPLIB_STATUS_XDMA_WDMA1_FINISH:
    case SENSORDPLIB_STATUS_XDMA_WDMA2_FINISH:
    case SENSORDPLIB_STATUS_XDMA_WDMA3_FINISH:
    case SENSORDPLIB_STATUS_XDMA_RDMA_FINISH:
        break;

    case SENSORDPLIB_STATUS_RSDMA_FINISH:
        /*
         * RS Frame result ready
         */
        g_rs_frame_ready = 1;
        break;
    case SENSORDPLIB_STATUS_HOGDMA_FINISH:
        /*
         * HOG Frame result ready
         */
        g_hog_frame_ready = 1;
        break;
    case SENSORDPLIB_STATUS_TIMER_FIRE_APP_NOTREADY:
        g_time++;
        break;
    case SENSORDPLIB_STATUS_TIMER_FIRE_APP_READY:
        g_time++;
        break;
    default:
        dbg_printf(DBG_LESS_INFO, "event = %d\n", event);
        break;
    }
}


static void app_dp_init_flag()
{
    g_xdma_abnormal = 0;
    g_md_detect = 0;
    g_cdm_fifoerror = 0;
    g_wdt1_timeout = 0;
    g_wdt2_timeout = 0;
    g_wdt3_timeout = 0;
    g_inp1bitparer_abnormal = 0;
    //g_dp_event = 0;
    g_frame_ready = 0;
    g_time = 0;
    g_cur_jpegenc_frame = 0;
	g_hxautoi2c_error = 0;
    
    g_first_frame_done = 0;
}

int8_t app_dp_init(const app_dp_cfg_t* dp_init_cfg)
{
    uint32_t wakeup_event = 0;
    uint32_t wakeup_event1 = 0;
    
    if(dp_init_cfg->wdma1 == 0 && dp_init_cfg->wdma2 == 0 && dp_init_cfg->wdma3 == 0)
    {
        dbg_printf(DBG_LESS_INFO, "app_dp_init: error! wdma need to be setup!\n");
    }
    else if(dp_init_cfg->wdma2 != 0 && (dp_init_cfg->jpeg_auto_fill_data == 0 || dp_init_cfg->wdma2_cyclic_buffer_cnt == 0))
    {
        dbg_printf(DBG_LESS_INFO, "app_dp_init: error! jpeg_auto_fill_data & wdma2_cyclic_buffer_cnt need to be setup!\n");
    }

    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();

    sensordplib_set_xDMA_baseaddrbyapp((uint32_t)dp_init_cfg->wdma1, (uint32_t)dp_init_cfg->wdma2, (uint32_t)dp_init_cfg->wdma3);
    sensordplib_set_jpegfilesize_addrbyapp((uint32_t)dp_init_cfg->jpeg_auto_fill_data);
    
    g_app_dp_cur_cfg.wdma1 = dp_init_cfg->wdma1;
    g_app_dp_cur_cfg.wdma2 = dp_init_cfg->wdma2;
    g_app_dp_cur_cfg.wdma3 = dp_init_cfg->wdma3;
    g_app_dp_cur_cfg.jpeg_auto_fill_data = dp_init_cfg->jpeg_auto_fill_data;
    g_app_dp_cur_cfg.wdma2_cyclic_buffer_cnt = dp_init_cfg->wdma2_cyclic_buffer_cnt;
    
    #ifdef IC_PACKAGE_WLCSP65
    hx_drv_scu_set_SEN_INT_pinmux(SCU_SEN_INT_PINMUX_FVALID);
    //hx_drv_scu_set_SEN_GPIO_pinmux(SCU_SEN_GPIO_PINMUX_LVALID);
    hx_drv_scu_set_SEN_XSLEEP_pinmux(SCU_SEN_XSLEEP_PINMUX_SEN_XSLEEP_0);
    #endif

    app_dp_init_flag();
    #ifdef HM2170_BAYER
    hx_drv_cis_set_slaveID(0x25);
    #endif

    hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
    dbg_printf(DBG_LESS_INFO, "WakeupEvt=0x%x\n", wakeup_event);
    dbg_printf(DBG_LESS_INFO, "WakeupEvt1=0x%x\n", wakeup_event1);

    if((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE))
    {
        app_cisdp_sensor_init(dp_init_cfg);
        app_cisdp_sensor_start();
    }
    
    app_cisdp_datapath_init(dp_init_cfg);
    
    g_app_dp_cur_cfg.init_type = dp_init_cfg->init_type;
    g_app_dp_cur_cfg.sensor_type = dp_init_cfg->sensor_type;
    g_app_dp_cur_cfg.stream_type = dp_init_cfg->stream_type;
    g_app_dp_cur_cfg.inp_subsample_type = dp_init_cfg->inp_subsample_type;
    g_app_dp_cur_cfg.crop.start_x = dp_init_cfg->crop.start_x;
    g_app_dp_cur_cfg.crop.start_y = dp_init_cfg->crop.start_y;
    g_app_dp_cur_cfg.crop.last_x = dp_init_cfg->crop.last_x;
    g_app_dp_cur_cfg.crop.last_y = dp_init_cfg->crop.last_y;
    #if (IC_VERSION >= 30)
    g_app_dp_cur_cfg.inp_bin = dp_init_cfg->inp_bin;
    #endif
    g_app_dp_cur_cfg.xshutdown_pin = dp_init_cfg->xshutdown_pin;
    g_app_dp_cur_cfg.mclk_div = dp_init_cfg->mclk_div;
    g_app_dp_cur_cfg.sclk_utg = dp_init_cfg->sclk_utg;
    g_app_dp_cur_cfg.h_fporch= dp_init_cfg->h_fporch;
    g_app_dp_cur_cfg.sensor_width = dp_init_cfg->sensor_width;
    g_app_dp_cur_cfg.sensor_height = dp_init_cfg->sensor_height;
    g_app_dp_cur_cfg.inp_out_width = dp_init_cfg->inp_out_width;
    g_app_dp_cur_cfg.inp_out_height = dp_init_cfg->inp_out_height;
    
    return API_SUCC;
}

int8_t app_dp_get_frame(uint32_t *praw_addr, uint32_t *pframe_width, uint32_t *pframe_height, uint32_t *pjpg_addr, uint32_t *pjpg_size)
{
    priv_dp_frame_result_t frame_result = {0};
    
    if(praw_addr && pframe_width && pframe_height)
    {
        *praw_addr = 0;
        *pframe_width = 0;
        *pframe_height = 0;
    }
    
    if(pjpg_addr && pjpg_size)
    {
        *pjpg_addr = 0;
        *pjpg_size = 0;
    }
    
    app_dp_check_over_frame_ready(&frame_result);
    
    if(praw_addr && pframe_width && pframe_height)
    {
        *praw_addr = frame_result.raw_address;
        *pframe_width = frame_result.frame_width;
        *pframe_height = frame_result.frame_height;
    }
    
    if(pjpg_addr && pjpg_size)
    {
        *pjpg_addr = frame_result.jpeg_address;
        *pjpg_size = frame_result.jpeg_size;
    }
    
    return API_SUCC;
}

int8_t app_dp_decode_jpeg(const app_dp_jpg_dec_t* dp_jpg_dec_settings)
{
    JPEG_CFG_T jpegdec_cfg;

    if((dp_jpg_dec_settings->dec_roi_stx%16 != 0) || (dp_jpg_dec_settings->dec_roi_sty%16 != 0))
    {
        dbg_printf(DBG_LESS_INFO, "dec_roi_stx or dec_roi_sty invalid\n");
        return API_ERROR;
    }
    
    if((dp_jpg_dec_settings->dec_width%16 != 0) || (dp_jpg_dec_settings->dec_height%16 != 0))
    {
        dbg_printf(DBG_LESS_INFO, "enc_width or enc_height invalid\n");
        return API_ERROR;
    }

    if(dp_jpg_dec_settings->frame_no >= g_app_dp_cur_cfg.wdma2_cyclic_buffer_cnt)
    {
        dbg_printf(DBG_LESS_INFO, "frame_no invalid\n");
        return API_ERROR;
    }

    //setup JPEG Dec block
    if(hx_drv_jpeg_get_defCfg(&jpegdec_cfg) != JPEG_NO_ERROR)
    {
       dbg_printf(DBG_LESS_INFO, "hx_drv_jpeg_get_defCfg fail\n");
    }
    
    jpegdec_cfg.jpeg_path = JPEG_PATH_DECODER_EN;
    jpegdec_cfg.dec_roi_stx = dp_jpg_dec_settings->dec_roi_stx;
    jpegdec_cfg.dec_roi_sty = dp_jpg_dec_settings->dec_roi_sty;
    jpegdec_cfg.dec_width = dp_jpg_dec_settings->dec_width;
    jpegdec_cfg.dec_height = dp_jpg_dec_settings->dec_height;
    jpegdec_cfg.jpeg_enctype = (JPEG_ENC_TYPE_T)dp_jpg_dec_settings->jpeg_enctype;
    jpegdec_cfg.jpeg_encqtable = (JPEG_ENC_QTABLE_T)dp_jpg_dec_settings->jpeg_encqtable;

    g_app_dp_jpg_dec_set.frame_no = dp_jpg_dec_settings->frame_no;
    g_app_dp_jpg_dec_set.dec_roi_stx = dp_jpg_dec_settings->dec_roi_stx;
    g_app_dp_jpg_dec_set.dec_roi_sty = dp_jpg_dec_settings->dec_roi_sty;
    g_app_dp_jpg_dec_set.dec_width = dp_jpg_dec_settings->dec_width;
    g_app_dp_jpg_dec_set.dec_height = dp_jpg_dec_settings->dec_height;
    g_app_dp_jpg_dec_set.jpeg_enctype = dp_jpg_dec_settings->jpeg_enctype;
    g_app_dp_jpg_dec_set.jpeg_encqtable = dp_jpg_dec_settings->jpeg_encqtable;

    //hx_dplib_evthandler_register_cb(app_dp_cb, SENSORDPLIB_CB_FUNTYPE_DP);
    sensordplib_set_jpegdec(jpegdec_cfg, dp_jpg_dec_settings->dec_width, dp_jpg_dec_settings->dec_height, dp_jpg_dec_settings->frame_no, app_dp_cb);

    g_datapath_case = SENSORDPLIB_PATH_JPEGDEC;
    
    return API_SUCC;
}

int8_t app_dp_encode_jpeg(const app_dp_jpg_enc_t* dp_jpg_enc_settings)
{
    JPEG_CFG_T jpegenc_cfg;
    uint32_t jpegenc_addr = 0;
    SENSORDPLIB_RDMA_CFG_T dplib_rdma_cfg = {0, 0, 0};

    if((dp_jpg_enc_settings->enc_width%16 != 0) || (dp_jpg_enc_settings->enc_height%16 != 0))
    {
        dbg_printf(DBG_LESS_INFO, "enc_width or enc_height invalid\n");
        return API_ERROR;
    }

    if(dp_jpg_enc_settings->frame_no >= g_app_dp_cur_cfg.wdma2_cyclic_buffer_cnt)
    {
        dbg_printf(DBG_LESS_INFO, "frame_no invalid\n");
        return API_ERROR;
    }
    
    hx_drv_jpeg_get_defCfg(&jpegenc_cfg);
    hx_drv_jpeg_get_MemAddrByFrameNo(dp_jpg_enc_settings->frame_no, (uint32_t)g_app_dp_cur_cfg.wdma2, &jpegenc_addr);
    
    jpegenc_cfg.jpeg_path = JPEG_PATH_ENCODER_EN;
    jpegenc_cfg.enc_width = dp_jpg_enc_settings->enc_width;
    jpegenc_cfg.enc_height = dp_jpg_enc_settings->enc_height;
    jpegenc_cfg.jpeg_enctype = (JPEG_ENC_TYPE_T)dp_jpg_enc_settings->jpeg_enctype;
    jpegenc_cfg.jpeg_encqtable = (JPEG_ENC_QTABLE_T)dp_jpg_enc_settings->jpeg_encqtable;
    
    if(jpegenc_cfg.jpeg_enctype == JPEG_ENC_TYPE_YUV400) // YUV400
    {
        dplib_rdma_cfg.rdma_ch1_startaddr = (uint32_t)g_app_dp_cur_cfg.wdma3;
    }else if(jpegenc_cfg.jpeg_enctype == JPEG_ENC_TYPE_YUV420) // YUV420
    {
        dplib_rdma_cfg.rdma_ch1_startaddr = (uint32_t)g_app_dp_cur_cfg.wdma3;
        dplib_rdma_cfg.rdma_ch2_startaddr = dplib_rdma_cfg.rdma_ch1_startaddr+(jpegenc_cfg.enc_width*jpegenc_cfg.enc_height);
        dplib_rdma_cfg.rdma_ch3_startaddr = dplib_rdma_cfg.rdma_ch2_startaddr+((jpegenc_cfg.enc_width*jpegenc_cfg.enc_height)>>2);
    }else if(jpegenc_cfg.jpeg_enctype == JPEG_ENC_TYPE_YUV422) // YUV422
    {
        dplib_rdma_cfg.rdma_ch1_startaddr = (uint32_t)g_app_dp_cur_cfg.wdma3;
        dplib_rdma_cfg.rdma_ch2_startaddr = dplib_rdma_cfg.rdma_ch1_startaddr+(jpegenc_cfg.enc_width*jpegenc_cfg.enc_height);
        dplib_rdma_cfg.rdma_ch3_startaddr = dplib_rdma_cfg.rdma_ch2_startaddr+((jpegenc_cfg.enc_width*jpegenc_cfg.enc_height)>>1);
    }
    
    g_app_dp_jpg_enc_set.frame_no = dp_jpg_enc_settings->frame_no;
    g_app_dp_jpg_enc_set.enc_width = dp_jpg_enc_settings->enc_width;
    g_app_dp_jpg_enc_set.enc_height = dp_jpg_enc_settings->enc_height;
    g_app_dp_jpg_enc_set.jpeg_enctype = dp_jpg_enc_settings->jpeg_enctype;
    g_app_dp_jpg_enc_set.jpeg_encqtable = dp_jpg_enc_settings->jpeg_encqtable;
    
    //hx_dplib_evthandler_register_cb(app_dp_cb, SENSORDPLIB_CB_FUNTYPE_DP);
    sensordplib_tpg_jpegenc_wdma2(jpegenc_cfg, jpegenc_addr, 1, (uint32_t)(g_app_dp_cur_cfg.jpeg_auto_fill_data+dp_jpg_enc_settings->frame_no*4), dplib_rdma_cfg, app_dp_cb);
    
    #if (CIS_ENABLE_HX_AUTOI2C != 0x00)
    //init hxauotoi2c
	if(g_app_dp_cur_cfg.init_type != APP_INIT_TYPE_VIDEO_STREAM)
		app_cisdp_set_hxautoi2c(g_app_dp_cur_cfg.init_type);
    #endif

    g_datapath_case = SENSORDPLIB_PATH_TPG_JPEGENC;
    
    return API_SUCC;
}

int8_t app_dp_capture_frame(const app_dp_cap_t* dp_cap_settings)
{
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T jpeg_cfg;

    if(g_first_frame_done == 0 ||
        g_datapath_case != SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG ||
        g_app_dp_cap_set.frame_no != dp_cap_settings->frame_no ||
        g_app_dp_cap_set.hw55_crop_stx != dp_cap_settings->hw55_crop_stx ||
        g_app_dp_cap_set.hw55_crop_sty != dp_cap_settings->hw55_crop_sty ||
        g_app_dp_cap_set.hw55_in_width != dp_cap_settings->enc_width ||
        g_app_dp_cap_set.hw55_in_height != dp_cap_settings->enc_height ||
        g_app_dp_cap_set.hw55_path != dp_cap_settings->hw55_path ||
        g_app_dp_cap_set.hw55_y_only != dp_cap_settings->hw55_y_only ||
        g_app_dp_cap_set.enc_width != dp_cap_settings->enc_width ||
        g_app_dp_cap_set.enc_height != dp_cap_settings->enc_height ||
        g_app_dp_cap_set.jpeg_enctype != dp_cap_settings->jpeg_enctype ||
        g_app_dp_cap_set.jpeg_encqtable != dp_cap_settings->jpeg_encqtable)
    {
        hx_drv_hw5x5_get_defCfg(&hw5x5_cfg);
        /* HW5x5 Path */
        hw5x5_cfg.hw5x5_path = DP_HW5X5_PATH;
        /* HW5x5 Color Mode */
        if(dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV420) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV420;
        } else if (dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV400) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV400;
        } else if (dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV422) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV422;
        } else {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV400;
        }
        
        hw5x5_cfg.demos_bndmode = DP_HW5X5_DEMOS_BNDMODE;
        hw5x5_cfg.hw55_crop_stx = dp_cap_settings->hw55_crop_stx;
        hw5x5_cfg.hw55_crop_sty = dp_cap_settings->hw55_crop_sty;
        hw5x5_cfg.hw55_in_width = dp_cap_settings->enc_width;
        hw5x5_cfg.hw55_in_height = dp_cap_settings->enc_height;
#if 0
        hw5x5_cfg.wdma3_ch1_only = (1 == dp_cap_settings->hw55_y_only)?true:false;
#else
    #warning "TEMPORARY COMMENT OUT PMU RELATED CALLS"
#endif
        
        g_app_dp_cap_set.hw55_path = dp_cap_settings->hw55_path;
        g_app_dp_cap_set.hw55_crop_stx = dp_cap_settings->hw55_crop_stx;
        g_app_dp_cap_set.hw55_crop_sty = dp_cap_settings->hw55_crop_sty;
        g_app_dp_cap_set.hw55_in_width = dp_cap_settings->enc_width;
        g_app_dp_cap_set.hw55_in_height = dp_cap_settings->enc_height;
        g_app_dp_cap_set.hw55_y_only = dp_cap_settings->hw55_y_only;

        hx_drv_jpeg_get_defCfg(&jpeg_cfg);

        jpeg_cfg.jpeg_path = JPEG_PATH_ENCODER_EN;
        jpeg_cfg.enc_width = dp_cap_settings->enc_width;
        jpeg_cfg.enc_height = dp_cap_settings->enc_height;
        jpeg_cfg.jpeg_enctype = (JPEG_ENC_TYPE_T)dp_cap_settings->jpeg_enctype;
        jpeg_cfg.jpeg_encqtable = (JPEG_ENC_QTABLE_T)dp_cap_settings->jpeg_encqtable;
        
        g_app_dp_cap_set.enc_width = dp_cap_settings->enc_width;
        g_app_dp_cap_set.enc_height = dp_cap_settings->enc_height;
        g_app_dp_cap_set.jpeg_enctype = dp_cap_settings->jpeg_enctype;
        g_app_dp_cap_set.jpeg_encqtable = dp_cap_settings->jpeg_encqtable;
        
        g_app_dp_cap_set.frame_no = dp_cap_settings->frame_no;

        //hx_dplib_evthandler_register_cb(app_dp_cb, SENSORDPLIB_CB_FUNTYPE_DP);
        sensordplib_set_int_hw5x5_jpeg_wdma23_start_no(hw5x5_cfg, jpeg_cfg, g_app_dp_cur_cfg.wdma2_cyclic_buffer_cnt, dp_cap_settings->frame_no, app_dp_cb);
        
        #if (CIS_ENABLE_HX_AUTOI2C != 0x00)
        //init hxauotoi2c
    	if(g_app_dp_cur_cfg.init_type != APP_INIT_TYPE_VIDEO_STREAM)
    		app_cisdp_set_hxautoi2c(g_app_dp_cur_cfg.init_type);
        #endif
        
        g_datapath_case = SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG;
    }

    dbg_printf(DBG_LESS_INFO, "g_first_frame_done=%u\n", g_first_frame_done);

    if(g_first_frame_done == 0)
    {
        sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
        sensordplib_set_sensorctrl_start();
    }
    else
    {
        sensordplib_retrigger_capture();
    }

    return API_SUCC;
}

int8_t app_dp_capture_jpeg_frame(const app_dp_cap_t* dp_cap_settings)
{
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T jpeg_cfg;

    if(g_first_frame_done == 0 ||
        g_datapath_case != SENSORDPLIB_PATH_INP_HW5x5_JPEG ||
        g_app_dp_cap_set.frame_no != dp_cap_settings->frame_no ||
        g_app_dp_cap_set.hw55_crop_stx != dp_cap_settings->hw55_crop_stx ||
        g_app_dp_cap_set.hw55_crop_sty != dp_cap_settings->hw55_crop_sty ||
        g_app_dp_cap_set.hw55_in_width != dp_cap_settings->enc_width ||
        g_app_dp_cap_set.hw55_in_height != dp_cap_settings->enc_height ||
        g_app_dp_cap_set.hw55_path != dp_cap_settings->hw55_path ||
        g_app_dp_cap_set.enc_width != dp_cap_settings->enc_width ||
        g_app_dp_cap_set.enc_height != dp_cap_settings->enc_height ||
        g_app_dp_cap_set.jpeg_enctype != dp_cap_settings->jpeg_enctype ||
        g_app_dp_cap_set.jpeg_encqtable != dp_cap_settings->jpeg_encqtable)
    {
        hx_drv_hw5x5_get_defCfg(&hw5x5_cfg);
        /* HW5x5 Path */
        hw5x5_cfg.hw5x5_path = DP_HW5X5_PATH;
        /* HW5x5 Color Mode */
        if(dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV420) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV420;
        } else if (dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV400) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV400;
        } else if (dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV422) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV422;
        } else {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV400;
        }
        
        hw5x5_cfg.demos_bndmode = DP_HW5X5_DEMOS_BNDMODE;
        hw5x5_cfg.hw55_crop_stx = dp_cap_settings->hw55_crop_stx;
        hw5x5_cfg.hw55_crop_sty = dp_cap_settings->hw55_crop_sty;
        hw5x5_cfg.hw55_in_width = dp_cap_settings->enc_width;
        hw5x5_cfg.hw55_in_height = dp_cap_settings->enc_height;

        g_app_dp_cap_set.hw55_path = dp_cap_settings->hw55_path;
        g_app_dp_cap_set.hw55_crop_stx = dp_cap_settings->hw55_crop_stx;
        g_app_dp_cap_set.hw55_crop_sty = dp_cap_settings->hw55_crop_sty;
        g_app_dp_cap_set.hw55_in_width = dp_cap_settings->enc_width;
        g_app_dp_cap_set.hw55_in_height = dp_cap_settings->enc_height;
        g_app_dp_cap_set.hw55_y_only = dp_cap_settings->hw55_y_only;
        
        hx_drv_jpeg_get_defCfg(&jpeg_cfg);

        jpeg_cfg.jpeg_path = JPEG_PATH_ENCODER_EN;
        jpeg_cfg.enc_width = dp_cap_settings->enc_width;
        jpeg_cfg.enc_height = dp_cap_settings->enc_height;
        jpeg_cfg.jpeg_enctype = (JPEG_ENC_TYPE_T)dp_cap_settings->jpeg_enctype;
        jpeg_cfg.jpeg_encqtable = (JPEG_ENC_QTABLE_T)dp_cap_settings->jpeg_encqtable;
        
        g_app_dp_cap_set.enc_width = dp_cap_settings->enc_width;
        g_app_dp_cap_set.enc_height = dp_cap_settings->enc_height;
        g_app_dp_cap_set.jpeg_enctype = dp_cap_settings->jpeg_enctype;
        g_app_dp_cap_set.jpeg_encqtable = dp_cap_settings->jpeg_encqtable;

        g_app_dp_cap_set.frame_no = dp_cap_settings->frame_no;
        
        //hx_dplib_evthandler_register_cb(app_dp_cb, SENSORDPLIB_CB_FUNTYPE_DP);
        sensordplib_set_hw5x5_jpeg_wdma2_start_no(hw5x5_cfg, jpeg_cfg, g_app_dp_cur_cfg.wdma2_cyclic_buffer_cnt, dp_cap_settings->frame_no, app_dp_cb);
        
        #if (CIS_ENABLE_HX_AUTOI2C != 0x00)
        //init hxauotoi2c
    	if(g_app_dp_cur_cfg.init_type != APP_INIT_TYPE_VIDEO_STREAM)
    		app_cisdp_set_hxautoi2c(g_app_dp_cur_cfg.init_type);
        #endif
        
        g_datapath_case = SENSORDPLIB_PATH_INP_HW5x5_JPEG;
    }

    //dbg_printf(DBG_LESS_INFO, "g_first_frame_done=%u\n", g_first_frame_done);

    if(g_first_frame_done == 0)
    {
        sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
        sensordplib_set_sensorctrl_start();
    }
    else
    {
        sensordplib_retrigger_capture();
    }

    return API_SUCC;
}

int8_t app_dp_capture_raw_frame(const app_dp_cap_t* dp_cap_settings)
{
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T jpeg_cfg;

    if(g_first_frame_done == 0 || g_datapath_case != SENSORDPLIB_PATH_INP_WDMA2 || g_app_dp_cap_set.frame_no != dp_cap_settings->frame_no|| g_app_dp_cap_set.frame_no != dp_cap_settings->frame_no)
    {
        //hx_dplib_evthandler_register_cb(app_dp_cb, SENSORDPLIB_CB_FUNTYPE_DP);
        
        #if defined(HM0435) /*yuv422*/
        sensordplib_set_raw_wdma2_start_no(g_app_dp_cur_cfg.inp_out_width+g_app_dp_cur_cfg.inp_out_width, g_app_dp_cur_cfg.inp_out_height, g_app_dp_cur_cfg.wdma2_cyclic_buffer_cnt ,dp_cap_settings->frame_no, app_dp_cb);
        #else /*RGB*/
        sensordplib_set_raw_wdma2_start_no(g_app_dp_cur_cfg.inp_out_width, g_app_dp_cur_cfg.inp_out_height, g_app_dp_cur_cfg.wdma2_cyclic_buffer_cnt ,dp_cap_settings->frame_no, app_dp_cb);
        #endif

        g_app_dp_cap_set.frame_no = dp_cap_settings->frame_no;
        
        #if (CIS_ENABLE_HX_AUTOI2C != 0x00)
        //init hxauotoi2c
    	if(g_app_dp_cur_cfg.init_type != APP_INIT_TYPE_VIDEO_STREAM)
    		app_cisdp_set_hxautoi2c(g_app_dp_cur_cfg.init_type);
        #endif
        
        g_datapath_case = SENSORDPLIB_PATH_INP_WDMA2;
    }

    //dbg_printf(DBG_LESS_INFO, "g_first_frame_done=%u\n", g_first_frame_done);

    if(g_first_frame_done == 0)
    {
        sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
        sensordplib_set_sensorctrl_start();
    }
    else
    {
        sensordplib_retrigger_capture();
    }

    return API_SUCC;
}

int8_t app_dp_capture_frame_with_cdm(const app_dp_cap_t* dp_cap_settings)
{
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T jpeg_cfg;
    HW2x2_CFG_T hw2x2_cfg;
    CDM_CFG_T cdm_cfg;

    if(g_first_frame_done == 0 ||
        g_datapath_case != SENSORDPLIB_PATH_INT1 ||
        g_app_dp_cap_set.frame_no != dp_cap_settings->frame_no ||
        g_app_dp_cap_set.hw55_crop_stx != dp_cap_settings->hw55_crop_stx ||
        g_app_dp_cap_set.hw55_crop_sty != dp_cap_settings->hw55_crop_sty ||
        g_app_dp_cap_set.hw55_in_width != dp_cap_settings->enc_width ||
        g_app_dp_cap_set.hw55_in_height != dp_cap_settings->enc_height ||
        g_app_dp_cap_set.hw55_path != dp_cap_settings->hw55_path ||
        g_app_dp_cap_set.hw55_y_only != dp_cap_settings->hw55_y_only ||
        g_app_dp_cap_set.enc_width != dp_cap_settings->enc_width ||
        g_app_dp_cap_set.enc_height != dp_cap_settings->enc_height ||
        g_app_dp_cap_set.jpeg_enctype != dp_cap_settings->jpeg_enctype ||
        g_app_dp_cap_set.jpeg_encqtable != dp_cap_settings->jpeg_encqtable)
    {
        hx_drv_hw2x2_get_defCfg(&hw2x2_cfg);
        hw2x2_cfg.hw2x2_path = DP_HW2X2_PATH;
        hw2x2_cfg.hw_22_process_mode = DP_HW2X2_PROCESS_MODE;/**< HW2x2 Subsample Mode*/
        hw2x2_cfg.hw_22_mono_round_mode = DP_HW2X2_ROUND_MODE;/**< HW2x2 Rounding Mode*/
        hw2x2_cfg.hw_22_crop_stx = DP_HW2X2_CROP_START_X;  /**< HW2x2 ROI Start X Position*/
        hw2x2_cfg.hw_22_crop_sty = DP_HW2X2_CROP_START_Y;  /**< HW2x2 ROI Start Y Position*/
        hw2x2_cfg.hw_22_in_width = DP_HW2X2_CROP_WIDTH; /**< HW2x2 ROI Width*/
        hw2x2_cfg.hw_22_in_height = DP_HW2X2_CROP_HEIGHT; /**< HW2x2 ROI Height*/

        hx_drv_cdm_get_defCfg(&cdm_cfg);
        cdm_cfg.cdm_enable = DP_CDM_ENABLE;             /*!< CDM enable */
        cdm_cfg.init_map_flag = DP_CDM_INIT_MAP_FLAG;       /*!< CDM initial map flag */
        cdm_cfg.cpu_activeflag = DP_CDM_CPU_ACTIVEFLAG;        /*!< CDM cpu active flag */
        cdm_cfg.meta_dump = DP_CDM_META_DUMP;        /*!< CDM meta data dump enable or disable */
        cdm_cfg.ht_packing = DP_CDM_HT_PACKING;      /*!< CDM meta data dump packing or unpacking */
        cdm_cfg.cdm_min_allow_dis = DP_CDM_MIN_ALLOW_DIS;              /*!< CDM algorithm minimum allowed distance between pixel and threshold setting*/
        cdm_cfg.cdm_tolerance = DP_CDM_TOLERANCE;              /*!< CDM algorithm tolerance setting*/
        cdm_cfg.cdm_reactance = DP_CDM_REACTANCE;              /*!< CDM algorithm reactance setting*/
        cdm_cfg.cdm_relaxation = DP_CDM_RELAXATION;                 /*!< CDM algorithm relaxation setting*/
        cdm_cfg.cdm_eros_th = DP_CDM_EROS_TH;                /*!< CDM algorithm erosion threshold setting*/
        cdm_cfg.cdm_num_ht_th = DP_CDM_NUM_HT_TH;                 /*!< CDM algorithm number of hot pixels threshold setting*/
        cdm_cfg.cdm_num_ht_vect_th_x = DP_CDM_NUM_HT_VECT_TH_X;           /*!< CDM algorithm threshold for number of hot-pixels in each bin of the x-projection vector setting*/
        cdm_cfg.cdm_num_ht_vect_th_y = DP_CDM_NUM_HT_VECT_TH_Y;           /*!< CDM algorithm threshold for number of hot-pixels in each bin of the y-projection vector setting*/
        cdm_cfg.cdm_num_cons_ht_bin_th_x = DP_CDM_NUM_CONS_HT_BIN_TH_X;       /*!< CDM algorithm threshold for number of consecutive hot bins of the x-projection vector setting*/
        cdm_cfg.cdm_num_cons_ht_bin_th_y = DP_CDM_NUM_CONS_HT_BIN_TH_Y;       /*!< CDM algorithm threshold for number of consecutive hot bins of the y-projection vector setting*/
        cdm_cfg.cdm_crop_stx = DP_CDM_IN_START_X;   /*!< CDM input ROI start x position*/
        cdm_cfg.cdm_crop_sty = DP_CDM_IN_START_Y;   /*!< CDM input ROI start y position*/
        cdm_cfg.cdm_in_width = DP_CDM_IN_WIDTH; /*!< CDM input ROI width*/
        cdm_cfg.cdm_in_height = DP_CDM_IN_HEIGHT;    /*!< CDM input ROI height*/

        hx_drv_hw5x5_get_defCfg(&hw5x5_cfg);
        /* HW5x5 Path */
        hw5x5_cfg.hw5x5_path = DP_HW5X5_PATH;
        /* HW5x5 Color Mode */
        if(dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV420) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV420;
        } else if (dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV400) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV400;
        } else if (dp_cap_settings->jpeg_enctype == JPEG_ENC_TYPE_YUV422) {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV422;
        } else {
            hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV400;
        }
        
        hw5x5_cfg.demos_bndmode = DP_HW5X5_DEMOS_BNDMODE;
        hw5x5_cfg.hw55_crop_stx = dp_cap_settings->hw55_crop_stx;
        hw5x5_cfg.hw55_crop_sty = dp_cap_settings->hw55_crop_sty;
        hw5x5_cfg.hw55_in_width = dp_cap_settings->enc_width;
        hw5x5_cfg.hw55_in_height = dp_cap_settings->enc_height;
#if 0
        hw5x5_cfg.wdma3_ch1_only = (1 == dp_cap_settings->hw55_y_only)?true:false;
#else
        #warning "TEMPORARY COMMENT OUT"
#endif
        
        g_app_dp_cap_set.hw55_path = dp_cap_settings->hw55_path;
        g_app_dp_cap_set.hw55_crop_stx = dp_cap_settings->hw55_crop_stx;
        g_app_dp_cap_set.hw55_crop_sty = dp_cap_settings->hw55_crop_sty;
        g_app_dp_cap_set.hw55_in_width = dp_cap_settings->enc_width;
        g_app_dp_cap_set.hw55_in_height = dp_cap_settings->enc_height;
        g_app_dp_cap_set.hw55_y_only = dp_cap_settings->hw55_y_only;
        
        hx_drv_jpeg_get_defCfg(&jpeg_cfg);

        jpeg_cfg.jpeg_path = JPEG_PATH_ENCODER_EN;
        jpeg_cfg.enc_width = dp_cap_settings->enc_width;
        jpeg_cfg.enc_height = dp_cap_settings->enc_height;
        jpeg_cfg.jpeg_enctype = (JPEG_ENC_TYPE_T)dp_cap_settings->jpeg_enctype;
        jpeg_cfg.jpeg_encqtable = (JPEG_ENC_QTABLE_T)dp_cap_settings->jpeg_encqtable;
        
        g_app_dp_cap_set.enc_width = dp_cap_settings->enc_width;
        g_app_dp_cap_set.enc_height = dp_cap_settings->enc_height;
        g_app_dp_cap_set.jpeg_enctype = dp_cap_settings->jpeg_enctype;
        g_app_dp_cap_set.jpeg_encqtable = dp_cap_settings->jpeg_encqtable;

        g_app_dp_cap_set.frame_no = dp_cap_settings->frame_no;
        
        //hx_dplib_evthandler_register_cb(app_dp_cb, SENSORDPLIB_CB_FUNTYPE_DP);
        sensordplib_set_INT1_HWACC_start_no(hw2x2_cfg
            ,cdm_cfg
            ,hw5x5_cfg
            ,jpeg_cfg
            ,g_app_dp_cur_cfg.wdma2_cyclic_buffer_cnt
            ,dp_cap_settings->frame_no
            ,app_dp_cb);
        
        #if (CIS_ENABLE_HX_AUTOI2C != 0x00)
        //init hxauotoi2c
    	if(g_app_dp_cur_cfg.init_type != APP_INIT_TYPE_VIDEO_STREAM)
    		app_cisdp_set_hxautoi2c(g_app_dp_cur_cfg.init_type);
        #endif
        
        g_datapath_case = SENSORDPLIB_PATH_INT1;

        g_first_frame_done = 0;
    }

    dbg_printf(DBG_LESS_INFO, "g_first_frame_done=%u\n", g_first_frame_done);
    
    if(g_first_frame_done == 0)
    {
        sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
        sensordplib_set_sensorctrl_start();
    }
    else
    {
        sensordplib_retrigger_capture();
    }

    return API_SUCC;
}

int8_t app_dp_get_def_init_cfg(app_dp_cfg_t* dp_init_cfg)
{ 
    dp_init_cfg->sensor_id = 0;
    dp_init_cfg->rev_id = 0;
    dp_init_cfg->sensor_width = SENCTRL_SENSOR_WIDTH;
    dp_init_cfg->sensor_height = SENCTRL_SENSOR_HEIGHT;
    dp_init_cfg->inp_out_width = DP_INP_OUT_WIDTH;
    dp_init_cfg->inp_out_height = DP_INP_OUT_HEIGHT;

    dp_init_cfg->init_type = CISDP_INIT_TYPE;
    dp_init_cfg->mclk_div = CISDP_SENSORCTRL_MCLK_DIV;
    dp_init_cfg->sclk_utg = SCLK_UTG;
    dp_init_cfg->h_fporch = RX_LBUF_INSERT_H_FPORCH;
    dp_init_cfg->sensor_type = SENCTRL_SENSOR_TYPE;
    dp_init_cfg->stream_type = SENCTRL_STREAM_TYPE;
    dp_init_cfg->inp_subsample_type = DP_INP_SUBSAMPLE;
    dp_init_cfg->xshutdown_pin = CISDP_CIS_XHSUTDOWN_PIN;
    #if (IC_VERSION >= 30)
    dp_init_cfg->inp_bin = DP_INP_BINNING;
    #endif
    dp_init_cfg->crop.start_x = DP_INP_CROP_START_X;
    dp_init_cfg->crop.start_y = DP_INP_CROP_START_Y;

    if(DP_INP_CROP_WIDTH >= 1)
    	dp_init_cfg->crop.last_x = DP_INP_CROP_WIDTH - 1;
    else
    	dp_init_cfg->crop.last_x = 0;

    if(DP_INP_CROP_HEIGHT >= 1)
    	dp_init_cfg->crop.last_y = DP_INP_CROP_HEIGHT - 1;
    else
    	dp_init_cfg->crop.last_y = 0;

    dp_init_cfg->wdma1 = (uint32_t)NULL;
    dp_init_cfg->wdma2 = (uint32_t)NULL;
    dp_init_cfg->wdma3 = (uint32_t)NULL;
    dp_init_cfg->jpeg_auto_fill_data = (uint32_t)NULL;
    dp_init_cfg->wdma2_cyclic_buffer_cnt = (uint8_t)0;
    return API_SUCC;
}

int8_t app_dp_get_def_cap_settings(app_dp_cap_t* dp_cap_settings)
{
    dp_cap_settings->frame_no = 0;
    dp_cap_settings->hw55_crop_stx = DP_HW5X5_CROP_START_X;
    dp_cap_settings->hw55_crop_sty = DP_HW5X5_CROP_START_Y;
    dp_cap_settings->hw55_in_width = DP_HW5X5_CROP_WIDTH;
    dp_cap_settings->hw55_in_height = DP_HW5X5_CROP_HEIGHT;
    dp_cap_settings->hw55_path = DP_HW5X5_PATH;
    dp_cap_settings->hw55_y_only = 0;
    dp_cap_settings->enc_width = DP_JPEG_ENC_WIDTH;
    dp_cap_settings->enc_height = DP_JPEG_ENC_HEIGHT;
    dp_cap_settings->jpeg_enctype = DP_JPEG_ENCTYPE;
    dp_cap_settings->jpeg_encqtable = DP_JPEG_ENCQTABLE;

    return API_SUCC;
}
int8_t app_dp_get_def_dec_jpeg_settings(app_dp_jpg_dec_t* dp_jpg_dec_settings)
{
    dp_jpg_dec_settings->frame_no = 0;
    dp_jpg_dec_settings->dec_roi_stx = DP_JPEG_DEC_ROI_START_X;
    dp_jpg_dec_settings->dec_roi_sty = DP_JPEG_DEC_ROI_START_Y;
    dp_jpg_dec_settings->dec_width = DP_JPEG_DEC_WIDTH;
    dp_jpg_dec_settings->dec_height = DP_JPEG_DEC_HEIGHT;
    dp_jpg_dec_settings->jpeg_enctype = DP_JPEG_ENCTYPE;
    dp_jpg_dec_settings->jpeg_encqtable = DP_JPEG_ENCQTABLE;

    return API_SUCC;
}
int8_t app_dp_get_def_enc_jpeg_settings(app_dp_jpg_enc_t* dp_jpg_enc_settings)
{
    dp_jpg_enc_settings->frame_no = 0;
    dp_jpg_enc_settings->enc_width = DP_JPEG_ENC_WIDTH;
    dp_jpg_enc_settings->enc_height = DP_JPEG_ENC_HEIGHT;
    dp_jpg_enc_settings->jpeg_enctype = DP_JPEG_ENCTYPE;
    dp_jpg_enc_settings->jpeg_encqtable = DP_JPEG_ENCQTABLE;

    return API_SUCC;
}


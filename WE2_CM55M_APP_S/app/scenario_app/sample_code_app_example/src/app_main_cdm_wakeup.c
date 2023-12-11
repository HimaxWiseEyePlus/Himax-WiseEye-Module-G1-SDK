#include "app_platform.h"
#include "app_api.h"
#include "app_sys_info_cmd.h"
#include "app_datapath.h"
#include "app_board.h"
#include "app_xdma_cfg.h"
#include "app_i2c_cmd.h"
#include "app_pmu.h"
#include "app_cisdp_cfg.h"

#include "hx_drv_pmu.h"
#include "hx_drv_scu_export.h"

#include "powermode_export.h"


/* local variables */
static volatile uint8_t g_wdma1_data[WDMA1_SIZE] = {};
static volatile uint8_t g_wdma2_data[WDMA2_SLOT_SIZE*WDMA2_CYCLIC_BUF_CNT] = {};
static volatile uint8_t g_wdma3_data[WDMA3_SIZE] = {};
static volatile uint8_t g_jpeg_autofill_data[JPEG_AUTOFILL_SIZE] = {};

void app_set_vidpre_cdmwakeup();

void clear_wakeup_evt()
{
	uint32_t wakeup_event, wakeup_event1;
	hx_lib_pm_clear_event();
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
    dbg_printf(DBG_LESS_INFO, "after clear WakeupEvt=0x%x\n", wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
    dbg_printf(DBG_LESS_INFO, "after clear WakeupEvt1=0x%x\n", wakeup_event1);
}

void clear_all_sysip_int()
{
#ifdef IP_rtc
	hx_drv_rtc_clear_alarm_int_status(RTC_ID_0);
	hx_drv_rtc_clear_alarm_int_status(RTC_ID_1);
	hx_drv_rtc_clear_alarm_int_status(RTC_ID_2);
#endif
#ifdef IP_timer
	hx_drv_timer_ClearIRQ(TIMER_ID_0);
	hx_drv_timer_ClearIRQ(TIMER_ID_1);
	hx_drv_timer_ClearIRQ(TIMER_ID_2);
	hx_drv_timer_ClearIRQ(TIMER_ID_3);
#ifdef WE2_SINGLE_CORE
	hx_drv_timer_ClearIRQ(TIMER_ID_4);
	hx_drv_timer_ClearIRQ(TIMER_ID_5);
	hx_drv_timer_ClearIRQ(TIMER_ID_6);
	hx_drv_timer_ClearIRQ(TIMER_ID_7);
	hx_drv_timer_ClearIRQ(TIMER_ID_8);
#endif
#endif
#ifdef IP_watchdog
	hx_drv_watchdog_irq_clear(WATCHDOG_ID_0);
	hx_drv_watchdog_irq_clear(WATCHDOG_ID_1);
#endif
#ifdef IP_gpio
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_clr_int_status(AON_GPIO1);
	hx_drv_gpio_clr_int_status(SB_GPIO0);
	hx_drv_gpio_clr_int_status(SB_GPIO1);
#endif
#ifdef IP_hxautoi2c_mst
	hx_drv_hxautoi2c_clr_noack_int(HXAUTOI2CHC_INTSTATUS_DATA_NO_ACK);
	hx_drv_hxautoi2c_clr_noack_int(HXAUTOI2CHC_INTSTATUS_ADDR_NO_ACK);
#endif
}

void app_init()
{
	app_dp_cfg_t app_dp_init_cfg;
    APP_SPI_CFG_T spi_cfg;
	
    app_board_pinmux_settings();

    #ifdef SPI_MASTER_SEND /*master*/
    spi_cfg.mst_id = SPI_MST_1; /**< id*/
    //spi_cfg.mst_freq = 10000000;  /**< frequency*/
    spi_cfg.mst_freq = 5000000;  /**< frequency*/
    #endif
    
    #ifdef SPI_SLAVE_SEND /*slave*/
    spi_cfg.slv_handshake_pin = HX_AON_GPIO_0;         /**< handshake gpio pin number*/
    spi_cfg.slv_handshake_actv_lvl = HX_GPIO_HIGH;     /**< handshake gpio pin active level*/
    
    dbg_printf(DBG_LESS_INFO, "HX_AON_GPIO_0 %02x\n", HX_AON_GPIO_0);
    dbg_printf(DBG_LESS_INFO, "HX_GPIO_HIGH %u\n", HX_GPIO_HIGH);
    
    dbg_printf(DBG_LESS_INFO, "spi_cfg.slv_handshake_pin %02x\n", spi_cfg.slv_handshake_pin);
    dbg_printf(DBG_LESS_INFO, "spi_cfg.slv_handshake_actv_lvl %u\n", spi_cfg.slv_handshake_actv_lvl);
    #endif
    
	#if defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND)
    app_spi_init(&spi_cfg);
    #endif

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
	uint32_t wakeup_event = 0;
    uint32_t wakeup_event1 = 0;
	uint32_t cnt = 0;
	
    app_dp_cap_t app_dp_cap_set;
	
	uint32_t raw_img_addr = 0, img_width = 0, img_heigh = 0;
	uint32_t jpg_addr = 0, jpg_size = 0;
	
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	dbg_printf(DBG_LESS_INFO, "WakeupEvt=0x%x\n", wakeup_event);

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
	dbg_printf(DBG_LESS_INFO, "WakeupEvt1=0x%x\n", wakeup_event1);
	
	print_wakeup_event(wakeup_event, wakeup_event1);
	
	#ifdef WE2_DUAL_CORE
	dbg_printf(DBG_LESS_INFO, "CM55M_ENABLE_CM55S \n\n\n");
	hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
	hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
	hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);
	
	dbg_printf(DBG_LESS_INFO, "app_main start(dual core).\n");
	#endif
	
	app_debug_print();

	hx_drv_timer_sensor_stop(); /*TIMER_ID_0*/
    hx_drv_timer_hw_stop(TIMER_ID_1); /*TIMER_ID_1*/
	hx_drv_timer_hw_stop(TIMER_ID_2); /*TIMER_ID_2*/
	hx_drv_timer_hw_stop(TIMER_ID_3); /*TIMER_ID_3*/
    
    hx_lib_pm_pmu_dpdone_check(NULL);
    hx_lib_pm_ctrl_fromPMUtoCPU(NULL);
	
    app_init();
	
	if((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE))
	{
	}
	else
	{
	}



    /* Set DP settings */
    app_dp_get_def_cap_settings(&app_dp_cap_set);

	#if 1
    while(cnt++ < 10)
    {
        /* Capture jpeg and raw image */
        dbg_printf(DBG_LESS_INFO, "[app_dp_capture_jpeg_frame]\n");
        app_dp_capture_jpeg_frame(&app_dp_cap_set);

		dbg_printf(DBG_LESS_INFO, "[app_dp_get_frame]\n");
        /* Capture sensor output to jpeg and raw */
        app_dp_get_frame(&raw_img_addr, &img_width, &img_heigh, &jpg_addr, &jpg_size);
        dbg_printf(DBG_LESS_INFO, "[app_dp_get_frame] raw_img_addr = %x img_width = %d img_width = %d, jpg_addr = %x, jpg_size = %d\n", raw_img_addr, img_width, img_heigh, jpg_addr, jpg_size);

        /* Send out image by SPI */
        #if defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND)
        if (jpg_addr != 0)
            app_spi_write(jpg_addr, jpg_size, DATA_TYPE_JPG);
        #endif
		
		board_delay_ms(10);
    }
	#endif
	
	/* Set DP settings */
    app_dp_get_def_cap_settings(&app_dp_cap_set);
	
    /* Capture jpeg and raw image with cdm*/
    dbg_printf(DBG_LESS_INFO, "[app_dp_capture_with_cdm]\n");
    app_dp_capture_frame_with_cdm(&app_dp_cap_set);
    
    app_dp_get_frame(&raw_img_addr, &img_width, &img_heigh, &jpg_addr, &jpg_size);
    dbg_printf(DBG_LESS_INFO, "[app_dp_get_frame] raw_img_addr = %x, img_width = %d, img_heigh = %d, jpg_addr = %x, jpg_size = %d\n", raw_img_addr, img_width, img_heigh, jpg_addr, jpg_size);
    
    if(raw_img_addr && img_width && img_heigh)
    {
        dbg_printf(DBG_LESS_INFO,"set init map off and CPU on \n");

        if(hx_drv_cdm_set_initmapFlag(CDM_INIMAP_FLAG_OFF) != CDM_NO_ERROR)
        {
            dbg_printf(DBG_LESS_INFO,"hx_drv_cdm_set_initmapFlag fail\n");
        }
    }

    if(hx_drv_cdm_set_CPUActiveFlag(CDM_CPU_ACTFLAG_ACTIVE) != CDM_NO_ERROR)
    {
        dbg_printf(DBG_LESS_INFO,"hx_drv_cdm_set_CPUActiveFlag fail\n");
    }
	
    clear_wakeup_evt();
    clear_all_sysip_int();

	AppCfgCustGpio_t gpio_cfg = {0};
	app_pmu_enter_sleep1_aos(gpio_cfg, 330, 0, 1);
}


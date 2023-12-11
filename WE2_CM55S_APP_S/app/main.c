/**************************************************************************************************
 (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
 ------------------------------------------------------------------------
 File        : main.c
 Project     : EPII
 DATE        : 2021/04/01
 AUTHOR      : 902447
 BRIFE       : main function
 HISTORY     : Initial version - 2021/04/01 created by Jacky
 **************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "board.h"
#include "xprintf.h"
#define CIS_XSHUT_SGPIO0

#ifdef HM_COMMON
#include "hx_drv_CIS_common.h"
#endif
#if defined(CIS_HM0360_MONO_REVB) || defined(CIS_HM0360_MONO_OSC_REVB) \
    || defined(CIS_HM0360_BAYER_REVB) || defined(CIS_HM0360_BAYER_OSC_REVB) \
    ||  defined(CIS_HM0360_MONO) || defined(CIS_HM0360_BAYER)
#include "hx_drv_hm0360.h"
#endif
#ifdef CIS_HM11B1
#include "hx_drv_hm11b1.h"
#endif
#if defined(CIS_HM01B0_MONO) || defined(CIS_HM01B0_BAYER)
#include "hx_drv_hm01b0.h"
#endif
#ifdef CIS_HM2140
#include "hx_drv_hm2140.h"
#endif
#ifdef CIS_XSHUT_SGPIO0
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2 
#else
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2 
#endif

/******************************************************/
// ifeq ($(APP_TYPE), CLI_PERIPHERAL)
/******************************************************/
#ifdef MID_CLI
#ifdef TRUSTZONE_SEC
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
#ifdef NSC
#include "veneer_table.h"
#endif
#ifndef TRUSTZONE_SEC_ONLY
#include "trustzone_cfg.h"
#endif
#endif

#include "cli.h"
#include "board.h"
#ifdef LIB_COMMON
#include "console_io.h"
#include "xprintf.h"
#endif
#include "hx_drv_timer.h"

void cli_app_start()
{
#ifdef FREERTOS
	  const TickType_t xDelay = pdMS_TO_TICKS(1);
#endif
#ifdef CLI_ENABLE
#ifdef MID_CLI
#ifdef CLI_TZS_MPC_AT_S
	  SCB_DisableICache();
	  SCB_DisableDCache();
#endif
	xprintf("[CM55S S CLI Program V0.3]\r\n");
#ifdef HM_COMMON
	//init sensor
	{
        xprintf("init sensor\n");
        /*disable INTC side effect period is accurate*/
        //  int_disable(BOARD_SYS_TIMER_INTNO); /* disable timer interrupt (method 1 to stop capture frame)*/
        //  int_disable(BOARD_STD_TIMER_ID); /* disable timer interrupt (method 1 to stop capture frame)*/
#if defined(CIS_HM0360_MONO_OSC_REVB) || defined(CIS_HM0360_BAYER_OSC_REVB)
        hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_RC36M);
#else
#ifdef HM_COMMON
        hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_XTAL);
#endif
#endif
#if defined(CIS_HM01B0_MONO) || defined(CIS_HM01B0_BAYER) || defined(CIS_COMMON) || defined(CIS_HM1245) || defined(CIS_HM2140) || defined(CIS_HM0360_MONO) || defined(CIS_HM0360_MONO_OSC) || defined(CIS_HM0360_BAYER) || defined(CIS_HM0360_BAYER_OSC) || defined(CIS_HM0360_MONO_REVB) || defined(CIS_HM0360_MONO_OSC_REVB) || defined(CIS_HM0360_BAYER_REVB) || defined(CIS_HM0360_BAYER_OSC_REVB)
#ifdef IC_PACKAGE_WLCSP38
        hx_drv_sensorctrl_set_wlcsp_sharepin(SENSORCTRL_WLCSP_SHAREPIN_HM01B0);
#endif
        DP_MCLK_SRC_INT_EXT_E clk_int_ext;
        DP_MCLK_SRC_INT_SEL_E clk_int_src;
        hx_drv_dp_get_mclk_src(&clk_int_ext, &clk_int_src);
        xprintf("mclk_int_ext=%d,mclk_int_src=%d\n",clk_int_ext, clk_int_src);
        if(clk_int_src == DP_MCLK_SRC_INT_SEL_XTAL)
        {
            xprintf("mclk DIV2, xshutdown_pin=%d\n",DEAULT_XHSUTDOWN_PIN);
#if defined(CIS_HM01B0_MONO) || defined(CIS_HM01B0_BAYER)
#if (HM01B0_MCLK_MHZ == 24)
            hx_drv_cis_init(CIS_XHSUTDOWN_IOMUX_NONE, SENSORCTRL_MCLK_DIV1);
#elif (HM01B0_MCLK_MHZ == 18)
            hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_RC36M);
            hx_drv_cis_init(CIS_XHSUTDOWN_IOMUX_NONE, SENSORCTRL_MCLK_DIV2);
#elif (HM01B0_MCLK_MHZ == 12)
            hx_drv_cis_init(CIS_XHSUTDOWN_IOMUX_NONE, SENSORCTRL_MCLK_DIV2);
#endif
#elif defined(HM0360_MONO_OSC) || defined(HM0360_BAYER_OSC) || defined(CIS_HM0360_MONO_OSC_REVB) || defined(CIS_HM0360_BAYER_OSC_REVB)
            hx_drv_cis_init(DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_NO);
#else
#if defined(CIS_HM0360_MONO_REVB) || defined(CIS_HM0360_BAYER_REVB)
#if(HM0360_MCLK_MHZ == 12)
            hx_drv_cis_init(DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_DIV2);
#elif(HM0360_MCLK_MHZ == 24)
            hx_drv_cis_init(DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_DIV1);
#endif
#else
            hx_drv_cis_init(DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_DIV2);
#endif
#endif
        }else{
            xprintf("mclk DIV3, xshutdown_pin=%d\n",DEAULT_XHSUTDOWN_PIN);
#if defined(CIS_HM01B0_MONO) || defined(CIS_HM01B0_BAYER)
            hx_drv_cis_init(CIS_XHSUTDOWN_IOMUX_NONE, SENSORCTRL_MCLK_DIV3);
#else
            hx_drv_cis_init(DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_DIV3);
#endif
        }
#endif
#ifdef CIS_HM0360_FPGA
        hx_drv_hm0360fpga_init(DEAULT_XHSUTDOWN_PIN);
#endif
#ifdef CIS_HM11B1
#ifdef IC_PACKAGE_WLCSP38
        hx_drv_sensorctrl_set_wlcsp_sharepin(SENSORCTRL_WLCSP_SHAREPIN_HM11B1);
#endif
        hx_drv_hm11b1_init(DEAULT_XHSUTDOWN_PIN);
#endif

	} //end of init sensor
#endif

    CLI_InitMenu();
#ifdef CLI_ENABLE_PASSWORD
    CLI_EnablePassword(FALSE);
    CLI_EnablePassword(TRUE);
#endif
	CLI_CommandLoop();
#endif
#endif
	while(1)
	{
#ifdef FREERTOS
		vTaskDelay( xDelay );
#else
#ifdef CM55_BIG
		hx_drv_timer_cm55m_delay_ms(1, TIMER_STATE_DC);
#else
		hx_drv_timer_cm55s_delay_ms(1, TIMER_STATE_DC);
#endif
#endif
	}
}


/** main entry */
int main(void)
{
	board_init();
#ifdef FREERTOS
    if (xTaskCreate(cli_task, "cli_task", configMINIMAL_STACK_SIZE + 100, NULL, hello_task_PRIORITY, NULL) !=
        pdPASS)
    {
        xprintf("Task creation failed!.\r\n");
        while (1)
            ;
    }
    vTaskStartScheduler();
    for (;;)
        ;
#else
    cli_app_start();
    while (1){
    	printf("end of program");
    }
#endif
	return 0;
}
#endif

#ifdef HELLO_WORLD_TZ
#include "hello_world_tz.h"

/** main entry */
int main(void)
{
	board_init();
	hello_world_app();
	return 0;
}
#endif

#ifdef HELLO_WORLD_FREERTOS_TZ
#include "hello_world_freertostz.h"

/** main entry */
int main(void)
{
	board_init();
	hello_world_app();
	return 0;
}
#endif

#ifdef HELLO_WORLD_FREERTOSTZ_MPU
#include "hello_world_freertostz_mpu.h"

/** main entry */
int main(void)
{
	board_init();
	hello_world_app();
	return 0;
}
#endif

#ifdef HELLO_WORLD_TZ_S_ONLY
#include "hello_world_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
	hello_world_app();
	return 0;
}
#endif

#ifdef HELLO_WORLD_FREERTOS_TZ_S_ONLY
#include "hello_world_freertos_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
	hello_world_app();
	return 0;
}
#endif

#ifdef HELLO_WORLD_FREERTOS_MPU_TZ_S_ONLY
#include "hello_world_freertos_mpu_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
	hello_world_app();
	return 0;
}
#endif

#ifdef HELLO_WORLD_NOTZ
#include "hello_world_notz.h"

/** main entry */
int main(void)
{
	board_init();
	hello_world_app();
	return 0;
}
#endif

#ifdef TZ_TO_CLI
#include "tz_to_cli.h"

/** main entry */
int main(void)
{
	board_init();
	tz_to_cli_app();
	return 0;
}
#endif

#ifdef TZ_TO_CLI_DUAL
#include "tz_to_cli_dual.h"

/** main entry */
int main(void)
{
	board_init();
	tz_to_cli_dual_app();
	return 0;
}
#endif

#ifdef TZ_TO_CLI_DUAL_PMU
#include "tz_to_cli_dual_pmu.h"

/** main entry */
int main(void)
{
	board_init();
	tz_to_cli_dual_pmu_app();
	return 0;
}
#endif

#ifdef TZ_TO_CLI_DUAL_PERIPHERAL
#include "tz_to_cli_dual_peripheral.h"

/** main entry */
int main(void)
{
	board_init();
	tz_to_cli_dual_peripheral_app();
	return 0;
}
#endif

#ifdef HWACCBYTPG_AUTO_TEST
#ifdef BYPASS_TEST
#include "bypass_test.h"
#endif


int main(void)
{
	board_init();
	printf("AUTOTEST Start\n");
#ifdef BYPASS_TEST
	BYPASS_AUTOTEST_ALL();
#endif
	printf("AUTOTEST End\n");
}

#endif//HWACCBYTPG_AUTO_TEST

#ifdef HELLO_WORLD_EVT_TZ_S_ONLY
#include "hello_world_evt_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif

#ifdef AIOT_NB_EXAMPLE_TZ_S_ONLY
#include "aiot_nb_example_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
    notebook_example_app();
    return 0;
}
#endif

#ifdef SCENARIO_DC_SCL_SCH_TZ_S_ONLY
#include "dc_scl_sch_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
    return 0;
}
#endif

#ifdef swvad_hwvad_evt_tz_s_only
#include "swvad_hwvad_evt_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
	swvad_hwvad_evt_app();
	return 0;
}
#endif

#ifdef PMU_SIM_AUDPRE_CM55S_TIMER1
#include "pmu_sim_audpre_cm55s_timer1.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif

#ifdef PMU_SIM_AUDPRE_CM55S_VAD
#include "pmu_sim_audpre_cm55s_vad.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif


#ifdef PMU_SIM_AUDVIDPRE_CM55S_TIMER1
#include "pmu_sim_audvidpre_cm55s_timer1.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif

#ifdef PMU_SIM_AUDVIDPRE_CM55S_VAD
#include "pmu_sim_audvidpre_cm55s_vad.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif

#ifdef TZ_TO_CLI_DUAL_TIMER
#include "tz_to_cli_dual_timer.h"

/** main entry */
int main(void)
{
	board_init();
	tz_to_cli_dual_app();
	return 0;
}
#endif

#ifdef PMU_SIM_DUALCORE_CM55S_BUS_ACCESS
#include "pmu_sim_dualcore_cm55s_bus_access.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif

#ifdef SIM_TZ_S_ONLY
#include "sim_tz_s_only.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif

#ifdef PMU_SIM_AUDPRE_CM55S_FORCE
#include "pmu_sim_audpre_cm55s_force.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif

#ifdef PMU_SIM_AUDVIDPRE_CM55S_FORCE
#include "pmu_sim_audvidpre_cm55s_force.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif


#ifdef PMU_SIM_AUDPRE_CM55S_LOWFREQ
#include "pmu_sim_audpre_cm55s_lowfreq.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif


#ifdef SIM_PER
#include "sim_per.h"

/** main entry */
int main(void)
{
	board_init();
    hello_world_app();
    return 0;
}
#endif

#ifdef PMU_AUDPRE_CM55S_TIMER2
#include "pmu_audpre_cm55s_timer2_sample.h"

/** main entry */
int main(void)
{
	board_init();
	pmu_audpre_cm55s_timer2_app();
    return 0;
}
#endif

#ifdef PMU_SIM_AUDVIDPRE_CM55S_TIMER2
#include "pmu_audvidpre_cm55s_timer2_sample.h"

/** main entry */
int main(void)
{
	board_init();
	pmu_audvidpre_cm55s_timer2_app();
    return 0;
}
#endif

#ifdef APP_API
#include "app_main.h"

int main(void)
{
	board_init();
	app_main();

	return 0;
}
#endif

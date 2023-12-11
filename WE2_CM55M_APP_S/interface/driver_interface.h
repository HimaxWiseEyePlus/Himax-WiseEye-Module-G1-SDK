/*
 * driver_interface.h
 *
 *  Created on: 2023�~5��9��
 *      Author: 902447
 */

#ifndef INTERFACE_DRIVER_INTERFACE_H_
#define INTERFACE_DRIVER_INTERFACE_H_

#include "hx_drv_scu_export.h"
#ifdef IP_rtc
#include "hx_drv_rtc.h"
#endif
#ifdef IP_watchdog
#include "hx_drv_watchdog.h"
#endif
#ifdef IP_timer
#include "hx_drv_timer.h"
#endif
#ifdef IP_timer
#include "timer_interface.h"
#endif
/**
 * \enum DRIVER_INTERFACE_E
 * \brief Driver Interface error
 */
typedef enum
{
	DRIVER_INTERFACE_NO_ERROR                     	= 0,	/**< No ERROR */
	DRIVER_INTERFACE_UNKNOWN_ERROR      			= 1,	/**< ERROR MSG: UNKNOWN ERROR*/
} DRIVER_INTERFACE_E;

/**
 * \brief	get current Frequency
 *
 * \param[in]	type	 Frequency Type
 * \param[out]	freq	 Freq
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_freq(SCU_CLK_FREQ_TYPE_E type, uint32_t *freq);
#ifdef IP_rtc
/**
 * \brief	set rtc clock enable
 *
 * \param[in]	type	 Frequency Type
 * \param[out]	freq	 Freq
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_rtc_clk_en(RTC_ID_E id, uint8_t clk_en);

/**
 * \brief	set RTC cycle width
 *
 * \param[in]	cycle_width	 cycle width (if clock is 24Mhz, cycle_width 24Mhz is 1 HZ for RTC)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_rtc_cycle(RTC_ID_E id, uint32_t cycle_width);
#endif
#ifdef IP_watchdog
/**
 * \brief	set WDG Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_wdg_clk_en(WATCHDOG_ID_E id, uint8_t clk_en);

/**
 * \brief	set WDG clk div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_wdg_clkdiv(WATCHDOG_ID_E id, uint32_t clk_div);

/**
 * \brief	set WDT control owner
 *
 * \param[in]	id 0 or 1
 * \param[in]	ctrl enable by CPU or PMU
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_wdt_ctrl(WATCHDOG_ID_E id, SCU_WDTCTRL_E ctrl);
#endif
#ifdef IP_timer
/**
 * \brief	set Timer control owner
 *
 * \param[in]	id 0 ~ 8
 * \param[in]	ctrl enable by CPU or PMU
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_ctrl(TIMER_ID_E id, SCU_TIMERCTRL_E ctrl);

/**
 * \brief	set Timer Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_clk_en(TIMER_ID_E id, uint8_t clk_en);

/**
 * \brief	set Timer Clock Div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_timer_clkdiv(TIMER_ID_E id, uint32_t clk_div);
#endif

/**
 * \brief	set ADC LV Reset
 *
 * \param[in]	cfg	 reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_lsc_adclv_reset(SCU_SWRESET_E cfg);

/**
 * \brief	set ADC HV Reset
 *
 * \param[in]	cfg	 reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_lsc_adchv_reset(SCU_SWRESET_E cfg);

/**
 * \brief	set PD LSC adcckin CLK Configuration
 *
 * \param[in]	cfg	 PD LSC adcckin CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdlsc_adcckinclk_cfg(SCU_PDLSC_ADCCKIN_CFG_T cfg);

/**
 * \brief	set PDM Clk Div
 *
 * \param[in]	clk_div	 PDM Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdm_clkdiv(uint8_t clk_div);

/**
 * \brief	set Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[in]	channel	 capture channel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E channel);

/**
 * \brief	set PD LSC PDM CLK Source
 *
 * \param[in]	cfg	 PD LSC PDM CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdlsc_pdmck_src(SCU_LSCPDMCLKSRC_E pdmsrc_clk);

/**
 * \brief	set VAD D SRC Selection
 *
 * \param[in]	data	 VAD D SRC Sel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_vad_d_src_sel(SCU_LSCVADDCLKSRC_E data);

/**
 * \brief	set VAD D PCM Selection
 *
 * \param[in]	data	 VAD D PCM Sel
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_vad_d_pcm_sel(SCU_VAD_PCM_DATA_SEL_E data);


/**
 * \brief	set I2S Clk Div
 *
 * \param[in]	clk_div	 I2S Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_i2s_clkdiv(uint8_t clk_div);

/**
 * \brief	set I2S mode
 *
 * \param[in]	mode	 I2S mode
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_i2s_mode(SCU_I2S_MODE_E mode);

/**
 * \brief	set DP SWReset
 *
 * \param[in]	cfg_swreset	 DP SW Reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_dp_swreset(SCU_DP_SWRESET_T cfg_swreset);


/**
 * \brief	get DP SWReset
 *
 * \param[out]	cfg_swreset	 DP SW Reset
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_dp_swreset(SCU_DP_SWRESET_T *cfg_swreset);

/**
 * \brief	set PD HSC DP CLK Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK Configuration
 * \param[in]	change_dprx	 change DP RX or not
 * \param[in]	change_dp	 change DP or not
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T cfg, uint8_t change_dprx, uint8_t change_dp);


/**
 * \brief	get PD HSC DP CLK Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T *cfg);

/**
 * \brief	set PD HSC DP CLK enable Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_set_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T cfg);


/**
 * \brief	get PD HSC DP CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK enable Configuration
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T *cfg);

/**
 * \brief	get CPU Clock
 *
 * \param[out]	cpu_clk	 Get CPU clock
 * \return	DRIVER_INTERFACE_E.
 */
DRIVER_INTERFACE_E drv_interface_get_cpu_clk(uint32_t *cpu_clk);

#endif /* INTERFACE_DRIVER_INTERFACE_H_ */

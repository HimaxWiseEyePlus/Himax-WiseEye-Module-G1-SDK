/**
 * @file hm01b0_mono_cis_cfg.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __HM01B0_MONO_CIS_CFG_H__
#define __HM01B0_MONO_CIS_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_CIS_common.h"
#include "sensor_dp_lib.h"

#define HM01B0_MCLK_FOR_APP   12    /**< HM01B0 MCLK 12M */

#ifdef HX6540_HM01B0_I2S_CCM
#define CIS_IO_1BIT_LSB   /**< HM01B0 CIS output format 1BIT*/
#define RC_36M    /**< HM01B0 MONO CIS clock souce RC 36M*/
#else
//#define CIS_IO_4BIT_LSB   /**< HM01B0 CIS output format 4BIT LSB*/
#define CIS_IO_8BIT_LSB     /**< HM01B0 CIS output format 8BIT LSB*/
#define XTAL_24M    /**< HM01B0 MONO CIS clock souce XTAL 24M*/
#endif

/**
 * \enum HM01B0_TP_TYPE_E
 * \brief HM01B0 test pattern enum
 */
typedef enum HM01B0_TP_TYPE_S
{
  HM01B0_TP_OFF                 = 0,	/**< 0: TP OFF */
  HM01B0_COLORBAR               = 1,	/**< 1: Colorbar */
  HM01B0_WALKING_1              = 2,	/**< 2: Walking 1 */
} HM01B0_TP_TYPE_E;

/**
 * \brief	set hm01b0 mono cis default settings
 *
 * \return	success (0) or error (-1)
 */
int hm01b0_mono_cis_def_cfg(SENSORDPLIB_SENSOR_E sensor_type);

/**
 * \brief	set hm01b0 mono cis test pattern output type
 *
 * \param[in]	m_pattern	    test pattern output type
 * \return	cis error code
 */
HX_CIS_ERROR_E hm01b0_mono_cis_set_tp(HM01B0_TP_TYPE_E m_pattern);

/**
 * \brief	set hm01b0 mono cis streaming on
 *
 * \param[in]	sensorstream_type	    streaming type
 * \return	void
 */
void hm01b0_mono_cis_streaming_on(SENSORDPLIB_STREAM_E sensorstream_type);

/**
 * \brief	set hm01b0 mono cis streaming off
 *
 * \return	void
 */
void hm01b0_mono_cis_streaming_off(void);

#ifdef __cplusplus
}
#endif

#endif /*__HM01B0_MONO_CIS_CFG_H__*/

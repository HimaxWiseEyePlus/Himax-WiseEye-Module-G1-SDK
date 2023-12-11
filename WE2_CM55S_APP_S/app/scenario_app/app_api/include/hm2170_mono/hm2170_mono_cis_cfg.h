/**
 * @file hm2170_mono_cis_cfg.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __HM2170_MONO_CIS_CFG_H__
#define __HM2170_MONO_CIS_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_CIS_common.h"
#include "sensor_dp_lib.h"

//#define RC_36M /**< HM2170 MONO CIS clock souce RC 36M*/
#define XTAL_24M /**< HM2170 MONO CIS clock souce XTAL 24M*/

/**
 * \brief	set hm2170 mono cis default settings
 *
 * \return	success (0) or error (-1)
 */
int hm2170_mono_cis_def_cfg(void);

/**
 * \brief	set hm2170 mono cis streaming on
 *
 * \return	void
 */
void hm2170_mono_cis_streaming_on(void);

/**
 * \brief	set hm2170 mono cis streaming off
 *
 * \return	void
 */
void hm2170_mono_cis_streaming_off(void);

#ifdef __cplusplus
}
#endif

#endif /*__HM2170_MONO_CIS_CFG_H__*/


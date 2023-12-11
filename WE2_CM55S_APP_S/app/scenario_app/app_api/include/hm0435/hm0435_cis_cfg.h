/**
 * @file hm0435_cis_cfg.h
 * @brief 
 * @version V1.0.0
 * @date 2022-12-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __HM0435_CIS_CFG_H__
#define __HM0435_CIS_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_CIS_common.h"
#include "sensor_dp_lib.h"
#include "app_datapath.h"

//#define RC_36M /**< HM0435 CIS clock souce RC 36M*/
#define XTAL_24M /**< HM0435 CIS clock souce XTAL 24M*/

/**
 * \brief	set hm0435 cis default settings
 *
 * \return	success (0) or error (-1)
 */
int hm0435_cis_def_cfg(void);

/**
 * \brief	set hm0435 bayer cis streaming on
 *
 * \return	void
 */
void hm0435_cis_streaming_on(void);

/**
 * \brief	set hm0435 bayer cis streaming off
 *
 * \return	void
 */
void hm0435_cis_streaming_off(void);

/**
 * \brief	set hm0435 bayer cis get sensor id
 *
 * \return	void
 */
int8_t hm0435_cis_get_sensor_id(uint16_t *model_id, uint8_t *rev_id);

#ifdef __cplusplus
}
#endif

#endif /*__HM0435_CIS_CFG_H__*/


/**
 * @file jxf51_bayer_cis_cfg.h
 * @brief 
 * @version V1.0.0
 * @date 2022-09-16
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __JXF51_BAYER_CIS_CFG_H__
#define __JXF51_BAYER_CIS_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_CIS_common.h"
#include "sensor_dp_lib.h"

//#define RC_36M /**< JXF51 BAYER CIS clock souce RC 36M*/
#define XTAL_24M /**< JXF51 BAYER CIS clock souce XTAL 24M*/

/**
 * \brief	set jxf51 bayer cis default settings
 *
 * \return	success (0) or error (-1)
 */
int jxf51_bayer_cis_def_cfg(void);

/**
 * \brief	set jxf51 bayer cis streaming on
 *
 * \return	void
 */
void jxf51_bayer_cis_streaming_on(void);

/**
 * \brief	set jxf51 bayer cis streaming off
 *
 * \return	void
 */
void jxf51_bayer_cis_streaming_off(void);

/**
 * \brief	set jxf51 bayer cis init
 *
 * \param[in]	xshutdown_pin   xshutdown pin
 * \param[in]	mclk_div	    mclk division
 * \return	int
 */
int jxf51_bayer_cis_init(CIS_XHSHUTDOWN_INDEX_E xshutdown_pin, SENSORCTRL_MCLK_E mclk_div);

/**
 * \brief	set jxf51 bayer cis get sensor id
 *
 * \return	void
 */
int jxf51_bayer_cis_get_sensor_id(uint16_t *model_id, uint8_t *rev_id);

#ifdef __cplusplus
}
#endif

#endif /*__JXF51_BAYER_CIS_CFG_H__*/


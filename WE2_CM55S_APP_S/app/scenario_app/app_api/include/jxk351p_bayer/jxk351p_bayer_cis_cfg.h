/**
 * @file jxk351p_bayer_cis_cfg.h
 * @brief 
 * @version V1.0.0
 * @date 2023-01-06
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __JXK351P_BAYER_CIS_CFG_H__
#define __JXK351P_BAYER_CIS_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_CIS_common.h"
#include "sensor_dp_lib.h"

//#define RC_36M /**< JXK351P BAYER CIS clock souce RC 36M*/
#define XTAL_24M /**< JXK351P BAYER CIS clock souce XTAL 24M*/

/**
 * \brief	set jxk351p bayer cis default settings
 *
 * \return	success (0) or error (-1)
 */
int jxk351p_bayer_cis_def_cfg(void);

/**
 * \brief	set jxk351p bayer cis streaming on
 *
 * \return	void
 */
void jxk351p_bayer_cis_streaming_on(void);

/**
 * \brief	set jxk351p bayer cis streaming off
 *
 * \return	void
 */
void jxk351p_bayer_cis_streaming_off(void);

/**
 * \brief	set jxk351p bayer cis init
 *
 * \param[in]	xshutdown_pin   xshutdown pin
 * \param[in]	mclk_div	    mclk division
 * \return	int
 */
int jxk351p_bayer_cis_init(CIS_XHSHUTDOWN_INDEX_E xshutdown_pin, SENSORCTRL_MCLK_E mclk_div);

/**
 * \brief	set jxk351p bayer cis get sensor id
 *
 * \return	void
 */
int jxk351p_bayer_cis_get_sensor_id(uint16_t *model_id, uint8_t *rev_id);
int jxk351p_bayer_cis_read_reg(uint16_t reg_addr, uint8_t *reg_val);
int jxk351p_bayer_cis_write_reg(uint16_t reg_addr, uint8_t reg_val);
#ifdef __cplusplus
}
#endif

#endif /*__JXK351P_BAYER_CIS_CFG_H__*/


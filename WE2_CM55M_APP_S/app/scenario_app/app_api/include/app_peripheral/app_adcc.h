
/**
 * @file app_adcc.h
 * @brief 
 * @version V1.0.0
 * @date 2022-10-26
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_ADCC_H__
#define __APP_ADCC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_adcc.h"

/**
 * \struct  APP_ADCC_CFG_T
 * \brief   ADCC configuration
 */
typedef struct APP_ADCC_CFG_S
{
    uint16_t analog_vol;
	uint16_t enable;
	uint16_t single_shot_enable;
	uint16_t channel_mask;
	uint16_t sample_rate[4];
	uint16_t data_type[4];
	uint16_t offset[4];
	uint16_t high_threshold[4];
	uint16_t low_threshold[4];
	uint16_t int_delay;
	uint32_t int_mask;
	uint16_t data_select[4];
} APP_ADCC_CFG_T;

/**
 * \struct  APP_ADCC_STATUS_T
 * \brief   ADCC status
 */
typedef struct APP_ADCC_STATUS_S
{
    uint32_t int_status;
    uint16_t ch_data;
} APP_ADCC_STATUS_T;

int8_t app_adcc_init(void);
int8_t app_adcc_deinit(void);
int8_t app_adcc_enable(uint8_t enable);
int8_t app_adcc_get_def_cfg(APP_ADCC_CFG_T *padcc_cfg);
int8_t app_adcc_set_cfg(APP_ADCC_CFG_T *padcc_cfg);
int8_t app_adcc_read_status(uint8_t ch, APP_ADCC_STATUS_T *padcc_status);
int8_t app_adcc_dump_reg(void);

#ifdef __cplusplus
}
#endif

#endif /*__APP_ADCC_H__*/

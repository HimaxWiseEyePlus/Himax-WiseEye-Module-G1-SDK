/**
 * @file app_pwm.h
 * @brief 
 * @version V1.0.0
 * @date 2022-11-04
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */
 
#ifndef __APP_PWM_H__
#define __APP_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_pwm.h"

typedef struct PWM_CTRL_S{
    uint8_t continus;
    uint8_t inverse;
    uint8_t duty;          /**< Range : 1~100 */
    uint32_t freq;          /**< Programmed Frequency 0.045Hz~1.5MHz */
    uint32_t count;
}PWM_CTRL_T;

int8_t app_pwm_init(void);
int8_t app_pwm_deinit(void);
int8_t app_pwm_start(uint8_t index, PWM_CTRL_T *pwm_ctrl);
int8_t app_pwm_stop(uint8_t index);
int8_t app_pwm_gate(uint8_t index, uint32_t gate);

#ifdef __cplusplus
}
#endif

#endif /*__APP_PWM_H__*/



#include "embARC.h"
#include "embARC_debug.h"

#include "hx_drv_ckgen.h"

#include "app_api.h"
#include "app_pwm.h"

//#define PMW0_ENABLE /*IOMUX_PGPIO0*/
//#define PMW1_ENABLE /*IOMUX_PGPIO8*/
#define PMW2_ENABLE /*IOMUX_PGPIO9*/

/***********************************************
 * GLOBAL/EXTERN VARIABLE DECLARATION
 **********************************************/
static uint32_t gIntclr = 1;

/***************************************************
 * Static Function Declaration
 **************************************************/
#if defined(PMW0_ENABLE)
static void pwm_0_cb(void *param)
{
    /** Disable DMA request **/
    dbg_printf(DBG_LESS_INFO, "pwm_0_cb\n");

    if (gIntclr)
        hx_drv_pwm_set_intclr(0);
}
#endif

#if defined(PMW1_ENABLE)
static void pwm_1_cb(void *param)
{
    /** Disable DMA request **/
    dbg_printf(DBG_LESS_INFO, "pwm_1_cb\n");
    if (gIntclr)
        hx_drv_pwm_set_intclr(1);
}
#endif

#if defined(PMW2_ENABLE)
static void pwm_2_cb(void *param)
{
    /** Disable DMA request **/
    dbg_printf(DBG_LESS_INFO, "pwm_2_cb\n");
    if (gIntclr)
        hx_drv_pwm_set_intclr(2);
}
#endif
int8_t app_pwm_init(void)
{
    PWM_ERROR_E ret;
    IOMUX_ERROR_E res;
    
    #if defined(PMW0_ENABLE)
    ret = hx_drv_pwm_register_cb(PWM0, pwm_0_cb);
    //dbg_printf(DBG_LESS_INFO, "ret = %d\n", ret);
    if(ret != PWM_NO_ERROR)
        return API_ERROR;
    #endif
    #if defined(PMW1_ENABLE)
    ret = hx_drv_pwm_register_cb(PWM1, pwm_1_cb);
    //dbg_printf(DBG_LESS_INFO, "ret = %d\n", ret);
    if(ret != PWM_NO_ERROR)
        return API_ERROR;
    #endif
    #if defined(PMW2_ENABLE)
    ret = hx_drv_pwm_register_cb(PWM2, pwm_2_cb);
    //dbg_printf(DBG_LESS_INFO, "ret = %d\n", ret);
    if(ret != PWM_NO_ERROR)
        return API_ERROR;
    #endif

    #if defined(PMW0_ENABLE)
    res = hx_drv_iomux_set_pmux(IOMUX_PGPIO0, 0x00);
    //dbg_printf(DBG_LESS_INFO, "res = %d\n", res);
    if(res != IOMUX_NO_ERROR)
        return API_ERROR;
    #endif
    #if defined(PMW1_ENABLE)
    res = hx_drv_iomux_set_pmux(IOMUX_PGPIO8, 0x00);
    //dbg_printf(DBG_LESS_INFO, "res = %d\n", res);
    if(res != IOMUX_NO_ERROR)
        return API_ERROR;
    #endif
    #if defined(PMW2_ENABLE)
    res = hx_drv_iomux_set_pmux(IOMUX_PGPIO9, 0x00);
    //dbg_printf(DBG_LESS_INFO, "res = %d\n", res);
    if(res != IOMUX_NO_ERROR)
        return API_ERROR;
    #endif

    ret = hx_drv_pwm_init();
    //dbg_printf(DBG_LESS_INFO, "pwm inited, ret = %d\n", ret);

    if(ret != PWM_NO_ERROR && ret != PWM_ERROR_ALREADY_INIT)
        return API_ERROR;
    
    return API_SUCC;
}

int8_t app_pwm_deinit(void)
{
    PWM_ERROR_E ret;

    ret = hx_drv_pwm_deinit();
    //dbg_printf(DBG_LESS_INFO, "pwm deinited, ret = %d\n", ret);

    if(ret != PWM_NO_ERROR)
        return API_ERROR;
    
    return API_SUCC;
}

int8_t app_pwm_start(uint8_t index, PWM_CTRL_T *ppwm_ctrl)
{
    PWM_ERROR_E ret;
    SYS_ERROR_E sys_ret;
    pwm_ctrl ctrl;
    SYS_INDEX_E sys_index;
    
    #if defined(PMW0_ENABLE)
    if(index == 0)
        sys_index = SYS_PWM0;
    else 
    #endif
    #if defined(PMW1_ENABLE)
    if(index == 1)
        sys_index = SYS_PWM1;
    else
    #endif
    #if defined(PMW2_ENABLE)
    if(index == 2)
        sys_index = SYS_PWM2;
    else
    #endif
        return API_ERROR;
    
    sys_ret = hx_drv_ckgen_set_gated(sys_index, 0);
    if(sys_ret != SYS_NO_ERROR)
        return API_ERROR;
    
    ctrl.continus = ppwm_ctrl->continus;
    ctrl.inverse = ppwm_ctrl->inverse;
    ctrl.freq = ppwm_ctrl->freq / 2 * 3; /*1.5 = 36M/24M*/
    ctrl.duty = ppwm_ctrl->duty;
    ctrl.count = ppwm_ctrl->count;
    
    ret = hx_drv_pwm_start((PWM_INDEX_E)index, &ctrl);
    
    if(ret != PWM_NO_ERROR)
        return API_ERROR;
    
    return API_SUCC;
}

int8_t app_pwm_stop(uint8_t index)
{
    PWM_ERROR_E ret;
    
    ret = hx_drv_pwm_stop((PWM_INDEX_E)index);
    
    if(ret != PWM_NO_ERROR)
        return API_ERROR;
    
    return API_SUCC;
}

int8_t app_pwm_gate(uint8_t index, uint32_t gate)
{
    SYS_ERROR_E ret;
    SYS_INDEX_E sys_index;
    
    #if defined(PMW0_ENABLE)
    if(index == 0)
        sys_index = SYS_PWM0;
    else 
    #endif
    #if defined(PMW1_ENABLE)
    if(index == 1)
        sys_index = SYS_PWM1;
    else
    #endif
    #if defined(PMW2_ENABLE)
    if(index == 2)
        sys_index = SYS_PWM2;
    else
    #endif
        return API_ERROR;
    
    ret = hx_drv_ckgen_set_gated(sys_index, gate);
    
    if(ret != PWM_NO_ERROR)
        return API_ERROR;
    
    return API_SUCC;
}



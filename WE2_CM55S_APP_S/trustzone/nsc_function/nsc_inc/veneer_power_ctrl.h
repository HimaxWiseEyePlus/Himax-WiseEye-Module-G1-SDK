/*
 * pmu_ctrl.h
 *
 *  Created on: 2021¦~11¤ë14¤é
 *      Author: 902447
 */

#ifndef TRUSTZONE_NSC_FUNCTION_PMU_CTRL_H_
#define TRUSTZONE_NSC_FUNCTION_PMU_CTRL_H_

#include "hx_drv_scu_export.h"
#include "powermode_export.h"

#ifdef CM55_BIG
/**
 * \brief	Configure the power manager library for specific power mode
 *
 * \param[in] PM_CFG_T		configuration of power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_get_defcfg_bymode(void *aCfg, PM_CFG_PWR_MODE_E mode);

/**
 * \brief	check PMU done capture frame done when PMU to ALLON.
 *
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_pmu_dpdone_check(sensordplib_waitpmudmadone_t cb_fun);

/**
 * \brief	hand over control from PMU to CPU. Then CPU can control everything
 *
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_ctrl_fromPMUtoCPU(sensordplib_pmutocpuctrl_t cb_fun);

/**
 * \brief	Configure the power manager library for specific power mode
 *
 * \param[in] PM_CFG_T		configuration of power mode
 * \param[in] cb_fun		DP initial before enter PMU only use when DP in NS side
 * \param[in] mode		power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_cfg_set(void *aCfg, sensordplib_pmudpinit_t cb_fun, PM_CFG_PWR_MODE_E mode);

/**
 * \brief	Get Current power mode in the power manager library
 *
 * \param[out] PM_CFG_T		configuration of power mode
 * \param[in] mode		power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_cfg_get(void *aCfg, PM_CFG_PWR_MODE_E mode);

/**
 * \brief	Get Current power mode in the power manager library
 *
 * \param[out] mode		configuration of power mode
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_mode_get(PM_CFG_PWR_MODE_E *mode);

/**
 * \brief	The function is used to trigger to enter PMU mode. It is handled by PMU library hx_lib_pm_mode_set so application doesn't need take care it.
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_trigger(SCU_PDHSC_HSCCLK_CFG_T *hsc_cfg, SCU_LSC_CLK_CFG_T *lsc_cfg, PM_CLK_PARA_CTRL_E clkparactrl);
#endif

/**
 * \brief	The function is used to get the PMU wake up event
 *
 * \param[out] aWakeupEvt	store the PMU wakeup event
 * \param[out] aWakeupEvt1	store the PMU wakeup event1
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_get_event(PMU_WAKEUPEVENT_E *aWakeupEvt, PMU_WAKEUPEVENT1_E *aWakeupEvt1);

/**
 * \brief	The function is used to clear the PMU wake up event flag
 *
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_clear_event(void);
/**
 * \brief	The function is used to Force PMU to go to power state
 *
 * \param[in] state	 force power state
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_force_state_set(PMU_PS_STATE_E state);

/**
 * \brief	The function is used to get power state
 *
 * \param[in] state	 get power state
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_force_state_get(PMU_PS_STATE_E *state);

/**
 * \brief	The function is used to set dma usage cfg
 *
 * \param[in] dma_int	 dma_int cfg
 * \retval	PM_NO_ERROR		success
 * \retval	others			fail
 */
PM_ERROR_E veneer_pm_set_dma_cfg(uint8_t dma_int);

#endif /* TRUSTZONE_NSC_FUNCTION_PMU_CTRL_H_ */

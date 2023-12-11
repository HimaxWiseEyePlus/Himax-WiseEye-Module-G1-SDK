/*
 * hx_drv_rtc.h
 *
 *  Created on: 2021�~5��2��
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_RTC_H_
#define DRIVERS_INC_HX_DRV_RTC_H_
#include <stdio.h>
#include "WE2_device.h"


/**
 * \enum RTC_ERROR_E
 * \brief RTC Errors Type
 */
typedef enum {
	RTC_NO_ERROR = 0, /**< No ERROR */
	RTC_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	RTC_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
} RTC_ERROR_E;

/**
 * \enum RTC_AIE_E
 * \brief Alarm Int enable on/off
 */
typedef enum {
	RTC_AIE_ON = 0, /**< Alarm Int. Enable On */
	RTC_AIE_OFF, /**< Alarm Int. enable OFF */
} RTC_AIE_E;

/**
 * \enum RTC_ID_E
 * \brief ID of RTC
 */
typedef enum {
	RTC_ID_0 = 0, /**< Timer ID 0 */
	RTC_ID_1, /**< Timer ID 1 */
	RTC_ID_2, /**< Timer ID 2 */
	RTC_ID_MAX, /**< Timer MAX */
} RTC_ID_E;

typedef struct {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
//	int tm_isdst;
}rtc_time;

/*
 * alarm API.
 */
typedef struct {
	unsigned char enabled;	/* 0 = alarm disabled, 1 = alarm enabled */
	unsigned char pending;  /* 0 = alarm not pending, 1 = alarm pending */
	rtc_time time;	/* time the alarm is set to */
}rtc_wkalrm;

/**
 * \brief	Alarm interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*RTC_ISREvent_t) (uint32_t event);

/**
 * \brief RTC init
 *
 * \param[in]	id	RTC ID
 * \param[in]	rtc_baseaddr	RTC HW base address
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_init(RTC_ID_E id, uint32_t rtc_baseaddr);

/**
 * \brief RTC get clock
 *
 * \param[in]	id	RTC ID
 * \param[out]	freq	RTC Frequency
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_clk(RTC_ID_E id, uint32_t *freq);

/**
 * \brief	Disable RTC
 *
 * \param[in]	id	RTC ID
 * \param[in]	enable	RTC enable
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_enable(RTC_ID_E id, uint32_t enable);

/**
 * \brief	read RTC Time
 *
 * \param[in]	id	RTC ID
 * \param[out]	rtc_time	read RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_read_time(RTC_ID_E id, rtc_time *tm);

/**
 * \brief	set RTC Time
 *
 * \param[in]	id	RTC ID
 * \param[in]	rtc_time	read RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_time(RTC_ID_E id, rtc_time *tm);

/**
 * \brief	read alarm Time
 *
 * \param[in]	id	RTC ID
 * \param[out]	rtc_time	read Alarm RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_read_alarm(RTC_ID_E id, rtc_wkalrm *alarm);

/**
 * \brief	set alarm Time
 *
 * \param[in]	id	RTC ID
 * \param[in]	rtc_time	read Alarm RTC time
 * \param[in]	cb Alarm RTC Callback
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_alarm(RTC_ID_E id, rtc_wkalrm *alarm, RTC_ISREvent_t cb);
/**
 * \brief	set alarm interrupt enable
 *
 * \param[in]	id	RTC ID
 * \param[in]	inten	interrupt enable
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_alarm_int(RTC_ID_E id, RTC_AIE_E inten);
/**
 * \brief	get alarm interrupt enable
 *
 * \param[in]	id	RTC ID
 * \param[out]	inten	interrupt enable
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_alarm_int(RTC_ID_E id, RTC_AIE_E *inten);
/**
 * \brief	clear alarm interrupt status clear
 *
 * \param[in]	id	RTC ID
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_clear_alarm_int_status(RTC_ID_E id);
/**
 * \brief	get alarm raw interrupt status
 *
 * \param[in]	id	RTC ID
 * \param[out]	status	interrupt status
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_alarm_rawint_status(RTC_ID_E id, uint32_t *status);

/**
 * \brief	get alarm mask interrupt status
 *
 * \param[in]	id	RTC ID
 * \param[out]	status	interrupt status
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_alarm_maskint_status(RTC_ID_E id, uint32_t *status);

/**
 * \brief	read rtc val
 *
 * \param[in]	id	RTC ID
 * \param[out]	val	read rtc value
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_read_val(RTC_ID_E id, uint32_t *val);
/**
 * \brief	set rtc Time by cycle
 *
 * \param[in]	id	RTC ID
 * \param[in]	cycle	rtc cycle
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_time_cycle(RTC_ID_E id, uint32_t cycle);
/**
 * \brief	get rtc Time by cycle
 *
 * \param[in]	id	RTC ID
 * \param[out]	cycle	rtc cycle
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_time_cycle(RTC_ID_E id, uint32_t *cycle);

/**
 * \brief	set rtc alarm Time by cycle
 *
 * \param[in]	id	RTC ID
 * \param[in]	cycle	rtc alarm cycle time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_set_alarm_cycle(RTC_ID_E id, uint8_t alarm_en, uint32_t cycle, RTC_ISREvent_t cb);

/**
 * \brief	get rtc alarm Time by cycle
 *
 * \param[in]	id	RTC ID
 * \param[out]	cycle	rtc alarm cycle time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_get_alarm_cycle(RTC_ID_E id, uint32_t *cycle);



/**
 * \brief	read CM55M RTC Time
 *
 * \param[out]	rtc_time	read RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55m_read_time(rtc_time *tm);

/**
 * \brief	set CM55M RTC Time
 *
 * \param[in]	rtc_time	read RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55m_set_time(rtc_time *tm);

/**
 * \brief	read CM55M alarm Time
 *
 * \param[out]	rtc_time	read Alarm RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55m_read_alarm(rtc_wkalrm *alarm);

/**
 * \brief	set CM55M alarm Time
 *
 * \param[in]	rtc_time	read Alarm RTC time
 * \param[in]	cb Alarm RTC Callback
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55m_set_alarm(rtc_wkalrm *alarm, RTC_ISREvent_t cb);

/**
 * \brief	read CM55S RTC Time
 *
 * \param[out]	rtc_time	read RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55s_read_time(rtc_time *tm);

/**
 * \brief	set CM55S RTC Time
 *
 * \param[in]	rtc_time	read RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55s_set_time(rtc_time *tm);

/**
 * \brief	read CM55S alarm Time
 *
 * \param[out]	rtc_time	read Alarm RTC time
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55s_read_alarm(rtc_wkalrm *alarm);

/**
 * \brief	set CM55S alarm Time
 *
 * \param[in]	rtc_time	read Alarm RTC time
 * \param[in]	cb Alarm RTC Callback
 * \return	RTC_ERROR_E.
 */
RTC_ERROR_E hx_drv_rtc_cm55s_set_alarm(rtc_wkalrm *alarm, RTC_ISREvent_t cb);


#endif /* DRIVERS_INC_HX_DRV_RTC_H_ */

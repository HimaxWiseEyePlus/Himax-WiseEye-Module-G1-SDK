
#ifndef _APP_PLATFORM_H_
#define _APP_PLATFORM_H_

#include "WE2_ARMCM55.h"

#ifdef TRUSTZONE_SEC
#ifdef FREERTOS
/* Trustzone config. */
//#include "partition_ARMCM55.h"
/* FreeRTOS includes. */
//#include "secure_port_macros.h"
#else /* FREERTOS */
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
//#include "veneer_table.h"
//#include "partition_ARMCM55.h"
#endif /* FREERTOS */
#endif /* TRUSTZONE_SEC */

#include "xprintf.h"
#include "board.h"

#include "WE2_core.h"
#include "WE2_device_addr.h"
#include "WE2_debug.h"


#endif

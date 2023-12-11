/**
 * @file common.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef _COMMON_H
#define _COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "stdlib.h"
//#include "arc_builtin.h"
//#include "hx_drv_iomux.h"
//#include "hx_drv_uart.h"
//#include "spi_slave_protocol.h"
//#include "uart_protocol.h"
//#include "board_config.h"
//#include "io_config.h"
//#include "spi_protocol.h"
//#include "peripheral_cmd_handler.h"
#include "board.h"
//#include "app_algo.h"

//#include "arc.h"
//#include "arc_timer.h"

/* Color Print */
#define CPNT_RED "\033[1;31m"   /**< print text with red color*/
#define CPNT_GRN "\033[1;32m"   /**< print text with green color*/
#define CPNT_BLU "\033[1;34m"   /**< print text with blue color*/
#define CPNT_END "\033[m"       /**< print text with color disabled*/

#ifdef __cplusplus
}
#endif

#endif

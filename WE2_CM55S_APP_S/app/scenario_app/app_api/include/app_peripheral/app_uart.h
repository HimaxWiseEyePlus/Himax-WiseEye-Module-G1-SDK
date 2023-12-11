
/**
 * @file app_uart.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_UART_H__
#define __APP_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_uart.h"

#define UART_0_ID    USE_DW_UART_0 /**< UART 0 ID macro : 0*/
#define UART_1_ID    USE_DW_UART_1 /**< UART 1 ID macro : 1*/
#define UART_2_ID    USE_DW_UART_2 /**< UART 2 ID macro : 2*/
#define UART_MAX_ID  USE_DW_UART_MAX /**< UART MAX ID macro : MAX*/

/**
 * \brief	open uart device
 *
 * \param[in]	uart_id	        uart device id
 * \param[in]	baudrate	    uart baud rate
 * \return	app api error code.
 */
int32_t app_uart_open(uint32_t uart_id, uint32_t baudrate);

/**
 * \brief	close uart device
 *
 * \param[in]	uart_id	        uart device id
 * \return	app api error code.
 */
int32_t app_uart_close(uint32_t uart_id);

/**
 * \brief	read uart data
 *
 * \param[in]	uart_id	        uart device id
 * \param[out]	*buf	        specified data buffer to receive
 * \param[in]	len	            specified data length to receive
 * \return	received uart data size or error (-1)
 */
int32_t app_uart_read(uint32_t uart_id, uint8_t *buf, uint32_t len);

/**
 * \brief	write uart data
 *
 * \param[in]	uart_id	        uart device id
 * \param[in]	*buf	        specified data buffer to transmit
 * \param[in]	len	            specified data length to transmit
 * \return	transmitted uart data size or error (-1)
 */
int32_t app_uart_write(uint32_t uart_id, uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /*__APP_UART_H__*/

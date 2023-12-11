/**
 * @file app_spi_s.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_SPI_S_H__
#define __APP_SPI_S_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "spi_protocol.h"

/**
 * \brief	SPI slave open
 *
 * \return	app api error code.
 */
int8_t app_spi_s_open();

/**
 * \brief	SPI slave close
 *
 * \return	app api error code.
 */
int8_t app_spi_s_close();

/**
 * \brief	SPI slave write data
 *
 * \param[in]	*tx_data	    specified data buffer to transmit to spi master device
 * \param[in]	tx_len	        specified data length to transmit to spi master device
 * \param[in]	data_type	    specified data type to transmit to spi master device
 * \return	app api error code.
 */
int8_t app_spi_s_write(uint8_t *tx_data, uint32_t tx_len, SPI_CMD_DATA_TYPE data_type);

/**
 * \brief	SPI slave write handshake
 *
 * \param[in]	gpio_num	        SPI slave handshake gpio number (none 0xFF)
 * \param[in]	gpio_level	        SPI slave handshake gpio active High(1)/Low(0)
 * \return	app api error code.
 */
int8_t app_spi_s_preset_write_handshake(uint8_t gpio_num, uint8_t gpio_atv_lvl);

/**
 * \brief	SPI slave read data back
 *
 * \param[out]	*rx_data	        specified data buffer to receive from spi master device
 * \param[in]	rx_len	            specified data length to receive from spi master device
 * \return	app api error code.
 */
int8_t app_spi_s_read(uint8_t *rx_data, uint32_t rx_len);

#ifdef __cplusplus
}
#endif

#endif /*__APP_SPI_S_H__*/

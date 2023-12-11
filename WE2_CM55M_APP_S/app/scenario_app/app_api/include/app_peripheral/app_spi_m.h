/**
 * @file app_spi_m.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_SPI_M_H__
#define __APP_SPI_M_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_spi.h"

/**
 * \enum SPI_MST_E
 * \brief spi master id
 */
typedef enum SPI_MST_S
{
    SPI_MST_0 = USE_DW_SPI_MST_Q, /**< SPI Master 0 ID macro : master 0*/
    SPI_MST_1 = USE_DW_SPI_MST_S, /**< SPI Master 1 ID macro : master 1*/
    SPI_MST_2 = USE_DW_SPI_MST_O, /**< SPI Master 2 ID macro : master 2*/
}SPI_MST_E;

/**
 * \brief	SPI master open
 *
 * \param[in]	spi_id	    spi master device id
 * \param[in]	freq	    spi master clock frequency
 * \return	app api error code.
 */
int8_t app_spi_m_open(SPI_MST_E spi_id, uint32_t freq);

/**
 * \brief	SPI master close
 *
 * \param[in]	spi_id	    spi master device id
 * \return	app api error code.
 */
int8_t app_spi_m_close(SPI_MST_E spi_id);

/**
 * \brief	SPI master write data
 *
 * \param[in]	spi_id	    spi master device id
 * \param[in]	*tx_data	specified data buffer to transmit to spi slave device
 * \param[in]	tx_len	    specified data length to transmit to spi slave device
 * \param[in]	data_type	specified data type to transmit to spi slave device
 * \return	app api error code.
 */
int8_t app_spi_m_write(SPI_MST_E spi_id, uint8_t *tx_data, uint32_t tx_len, SPI_CMD_DATA_TYPE data_type);

/**
 * \brief	SPI master read data back
 *
 * \param[in]	spi_id	    spi master device id
 * \param[out]	*rx_data	specified data buffer to reveive from spi slave device
 * \param[in]	rx_len	    specified data length to reveive from spi slave device
 * \return	app api error code.
 */
int8_t app_spi_m_read(SPI_MST_E spi_id, uint8_t *rx_data, uint32_t rx_len);

#ifdef __cplusplus
}
#endif

#endif /*__APP_SPI_M_H__*/

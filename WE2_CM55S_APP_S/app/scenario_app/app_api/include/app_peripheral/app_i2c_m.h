/**
 * @file app_i2c_m.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_I2C_M_H__
#define __APP_I2C_M_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_iic.h"

/**
 * \enum I2C_SPEED_MODE_E
 * \brief i2c clock speed mode
 */
typedef enum I2C_SPEED_MODE_S {
    I2C_SPEED_100K = DW_IIC_SPEED_STANDARD,        /**< Bidirectional, Standard-mode (Sm), with a bit rate up to 100 kbit/s */
    I2C_SPEED_400K = DW_IIC_SPEED_FAST,            /**< Bidirectional, Fast-mode (Fm), with a bit rate up to 400 kbit/s */
    I2C_SPEED_1M = DW_IIC_SPEED_FASTPLUS           /**< Bidirectional, Fast-mode Plus (Fm+), with a bit rate up to 1 Mbit/s */
} I2C_SPEED_MODE_E;

#define I2C_0_ID    USE_DW_IIC_0       /**< IIC 0 ID macro : master 0*/
#define I2C_1_ID    USE_DW_IIC_1       /**< IIC 1 ID macro : master 1*/

/**
 * \brief	I2C master initialization
 *
 * \param[in]	i2c_id	        i2c master device id
 * \param[in]	speed_mode      i2c master clock speed
 * \return	app api error code.
 */
int8_t app_i2c_mst_init(uint8_t i2c_id, I2C_SPEED_MODE_E speed_mode);

/**
 * \brief	I2C master deinitialization
 *
 * \param[in]	i2c_id	        i2c master device id
 * \return	app api error code.
 */
int8_t app_i2c_mst_deinit(uint8_t i2c_id);

/**
 * \brief	I2C master write data
 *
 * \param[in]	i2c_id	        i2c master device id
 * \param[in]	slav_addr	    i2c slave device address
 * \param[in]	*reg_addr	    i2c slave device register address
 * \param[in]	reg_addr_len	i2c slave device register address length
 * \param[in]	*tx_buf	        specified data buffer to transmit to i2c slave device
 * \param[in]	tx_len	        specified data length to transmit to i2c slave device
 * \return	app api error code.
 */
int8_t app_i2c_mst_write(uint8_t i2c_id, uint8_t slav_addr, uint8_t *reg_addr, uint32_t reg_addr_len, uint8_t *tx_buf, uint32_t tx_len);

/**
 * \brief	I2C master read data back
 *
 * \param[in]	i2c_id	        i2c master device id
 * \param[in]	slav_addr	    i2c slave device address
 * \param[in]	*reg_addr	    i2c slave device register address
 * \param[in]	reg_addr_len	i2c slave device register address length
 * \param[out]	*rx_buf	        specified data buffer to receive from i2c slave device
 * \param[out]	rx_len	        specified data length to receive from i2c slave device
 * \return	app api error code.
 */
int8_t app_i2c_mst_write_read(uint8_t i2c_id, uint8_t slav_addr, uint8_t *reg_addr, uint32_t reg_addr_len, uint8_t *rx_buf, uint32_t rx_len);

#ifdef __cplusplus
}
#endif

#endif /*__APP_I2C_M_H__*/
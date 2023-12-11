/**
 * @file ota_update.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */


#ifndef __OTA_UPDATE_H__
#define __OTA_UPDATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	i2c ota command process
 *
 * \return	i2c command error code
 */
I2CCOMM_ERROR_E i2ccomm_cmd_process_ota_flow(uint8_t* read_buf);

#ifdef __cplusplus
}
#endif

#endif


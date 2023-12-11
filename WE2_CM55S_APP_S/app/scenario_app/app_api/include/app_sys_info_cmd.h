/**
 * @file app_sys_info_cmd.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_SYS_INFO_CMD_H__
#define __APP_SYS_INFO_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

// Formal Version
/**
 * \struct INFO_FORMAL_VERSION
 * \brief formal version information
 */
typedef struct _INFO_FORMAL_VERSION
{
    uint32_t svn_version;       /**< svn version*/
    uint8_t customer_name[5];   /**< customer name*/
    uint32_t customer_version;  /**< customer version*/
} INFO_FORMAL_VERSION;

// TX Protocol
#define TX_DATA_META 0x1       /**< transmit meta data*/
#define TX_DATA_JPEG 0x2       /**< transmit jpeg data*/
#define TX_DATA_RAW 0x4        /**< transmit raw data*/
#define TX_DATA_PDM 0x8        /**< transmit pdm data*/
#define TX_DATA_VOICE 0x10     /**< transmit voice data*/

/**
 * \enum TX_IF_TYPE
 * \brief data transmit interface
 */
typedef enum _TX_IF_TYPE {
    TX_IF_UART = 1,     /**< enable transmit via uart*/
    TX_IF_SPI_M = 2,    /**< enable transmit via spi master*/
    TX_IF_SPI_S = 3     /**< enable transmit via spi slave*/
} TX_IF_TYPE;

/**
 * \struct INFO_TX_PROTOCOL
 * \brief transmit protocol information
 */
typedef struct _INFO_TX_PROTOCOL
{
    TX_IF_TYPE intf_type;   /**< enable data transmit interface*/
    uint32_t speed;         /**< enable data transmit speed*/
    uint16_t data_types;    /**< transmit data type*/
    uint8_t reserved[1];    /**< reserved data*/
} INFO_TX_PROTOCOL;

// RAW Setting
/**
 * \enum RAW_FMT
 * \brief raw data format
 */
typedef enum _RAW_FMT {
    RAW_YUV400 = 1, /**< raw data format yuv400*/
    RAW_YUV420 = 2, /**< raw data format yuv420*/
    RAW_YUV422 = 3  /**< raw data format yuv422*/
} RAW_FMT;

/**
 * \struct INFO_RAW_SETTING
 * \brief raw data setting information
 */
typedef struct _INFO_RAW_SETTING
{
    RAW_FMT format;         /**< raw data format*/
    uint16_t width;         /**< raw data width*/
    uint16_t height;        /**< raw data height*/
    uint8_t reserved[3];    /**< reserved data*/
} INFO_RAW_SETTING;

/**
 * \brief	system information command process
 *
 * \param[in]	*cmd_buf            received command buffer
 * \param[in]	*i2c_wbuf           i2c write buffer
 * \return  success(0)/fail(-1).
 */
int32_t app_sys_info_cmd_handler(uint8_t *cmd_buf, uint8_t *i2c_wbuf);

/* The following APIs should be implemented in each application for app_sys_info_cmd_handler(). */
/**
 * \brief	get formal version information
 *
 * \param[out]	*formal_version     formal version information
 * \return  success(0).
 */
int8_t app_sys_info_get_formal_version(INFO_FORMAL_VERSION *formal_version);

/**
 * \brief	get transmit protocol information
 *
 * \param[out]	*tx_protocol        transmit protocol information
 * \return  success(0).
 */
int8_t app_sys_info_get_tx_protocol(INFO_TX_PROTOCOL *tx_protocol);

/**
 * \brief	get raw data settings
 *
 * \param[out]	*raw_setting        raw data settings information
 * \return  success(0).
 */
int8_t app_sys_info_get_raw_setting(INFO_RAW_SETTING *raw_setting);

#ifdef __cplusplus
}
#endif

#endif /* __APP_BOARD_TEST_H__ */

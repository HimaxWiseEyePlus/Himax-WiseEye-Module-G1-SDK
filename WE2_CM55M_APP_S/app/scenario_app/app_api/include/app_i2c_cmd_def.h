/**
 * @file app_i2c_cmd_def.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP__I2C_CMD_DEF_H__
#define __APP__I2C_CMD_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum I2CCOMM_FEATURE_E
 * \brief this enumeration use in i2c communication library, define the supported feature.
 */
typedef enum
{
    I2CCOMM_FEATURE_SYS                 = 0x00, /**< SUPPORT FEATURE: system information*/
    I2CCOMM_FEATURE_REG_ACCES           = 0x10, /**< SUPPORT FEATURE: WE-I Register read/write*/
    I2CCOMM_FEATURE_GPIO_CTRL           = 0x11,
    I2CCOMM_FEATURE_SENSOR_REG_ACCESS   = 0x20,
    I2CCOMM_FEATURE_QUANTA_ISH          = 0x30, /**< SUPPORT FEATURE: Quanta NB Product - ISH communication*/
    I2CCOMM_FEATURE_SPI                 = 0x40,
    I2CCOMM_FEATURE_OTA_RESERVED        = 0x50,
    I2CCOMM_FEATURE_OTA_ALL             = 0x51,
    I2CCOMM_FEATURE_OTA_MEM_DSP         = 0x52,
    I2CCOMM_FEATURE_OTA_APP             = 0x53,
    I2CCOMM_FEATURE_OTA_APP_CONFIG      = 0x54,
    I2CCOMM_FEATURE_OTA_EMZA_CONFIG     = 0x55,
    I2CCOMM_FEATURE_OTA_CNNLUT          = 0x56,
    I2CCOMM_FEATURE_AUDIO_TEST          = 0x57,
    I2CCOMM_FEATURE_CSTM_SECURE_DGB     = 0x70,

    I2CCOMM_FEATURE_CUSTOMER_MIN        = 0x80,
    I2CCOMM_FEATURE_CUSTOMER_MAX        = 0x8F,
    I2CCOMM_FEATURE_MAX
} I2CCOMM_FEATURE_E;

/**
 * \enum I2CCOMM_OTA_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported OTA command.
 */
typedef enum
{
    I2CCOMM_CMD_OTA_GET_VER         = 0x01,
    I2CCOMM_CMD_OTA_GET_PROJID      = 0x02,
    I2CCOMM_CMD_OTA_SEND_CHIP_ID    = 0x03,
    I2CCOMM_CMD_OTA_SENT_RESULT     = 0x04, 
    I2CCOMM_CMD_OTA_REBOOT          = 0x05,
    I2CCOMM_CMD_OTA_FLASH_DUMP      = 0x06,
    I2CCOMM_CMD_OTA_FLASH_ERASE_ALL = 0x07,
    I2CCOMM_CMD_OTA_JUMP2UPG        = 0x08,
    I2CCOMM_CMD_OTA_START           = 0x09,
    I2CCOMM_CMD_OTA_DATA            = 0x0A,
    I2CCOMM_CMD_OTA_END             = 0x0B,
    I2CCOMM_CMD_OTA_MAX
} I2CCOMM_OTA_CMD_E;

typedef enum
{
    I2C_CMD_CSTM_FEATURE_0 = 0x80,
    I2C_CMD_CSTM_FEATURE_1 = 0x81,
    I2C_CMD_CSTM_FEATURE_2 = 0x82,
    I2C_CMD_CSTM_FEATURE_3 = 0x83
} I2C_CMD_CSTM_FEATURE;

#ifdef __cplusplus
}
#endif

#endif


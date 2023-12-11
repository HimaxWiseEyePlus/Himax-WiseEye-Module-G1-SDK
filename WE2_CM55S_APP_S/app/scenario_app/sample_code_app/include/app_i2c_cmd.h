#ifndef __APP_I2C_CMD_H__
#define __APP_I2C_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_iic.h"
#include "app_i2c_cmd_def.h"

#define I2CS_0_SLV_ADDR         0x62
#define I2CS_1_SLV_ADDR         0x64

/***************************************************
 * Function Declaration
 **************************************************/
typedef void (*funcptr_void)(void);
typedef void (*I2C_CMD_CSTM_CB) (uint8_t cmd_id, uint8_t *cmd_payload, uint16_t cmd_payload_len);

#ifdef OS_FREERTOS
int32_t i2ccomm_task_init(USE_DW_IIC_SLV_E iic_id);
#else
int32_t app_i2c_cmd_init(USE_DW_IIC_SLV_E iic_id);
void app_i2c_cmd_polling(USE_DW_IIC_SLV_E iic_id);
#endif

int8_t app_i2c_cmd_write(USE_DW_IIC_SLV_E iic_id, uint8_t feature, uint8_t cmd, uint8_t *data, uint16_t data_len);
void app_i2c_cmd_reg_cstm_feature(I2C_CMD_CSTM_FEATURE feature_id, I2C_CMD_CSTM_CB feature_callback);


#ifdef __cplusplus
}
#endif

#endif /*__APP_I2C_CMD_H__*/
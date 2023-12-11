#ifndef __BYPASS_SENSOR_REG_H__
#define __BYPASS_SENSOR_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_comm.h"

//extern unsigned char gRead_buf[I2CCOMM_MAX_RBUF_SIZE];
//extern unsigned char gWrite_buf[I2CCOMM_MAX_RBUF_SIZE];

void i2ccomm_cmd_process_sensor_reg_rw(void);

#ifdef __cplusplus
}
#endif

#endif /* __BYPASS_SENSOR_REG_H__ */
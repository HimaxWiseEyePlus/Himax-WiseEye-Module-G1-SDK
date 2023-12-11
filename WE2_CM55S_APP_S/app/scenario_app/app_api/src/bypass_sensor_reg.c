#include "common.h"
#include "hx_drv_CIS_common.h"
#include "hx_drv_pmu.h"
#include "sensor_dp_lib.h"
//#include "bypass_sensor_reg.h"

/****************************************************
 * Constant Definition                              *
 ***************************************************/
#define DATA_SFT_OFFSET_0       0
#define DATA_SFT_OFFSET_8       8
#define DATA_SFT_OFFSET_16      16
#define DATA_SFT_OFFSET_24      24

// Command process for FEATURE:I2CCOMM_SENSOR_REG_ACCESS
void i2ccomm_cmd_process_sensor_reg_rw(void)
{
#if 0
    int retval = 0;
    unsigned char cmd;
    unsigned short addr;
    unsigned char val = 0x5A;
    unsigned short checksum;

    retval = hx_lib_i2ccomm_validate_checksum(gRead_buf);
    if (retval != I2CCOMM_NO_ERROR)
    {
        dbg_printf(DBG_LESS_INFO, "[Warning] i2c cmd - sensor reg r/w - checksum validation : FAIL\n");
        return;
    }

    cmd  =  gRead_buf[I2CFMT_COMMAND_OFFSET];
    addr = (gRead_buf[I2CFMT_PAYLOAD_OFFSET] << DATA_SFT_OFFSET_8) | (gRead_buf[I2CFMT_PAYLOAD_OFFSET + 1]);
    
    hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_NONAOS);
    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
    hx_drv_sensorctrl_set_xSleep(1);

    switch (cmd)
    {
        case I2CCOMM_CMD_SENSOR_REG_SET:
            val =  gRead_buf[I2CFMT_PAYLOAD_OFFSET + 2];
            dbg_printf(DBG_LESS_INFO, "[MSG] process_sensor_reg_set(0x%04x, 0x%02x) \n", addr, val);
            hx_drv_cis_set_reg(addr, val, 0);
            break;
        case I2CCOMM_CMD_SENSOR_REG_GET:
            hx_drv_cis_get_reg(addr, &val);
            dbg_printf(DBG_LESS_INFO, "[MSG] process_sensor_reg_get(0x%04x, 0x%02x) \n", addr, val);

            // prepare write buffer for write process
            gWrite_buf[I2CFMT_FEATURE_OFFSET] = gRead_buf[I2CFMT_FEATURE_OFFSET];
            gWrite_buf[I2CFMT_COMMAND_OFFSET] = I2CCOMM_CMD_SENSOR_REG_GET;
            gWrite_buf[I2CFMT_PAYLOADLEN_LSB_OFFSET] =  I2CCOMM_SENSOR_CMD_PAYLOAD_GET_VAL & 0xFF;
            gWrite_buf[I2CFMT_PAYLOADLEN_MSB_OFFSET] = (I2CCOMM_SENSOR_CMD_PAYLOAD_GET_VAL >> 8) & 0xFF;
            gWrite_buf[I2CFMT_PAYLOAD_OFFSET] = val;
            hx_lib_i2ccomm_generate_checksum(gWrite_buf, I2CCOMM_HEADER_SIZE + I2CCOMM_SENSOR_CMD_PAYLOAD_GET_VAL, &checksum);

            // dbg_printf(DBG_LESS_INFO, "[MSG] checksum: 0x%04x\n", checksum);
            if (retval == I2CCOMM_NO_ERROR)
            {
                gWrite_buf[I2CFMT_PAYLOAD_OFFSET + 1] = checksum & 0xFF;
                gWrite_buf[I2CFMT_PAYLOAD_OFFSET + 2] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;
            }
            else
            {
                dbg_printf(DBG_LESS_INFO, "[Warning] i2c cmd - checksum generation : FAIL\n");
                gWrite_buf[I2CFMT_PAYLOAD_OFFSET + 1] = 0xFF;
                gWrite_buf[I2CFMT_PAYLOAD_OFFSET + 2] = 0xFF;
            }

            // enable write process for i2c master read command
            hx_lib_i2ccomm_enable_write(gWrite_buf);
            break;
        default:
            break;
    }

    sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
#endif    
}

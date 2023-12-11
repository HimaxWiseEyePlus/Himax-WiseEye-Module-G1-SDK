//#include "hx_hal_sec.h"
#include "hx_drv_iic.h"

#include "i2c_comm.h"

#include "WE2_debug.h"

#include "app_api.h"
#include "app_sys_info_cmd.h"


/* Prototypes */
static int8_t priv_cmd_i2c_write(uint8_t feature, uint8_t cmd, uint8_t *data, uint16_t data_len, uint8_t *wbuf);

/* Entities */
int32_t app_sys_info_cmd_handler(uint8_t *cmd_buf, uint8_t *i2c_wbuf)
{
    int8_t retval = 0;
    uint8_t cmd_id = 0;
    uint16_t checksum = 0, rx_checksum;
    uint16_t payload_len = 0;
    #if 0
    if (I2CCOMM_FEATURE_SYS != cmd_buf[I2CFMT_FEATURE_OFFSET]) {
        dbg_printf(DBG_LESS_INFO, "app_sys_info_cmd_handler: cmd feature type mismatch. (value = %02x)\n", cmd_buf[I2CFMT_FEATURE_OFFSET]);
        return -1;
    }
    #endif
    /* Get command ID */
    cmd_id = cmd_buf[I2CFMT_COMMAND_OFFSET];
    payload_len = cmd_buf[I2CFMT_PAYLOADLEN_LSB_OFFSET]|(cmd_buf[I2CFMT_PAYLOADLEN_MSB_OFFSET]<<8);
    dbg_printf(DBG_LESS_INFO, "cmd_id = %d, payload_len = %d\n", cmd_id, payload_len);
    /* Get CRC and check CRC */
    rx_checksum = cmd_buf[I2CCOMM_HEADER_SIZE + payload_len] | (cmd_buf[I2CCOMM_HEADER_SIZE + payload_len + 1]<<8);
    hx_lib_i2ccomm_generate_checksum(cmd_buf, I2CCOMM_HEADER_SIZE + payload_len, &checksum);
    if (rx_checksum != checksum) {
        dbg_printf(DBG_LESS_INFO, "app_sys_info_cmd_handler: Check Sum FAILED! (got 0x%04x but the result is 0x%04x\n", rx_checksum, checksum);
        return -1;
    }
    #if 0
    switch (cmd_id)
    {
        case I2CCOMM_CMD_SYS_GET_VER_FORMAL:
            {
                INFO_FORMAL_VERSION version;
                uint8_t payload[32] = {0};

                dbg_printf(DBG_LESS_INFO, "[MSG] app_fw_info_cmd_handler(): I2CCOMM_CMD_SYS_GET_TX_PROTOCOL\n");
                memset(&version, 0, sizeof(version));
                app_sys_info_get_formal_version(&version);
                memcpy(&payload[0], &version.svn_version, 4);
                memcpy(&payload[4], &version.customer_name[0], 5);
                memcpy(&payload[9], &version.customer_version, 4);
                retval = priv_cmd_i2c_write(I2CCOMM_FEATURE_SYS, I2CCOMM_CMD_SYS_GET_VER_FORMAL, payload, sizeof(payload), i2c_wbuf);
            }
            break;

        case I2CCOMM_CMD_SYS_GET_TX_PROTOCOL:
            {
                INFO_TX_PROTOCOL tx_protocol;
                uint8_t payload[8] = {0};

                dbg_printf(DBG_LESS_INFO, "[MSG] app_fw_info_cmd_handler(): I2CCOMM_CMD_SYS_GET_TX_PROTOCOL\n");
                app_sys_info_get_tx_protocol(&tx_protocol);
                payload[0] = tx_protocol.intf_type; //TX_IF: uart or SPI_M or SPI_S.
                payload[1] = tx_protocol.speed & 0xff; //TX_IF_SPEED: uint32_t of SPI M speed or UART baud rate. (little endian)
                payload[2] = (tx_protocol.speed>>8)& 0xff;
                payload[3] = (tx_protocol.speed>>16)& 0xff;
                payload[4] = (tx_protocol.speed>>24)& 0xff;
                payload[5] = tx_protocol.data_types; //TX_TYPES: uint16_t of send data types. Each bit represented 1 type. (METADATA|JPG|RAW|PDM, ...)
                payload[6] = (tx_protocol.data_types>>8) & 0xff;
                retval = priv_cmd_i2c_write(I2CCOMM_FEATURE_SYS, I2CCOMM_CMD_SYS_GET_TX_PROTOCOL, payload, sizeof(payload), i2c_wbuf);
            }
            break;

        case I2CCOMM_CMD_SYS_GET_TX_RAW_SETTING:
            {
                INFO_RAW_SETTING raw_setting;
                uint8_t payload[8] = {0};

                dbg_printf(DBG_LESS_INFO, "[MSG] app_fw_info_cmd_handler(): I2CCOMM_CMD_SYS_GET_TX_RAW_SETTING\n");
                app_sys_info_get_raw_setting(&raw_setting);

                payload[0] = raw_setting.format & 0xff; //RAW_FMT: YUV400, YUV420, YUV422, ...
                payload[1] = raw_setting.width & 0xff; //IMG_WIDTH: uint16_t image width. (little endian)
                payload[2] = (raw_setting.width>>8)& 0xff;
                payload[3] = raw_setting.height & 0xff; //IMG_HEIGHT: uint16_t image height. (little endian)
                payload[4] = (raw_setting.height>>8)& 0xff;
                retval = priv_cmd_i2c_write(I2CCOMM_FEATURE_SYS, I2CCOMM_CMD_SYS_GET_TX_RAW_SETTING, payload, sizeof(payload), i2c_wbuf);
            }
            break;
         
        default:
            dbg_printf(DBG_LESS_INFO, "app_fw_info_cmd_handler(): unknown board test type. (value = %02x)\n", cmd_id);
            return -1;
    }
    #endif
    return 0;
}

static int8_t priv_cmd_i2c_write(uint8_t feature, uint8_t cmd, uint8_t *data, uint16_t data_len, uint8_t *wbuf)
{
    I2CCOMM_ERROR_E i2c_comm_status;
    uint16_t checksum = 0;

    dbg_printf(DBG_LESS_INFO, "cmd = %d, data_len = %d\n", cmd, data_len);

    wbuf[I2CFMT_FEATURE_OFFSET] = feature;
    wbuf[I2CFMT_COMMAND_OFFSET] = cmd;
    wbuf[I2CFMT_PAYLOADLEN_LSB_OFFSET] =  data_len & 0xFF;
    wbuf[I2CFMT_PAYLOADLEN_MSB_OFFSET] = (data_len >> 8) & 0xFF;
    memcpy(&wbuf[I2CFMT_PAYLOAD_OFFSET], (data), data_len);

    hx_lib_i2ccomm_generate_checksum(wbuf, I2CCOMM_HEADER_SIZE + data_len, &checksum);

    wbuf[I2CCOMM_HEADER_SIZE + data_len] = checksum & 0xFF;
    wbuf[I2CCOMM_HEADER_SIZE + data_len + 1] = (checksum >> 8) & 0xFF;

#if 0
    dbg_printf(DBG_LESS_INFO, "priv_cmd_i2c_write: checksum = 0x%04x\n", checksum);
    for (int i = 0; i < 4+data_len+2; i++) {
        dbg_printf(DBG_LESS_INFO, "wbuf[%d] = 0x%02x\n", i, wbuf[i]);
    }
#endif

    // enable write process for i2c master read command
    i2c_comm_status = hx_lib_i2ccomm_enable_write(USE_DW_IIC_SLV_0, wbuf);
    if (I2CCOMM_NO_ERROR == i2c_comm_status)
        return 0;
    else
        return -1;
}



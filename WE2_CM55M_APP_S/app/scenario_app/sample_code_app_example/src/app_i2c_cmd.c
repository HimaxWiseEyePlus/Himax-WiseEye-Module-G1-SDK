#include "WE2_debug.h"
#include "i2c_comm.h"
#include "dw_iic.h"

//#include "app_api.h"
#include "app_i2c_cmd_def.h"
#include "app_i2c_cmd.h"

static uint8_t gWrite_buf[DW_IIC_S_NUM][I2CCOMM_MAX_WBUF_SIZE] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
static uint8_t gRead_buf[DW_IIC_S_NUM][I2CCOMM_MAX_RBUF_SIZE] __ALIGNED(__SCB_DCACHE_LINE_SIZE);

static uint8_t i2c_rx_ready[DW_IIC_S_NUM] = {false, false};
static uint8_t g_i2cs_en_write[DW_IIC_S_NUM] = {false, false};
static I2C_CMD_CSTM_CB cstm_feature_cb[4] = {0};

void set_i2cs_enable_write(USE_DW_IIC_SLV_E iic_id, bool status)
{
    g_i2cs_en_write[iic_id] = status;
}

bool get_i2cs_enable_write(USE_DW_IIC_SLV_E iic_id)
{
    return g_i2cs_en_write[iic_id];
}

static void prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_E iic_id)
{
    memset((void *)&gRead_buf[iic_id], 0xFF, 4);
    hx_CleanDCache_by_Addr((uint32_t) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
}

static void i2ccomm_0_callback_fun_tx(void *param)
{    
    dbg_printf(DBG_LESS_INFO, "[%s]\n", __FUNCTION__);
    
    set_i2cs_enable_write(USE_DW_IIC_SLV_0, false);
    
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_0, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);
    
    return;
}

static void i2ccomm_0_callback_fun_rx(void *param)
{
    dbg_printf(DBG_LESS_INFO, "[%s]\n", __FUNCTION__);
    
    #ifdef OS_FREERTOS
    BaseType_t xYieldRequired;
    
    xYieldRequired = xTaskResumeFromISR(i2ccomm_task_handle);

    if( xYieldRequired == pdTRUE )
    {
        portYIELD_FROM_ISR();
    }
    #else
    i2c_rx_ready[USE_DW_IIC_SLV_0] = true;
    #endif
    
    return;
}

static void i2ccomm_0_callback_fun_err(void *param)
{
    dbg_printf(DBG_LESS_INFO, "[%s]\n", __FUNCTION__);
    
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_0, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);

    return;
}

static void i2ccomm_1_callback_fun_tx(void *param)
{    
    dbg_printf(DBG_LESS_INFO, "[%s]\n", __FUNCTION__);
    
    set_i2cs_enable_write(USE_DW_IIC_SLV_1, false);
    
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_1);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_1, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_1], I2CCOMM_MAX_RBUF_SIZE);
    
    return;
}

static void i2ccomm_1_callback_fun_rx(void *param)
{
    dbg_printf(DBG_LESS_INFO, "[%s]\n", __FUNCTION__);
    
    #ifdef OS_FREERTOS
    BaseType_t xYieldRequired;
    
    xYieldRequired = xTaskResumeFromISR(i2ccomm_task_handle);

    if( xYieldRequired == pdTRUE )
    {
        portYIELD_FROM_ISR();
    }
    #else
    i2c_rx_ready[USE_DW_IIC_SLV_1] = true;
    #endif
    
    return;
}

static void i2ccomm_1_callback_fun_err(void *param)
{
    dbg_printf(DBG_LESS_INFO, "[%s]\n", __FUNCTION__);
    
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_1);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_1, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_1], I2CCOMM_MAX_RBUF_SIZE);

    return;
}

I2CCOMM_CFG_T gI2CCOMM_cfg[DW_IIC_S_NUM] = {
    {   
        I2CS_0_SLV_ADDR,
        i2ccomm_0_callback_fun_tx,
        i2ccomm_0_callback_fun_rx,
        i2ccomm_0_callback_fun_err},
    {   
        I2CS_1_SLV_ADDR,
        i2ccomm_1_callback_fun_tx,
        i2ccomm_1_callback_fun_rx,
        i2ccomm_1_callback_fun_err},
};

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

TaskHandle_t i2ccomm_task_handle = NULL;

static void i2ccomm_task(void *par)
{
    unsigned char feature_type = I2CCOMM_FEATURE_MAX;
    uint8_t cmd_id = 0;
    uint16_t payload_len = 0;
    
    uint32_t device_id = USE_DW_IIC_SLV_0;
    
    if((uint32_t *)par == (uint32_t *)&gI2CCOMM_cfg[USE_DW_IIC_SLV_0])
        device_id = USE_DW_IIC_SLV_0;
    else if((uint32_t *)par == (uint32_t *)&gI2CCOMM_cfg[USE_DW_IIC_SLV_1])
        device_id = USE_DW_IIC_SLV_1;
    
    while (1)
    {
        vTaskSuspend(NULL); /*!< suspend i2ccomm_task */
        
        #if 1
        dbg_printf(DBG_LESS_INFO,"app_i2c_loop> i2c_rx_ready\n");
        for (uint32_t i = 0; i < 10; i++) {
            dbg_printf(DBG_LESS_INFO,"gRead_buf[%d] = 0x%02x\n", i, gRead_buf[device_id][i]);
        }
        #endif
        
        feature_type = gRead_buf[device_id][I2CFMT_FEATURE_OFFSET];
        cmd_id = gRead_buf[device_id][I2CFMT_COMMAND_OFFSET];
        payload_len = gRead_buf[device_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] | (gRead_buf[device_id][I2CFMT_PAYLOADLEN_MSB_OFFSET]<<8);

        dbg_printf(DBG_LESS_INFO, "[MSG] feature_type: 0x%x \n", feature_type);
        dbg_printf(DBG_LESS_INFO, "[MSG] cmd_id: 0x%x \n", cmd_id);
        dbg_printf(DBG_LESS_INFO, "[MSG] payload_len: 0x%x \n", payload_len);

        switch(feature_type)//Any modify need sync with app_i2c_cmd_polling
        {
            case I2CCOMM_FEATURE_OTA_RESERVED:
                if(cmd_id == I2CCOMM_CMD_OTA_JUMP2UPG)
                {
                    i2ccomm_cmd_process_ota_flow((unsigned char *)&gRead_buf[device_id]);
                }
                else if(cmd_id == I2CCOMM_CMD_OTA_SEND_CHIP_ID)
                {
                    uint32_t chipid = 0x57454949;

                    //hx_drv_scu_get_chipid(&chipid);
                    //dbg_printf(DBG_LESS_INFO, "chipid: 0x%08x \n", chipid);
                    
                    app_i2c_cmd_write(device_id, feature_type, cmd_id, &chipid, sizeof(chipid));
                }
                break;
            case I2CCOMM_FEATURE_SYS:
                //app_sys_info_cmd_handler((unsigned char *)&gRead_buf[device_id], (unsigned char *)&gWrite_buf[device_id]);
                break;
            case I2CCOMM_FEATURE_SENSOR_REG_ACCESS:
                //board_delay_ms(200);//delay some times for sensor to get/set the register.
                //i2ccomm_cmd_process_sensor_reg_rw();
                //board_delay_ms(200);//delay some times for sensor to get/set the register.
                break;
			case I2C_CMD_CSTM_FEATURE_0:
                #ifndef BOARD_TEST
				if (cstm_feature_cb[0])
                    cstm_feature_cb[0](cmd_id, &gRead_buf[device_id][I2CFMT_PAYLOAD_OFFSET], payload_len);
                else
                    dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                #else
                app_board_test_cmd_handler(gRead_buf[device_id], gWrite_buf[device_id]);
                #endif
                break;
            case I2C_CMD_CSTM_FEATURE_1:
                if (cstm_feature_cb[1])
                    cstm_feature_cb[1](cmd_id, &gRead_buf[device_id][I2CFMT_PAYLOAD_OFFSET], payload_len);
                else
                    dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
            case I2C_CMD_CSTM_FEATURE_2:
                if (cstm_feature_cb[2])
                    cstm_feature_cb[2](cmd_id, &gRead_buf[device_id][I2CFMT_PAYLOAD_OFFSET], payload_len);
                else
                    dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
            case I2C_CMD_CSTM_FEATURE_3:
                if (cstm_feature_cb[3])
                    cstm_feature_cb[3](cmd_id, &gRead_buf[device_id][I2CFMT_PAYLOAD_OFFSET], payload_len);
                else
                    dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
            default:
                dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
        }

        if (get_i2cs_enable_write(device_id) == false)
        {
            xprintf("no write trigger, enable read\n" );
            prv_evt_i2ccomm_clear_read_buf_header(device_id);
            hx_lib_i2ccomm_enable_read(device_id, (unsigned char *) &gRead_buf[device_id], I2CCOMM_MAX_RBUF_SIZE);
        }
    }

    vTaskDelete(i2ccomm_task_handle);
}

int32_t i2ccomm_task_init(USE_DW_IIC_SLV_E iic_id)
{
	I2CCOMM_STATUS_E status = I2CCOMM_STATUS_UNKNOWN;
    
    dbg_printf(DBG_LESS_INFO, "i2ccomm_task_init...\n");
    
    if(I2CCOMM_NO_ERROR != hx_lib_i2ccomm_init(iic_id, gI2CCOMM_cfg[iic_id]))
	{
		dbg_printf(DBG_LESS_INFO, "\t\033[33m[%s]\033[0m, error, \033[33mline = %d\033[0m\n", __FUNCTION__, __LINE__);
		return -1;
	}

	hx_lib_i2ccomm_status(iic_id, &status);
	if (status == I2CCOMM_STATUS_OPEN)
	{
	    dbg_printf(DBG_LESS_INFO,"I2C is already Open.\n");
		return -1;
	}

    // clear read buf header
    prv_evt_i2ccomm_clear_read_buf_header(iic_id);
    
	// start i2c communication process
	hx_lib_i2ccomm_start(iic_id, (unsigned char *)&gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);

    if (xTaskCreate(i2ccomm_task, "i2ccomm_task", 1024, (void *)&gI2CCOMM_cfg[iic_id], TSK_PRIOR_HI + 2, &i2ccomm_task_handle) != pdPASS) {
        dbg_printf(DBG_LESS_INFO,"create i2ccomm_task error\r\n");
        return -1;
    }

    return 0;  
}

#else //OS_FREERTOS
int32_t app_i2c_cmd_init(USE_DW_IIC_SLV_E iic_id)
{
	I2CCOMM_STATUS_E status = I2CCOMM_STATUS_UNKNOWN;
    
    if(I2CCOMM_NO_ERROR != hx_lib_i2ccomm_init(iic_id, gI2CCOMM_cfg[iic_id]))
	{
		dbg_printf(DBG_LESS_INFO, "\t\033[33m[%s]\033[0m, error, \033[33mline = %d\033[0m\n", __FUNCTION__, __LINE__);
		return -1;
	}

	hx_lib_i2ccomm_status(iic_id, &status);
	if (status == I2CCOMM_STATUS_OPEN)
	{
	    dbg_printf(DBG_LESS_INFO,"I2C is already Open.\n");
		return -1;
	}

    // clear read buf header
    prv_evt_i2ccomm_clear_read_buf_header(iic_id);
    
	// start i2c communication process
	hx_lib_i2ccomm_start(iic_id, (unsigned char *)&gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);

    return 0;  
}

void app_i2c_cmd_polling(USE_DW_IIC_SLV_E device_id)
{
    uint8_t feature_type = 0, cmd_id = 0;
    uint16_t payload_len = 0;
        
    if (i2c_rx_ready[device_id])
    {
        feature_type = gRead_buf[device_id][I2CFMT_FEATURE_OFFSET];
        cmd_id = gRead_buf[device_id][I2CFMT_COMMAND_OFFSET];
        payload_len = gRead_buf[device_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] | (gRead_buf[device_id][I2CFMT_PAYLOADLEN_MSB_OFFSET]<<8);

        dbg_printf(DBG_LESS_INFO, "[MSG] feature_type: 0x%x \n", feature_type);
        dbg_printf(DBG_LESS_INFO, "[MSG] cmd_id: 0x%x \n", cmd_id);
        dbg_printf(DBG_LESS_INFO, "[MSG] payload_len: 0x%x \n", payload_len);

        switch (feature_type)//Any modify need sync with i2ccomm_task
        {
            case I2CCOMM_FEATURE_OTA_RESERVED:
                if(cmd_id == I2CCOMM_CMD_OTA_JUMP2UPG)
                {
                    i2ccomm_cmd_process_ota_flow((unsigned char *)&gRead_buf[device_id]);
                }
                else if(cmd_id == I2CCOMM_CMD_OTA_SEND_CHIP_ID)
                {
                    uint32_t chipid = 0x57454949;

                    //hx_drv_scu_get_chipid(&chipid);
                    //dbg_printf(DBG_LESS_INFO, "chipid: 0x%08x \n", chipid);
                    
                    app_i2c_cmd_write(device_id, feature_type, cmd_id, &chipid, sizeof(chipid));
                }
                break;
            case I2CCOMM_FEATURE_SYS:
                //app_sys_info_cmd_handler((unsigned char *)&gRead_buf[device_id], (unsigned char *)&gWrite_buf[device_id]);
                break;
            case I2CCOMM_FEATURE_SENSOR_REG_ACCESS:
                //board_delay_ms(200);//delay some times for sensor to get/set the register.
                //i2ccomm_cmd_process_sensor_reg_rw();
                //board_delay_ms(200);//delay some times for sensor to get/set the register.
                break;
			case I2C_CMD_CSTM_FEATURE_0:
                #ifndef BOARD_TEST
				if (cstm_feature_cb[0])
                    cstm_feature_cb[0](cmd_id, &gRead_buf[device_id][I2CFMT_PAYLOAD_OFFSET], payload_len);
                else
                    dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                #else
                app_board_test_cmd_handler(gRead_buf[device_id], gWrite_buf[device_id]);
                #endif
                break;
            case I2C_CMD_CSTM_FEATURE_1:
                if (cstm_feature_cb[1])
                    cstm_feature_cb[1](cmd_id, &gRead_buf[device_id][I2CFMT_PAYLOAD_OFFSET], payload_len);
                else
                    dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
            case I2C_CMD_CSTM_FEATURE_2:
                if (cstm_feature_cb[2])
                    cstm_feature_cb[2](cmd_id, &gRead_buf[device_id][I2CFMT_PAYLOAD_OFFSET], payload_len);
                else
                    dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
            case I2C_CMD_CSTM_FEATURE_3:
                if (cstm_feature_cb[3])
                    cstm_feature_cb[3](cmd_id, &gRead_buf[device_id][I2CFMT_PAYLOAD_OFFSET], payload_len);
                else
                    dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
            default:
                dbg_printf(DBG_LESS_INFO, "i2c_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
        }
        
        if (get_i2cs_enable_write(device_id) == false)
        {
            xprintf("no write trigger, enable read\n" );
            prv_evt_i2ccomm_clear_read_buf_header(device_id);
            hx_lib_i2ccomm_enable_read(device_id, (unsigned char *) &gRead_buf[device_id], I2CCOMM_MAX_RBUF_SIZE);
        }
        
        i2c_rx_ready[device_id] = false;
        
    }
}
#endif //OS_FREERTOS

int8_t app_i2c_cmd_write(USE_DW_IIC_SLV_E iic_id, uint8_t feature, uint8_t cmd, uint8_t *data, uint16_t data_len)
{
    I2CCOMM_ERROR_E i2c_comm_status;
    uint16_t checksum = 0;

    dbg_printf(DBG_LESS_INFO, "cmd = %d, data_len = %d\n", cmd, data_len);

    gWrite_buf[iic_id][I2CFMT_FEATURE_OFFSET] = feature;
    gWrite_buf[iic_id][I2CFMT_COMMAND_OFFSET] = cmd;
    gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] =  data_len & 0xFF;
    gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] = (data_len >> 8) & 0xFF;
    memcpy(&gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET], (data), data_len);

    hx_lib_i2ccomm_generate_checksum((unsigned char *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + data_len, &checksum);

    gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + data_len] = checksum & 0xFF;
    gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + data_len + 1] = (checksum >> 8) & 0xFF;

#if 0
    dbg_printf(DBG_LESS_INFO, "priv_cmd_i2c_write: checksum = 0x%04x\n", checksum);
    for (int i = 0; i < 4+data_len+2; i++) {
        dbg_printf(DBG_LESS_INFO, "gWrite_buf[%d] = 0x%02x\n", i, gWrite_buf[i]);
    }
#endif

    set_i2cs_enable_write(iic_id, true);

    // enable write process for i2c master read command
    hx_CleanDCache_by_Addr((volatile void *) &gWrite_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
    i2c_comm_status = hx_lib_i2ccomm_enable_write(iic_id, (unsigned char *) &gWrite_buf[iic_id]);

    if (I2CCOMM_NO_ERROR == i2c_comm_status)
        return 0;
    else
        return -1;
}

#if 0
int8_t app_i2c_algores_write(USE_DW_IIC_SLV_E iic_id, uint8_t *data, uint16_t data_len)
{
    I2CCOMM_ERROR_E i2c_comm_status;
    uint16_t checksum = 0;

    i2c_comm_status = hx_lib_i2ccomm_enable_algores_write(iic_id, data, data_len);
    hx_drv_gpio_set_output(GPIO0,GPIO_OUT_HIGH);
    hx_drv_timer_cm55x_delay_us(500, TIMER_STATE_DC);   
    hx_drv_gpio_set_output(GPIO0,GPIO_OUT_LOW);
    if (I2CCOMM_NO_ERROR == i2c_comm_status)
        return 0;
    else
        return -1;
}
#endif

void app_i2c_cmd_reg_cstm_feature(I2C_CMD_CSTM_FEATURE feature_id, I2C_CMD_CSTM_CB feature_callback)
{
    switch (feature_id)
    {
    	case I2C_CMD_CSTM_FEATURE_0:
            cstm_feature_cb[0] = feature_callback;
            break;
        case I2C_CMD_CSTM_FEATURE_1:
            cstm_feature_cb[1] = feature_callback;
            break;
        case I2C_CMD_CSTM_FEATURE_2:
            cstm_feature_cb[2] = feature_callback;
            break;
        case I2C_CMD_CSTM_FEATURE_3:
            cstm_feature_cb[3] = feature_callback;
            break;
        default:
            dbg_printf(DBG_LESS_INFO, "app_i2c_cmd_reg_cstm_feature: feature_id 0x%02x out of range!\n", feature_id);
            break;
    }
}


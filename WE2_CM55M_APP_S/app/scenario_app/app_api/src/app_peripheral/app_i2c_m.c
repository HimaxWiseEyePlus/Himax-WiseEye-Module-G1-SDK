#include "WE2_debug.h"

#include "app_api.h"
#include "app_i2c_m.h"

static HX_DRV_DEV_IIC_PTR dev_iic_m[2];

static void i2c_mst0_tx_callback_fun(void *pcontext)
{
    //dbg_printf(DBG_LESS_INFO, "I2C Master 0 TX CB_FUN\n");
    //dbg_printf(DBG_LESS_INFO, "Interrupt Context : %x\n", pcontext);
}

static void i2c_mst0_rx_callback_fun(void *pcontext)
{
    //dbg_printf(DBG_LESS_INFO, "I2C Master 0 RX CB_FUN\n");
    //dbg_printf(DBG_LESS_INFO, "Interrupt Context : %x\n", pcontext);
}

static void i2c_mst0_err_callback_fun(void *pcontext)
{
    //dbg_printf(DBG_LESS_INFO, "I2C Master 0 ERR CB_FUN\n");
    //dbg_printf(DBG_LESS_INFO, "Interrupt Context : %x\n", pcontext);
}

static void i2c_mst1_tx_callback_fun(void *pcontext)
{
    //dbg_printf(DBG_LESS_INFO, "I2C Master 1 TX CB_FUN\n");
    //dbg_printf(DBG_LESS_INFO, "Interrupt Context : %x\n", pcontext);
}

static void i2c_mst1_rx_callback_fun(void *pcontext)
{
    //dbg_printf(DBG_LESS_INFO, "I2C Master 1 RX CB_FUN\n");
    //dbg_printf(DBG_LESS_INFO, "Interrupt Context : %x\n", pcontext);
}

static void i2c_mst1_err_callback_fun(void *pcontext)
{
    //dbg_printf(DBG_LESS_INFO, "I2C Master 1 ERR CB_FUN\n");
    //dbg_printf(DBG_LESS_INFO, "Interrupt Context : %x\n", pcontext);
}

int8_t app_i2c_mst_init(uint8_t i2c_id, I2C_SPEED_MODE_E speed_mode)
{
    uint32_t clock = DW_IIC_SPEED_STANDARD;

    if(i2c_id < I2C_0_ID || i2c_id > I2C_1_ID)
    {
        dbg_printf(DBG_LESS_INFO, "%s, i2c_id is incorrect\n", __func__);
        return API_ERROR;
    }

    dev_iic_m[i2c_id] = hx_drv_i2cm_get_dev(i2c_id);
    
    if(i2c_id == I2C_0_ID)
    {
        dev_iic_m[i2c_id]->iic_info.iic_cbs.tx_cb = i2c_mst0_tx_callback_fun;
        dev_iic_m[i2c_id]->iic_info.iic_cbs.rx_cb = i2c_mst0_rx_callback_fun;
        dev_iic_m[i2c_id]->iic_info.iic_cbs.err_cb = i2c_mst0_err_callback_fun;
    }
    else if(i2c_id == I2C_1_ID)
    {
        dev_iic_m[i2c_id]->iic_info.iic_cbs.tx_cb = i2c_mst1_tx_callback_fun;
        dev_iic_m[i2c_id]->iic_info.iic_cbs.rx_cb = i2c_mst1_rx_callback_fun;
        dev_iic_m[i2c_id]->iic_info.iic_cbs.err_cb = i2c_mst1_err_callback_fun;
    }

    // i2c set_speed
    if(speed_mode == I2C_SPEED_100K)
        clock = DW_IIC_SPEED_STANDARD;
    else if(speed_mode == I2C_SPEED_400K)
        clock = DW_IIC_SPEED_FAST;
    else if(speed_mode == I2C_SPEED_1M)
        clock = DW_IIC_SPEED_FASTPLUS;
    
    dev_iic_m[i2c_id]->iic_control(DW_IIC_CMD_MST_SET_SPEED_MODE, (void*)clock);

    return API_SUCC;
}

int8_t app_i2c_mst_deinit(uint8_t i2c_id)
{
    dev_iic_m[i2c_id] = hx_drv_i2cm_get_dev(i2c_id);
    dev_iic_m[i2c_id]->iic_info.iic_cbs.tx_cb = NULL;
    dev_iic_m[i2c_id]->iic_info.iic_cbs.rx_cb = NULL;
    dev_iic_m[i2c_id]->iic_info.iic_cbs.err_cb = NULL;

    return API_SUCC;
}

int8_t app_i2c_mst_write(uint8_t i2c_id, uint8_t slav_addr, uint8_t *reg_addr, uint32_t reg_addr_len, uint8_t *tx_buf, uint32_t tx_len)
{
    int32_t ret;

    ret = hx_drv_i2cm_write_data(i2c_id, slav_addr, reg_addr, reg_addr_len, tx_buf, tx_len);
    
    if(ret < E_OK)
    {
        dbg_printf(DBG_LESS_INFO, "%s, ret = %d\n", __func__, ret);
        return API_ERROR;
    }

    return API_SUCC;
}

int8_t app_i2c_mst_write_read(uint8_t i2c_id, uint8_t slav_addr, uint8_t *reg_addr, uint32_t reg_addr_len, uint8_t *rx_buf, uint32_t rx_len)
{
    int32_t ret;
    
    ret = hx_drv_i2cm_write_restart_read(i2c_id, slav_addr, reg_addr, reg_addr_len, rx_buf, rx_len);
    //ret = hx_drv_i2cm_write_stop_read(i2c_id, slav_addr, reg_addr, reg_addr_len, rx_buf, rx_len);

    if(ret < E_OK)
    {
        dbg_printf(DBG_LESS_INFO, "%s, ret = %d\n", __func__, ret);
        return API_ERROR;
    }

    return API_SUCC;
}

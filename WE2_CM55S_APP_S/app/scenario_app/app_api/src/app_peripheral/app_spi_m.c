//#include "hx_drv_spi_m.h"
//#include "hx_drv_iomux.h"

#include "WE2_debug.h"

#include "app_api.h"
#include "app_spi_m.h"

static bool app_spi_m0_rx_busy_status = false;
static bool app_spi_m1_rx_busy_status = false;
static bool app_spi_m2_rx_busy_status = false;

static bool app_spi_m0_tx_busy_status = false;
static bool app_spi_m1_tx_busy_status = false;
static bool app_spi_m2_tx_busy_status = false;

static void app_spi_m0_callback_fun_rx(void *status)
{
    dbg_printf(DBG_MORE_INFO,"SPI Master0 RX CB_FUN\n");
    app_spi_m0_rx_busy_status = false;
    return;
}

static void app_spi_m0_callback_fun_tx(void *status)
{
	dbg_printf(DBG_MORE_INFO,"SPI Master0 TX CB_FUN\n");
    app_spi_m0_tx_busy_status = false;
    return;
}

static void app_spi_m0_callback_fun_err(void *status)
{
	dbg_printf(DBG_MORE_INFO,"SPI Master0 ERR CB_FUN\n");
    return;
}

static void app_spi_m1_callback_fun_rx(void *status)
{
    dbg_printf(DBG_MORE_INFO,"SPI Master1 RX CB_FUN\n");
    app_spi_m1_rx_busy_status = false;
    return;
}

static void app_spi_m1_callback_fun_tx(void *status)
{
	dbg_printf(DBG_MORE_INFO,"SPI Master1 TX CB_FUN\n");
    app_spi_m1_tx_busy_status = false;
    return;
}

static void app_spi_m1_callback_fun_err(void *status)
{
	dbg_printf(DBG_MORE_INFO,"SPI Master1 ERR CB_FUN\n");
    return;
}

static void app_spi_m2_callback_fun_rx(void *status)
{
    dbg_printf(DBG_MORE_INFO,"SPI Master2 RX CB_FUN\n");
    app_spi_m2_rx_busy_status = false;
    return;
}

static void app_spi_m2_callback_fun_tx(void *status)
{
	dbg_printf(DBG_MORE_INFO,"SPI Master2 TX CB_FUN\n");
    app_spi_m2_tx_busy_status = false;
    return;
}

static void app_spi_m2_callback_fun_err(void *status)
{
	dbg_printf(DBG_MORE_INFO,"SPI Master2 ERR CB_FUN\n");
    return;
}

static void app_spi_m0_cb_fun_rx(void)
{
    dbg_printf(DBG_MORE_INFO,"app_spi_m0_cb_fun_rx Master0 RX CB_FUN\n");

    app_spi_m0_rx_busy_status = false;
    return;
}

static void app_spi_m0_cb_fun_tx(void)
{
    dbg_printf(DBG_MORE_INFO,"app_spi_m0_cb_fun_tx Master0 TX CB_FUN\n");

    app_spi_m0_tx_busy_status = false;
    return;
}

static void app_spi_m1_cb_fun_rx(void)
{
    dbg_printf(DBG_MORE_INFO,"app_spi_m1_cb_fun_rx Master1 RX CB_FUN\n");

    app_spi_m1_rx_busy_status = false;
    return;
}

static void app_spi_m1_cb_fun_tx(void)
{
    dbg_printf(DBG_MORE_INFO,"app_spi_m1_cb_fun_tx Master1 TX CB_FUN\n");

    app_spi_m1_tx_busy_status = false;
    return;
}

static void app_spi_m2_cb_fun_rx(void)
{
    dbg_printf(DBG_MORE_INFO,"app_spi_m2_cb_fun_rx Master1 RX CB_FUN\n");

    app_spi_m2_rx_busy_status = false;
    return;
}

static void app_spi_m2_cb_fun_tx(void)
{
    dbg_printf(DBG_MORE_INFO,"app_spi_m2_cb_fun_tx Master1 TX CB_FUN\n");

    app_spi_m2_tx_busy_status = false;
    return;
}

int8_t app_spi_m_open(SPI_MST_E spi_id, uint32_t freq)
{
    DEV_SPI_INFO_PTR info;
    DEV_SPI_PTR dev_spi_m;
    
    if(spi_id < SPI_MST_0 || spi_id > SPI_MST_2)
        return API_INVALID_ID;
    
    dev_spi_m = hx_drv_spi_mst_get_dev((USE_DW_SPI_MST_E)spi_id);

    dev_spi_m->spi_open(DEV_MASTER_MODE, freq); //master mode, spi clock, cpu clock

    info = &(dev_spi_m->spi_info);

    if(spi_id == SPI_MST_0)
    {
        info->spi_cbs.tx_cb = app_spi_m0_callback_fun_tx;
        info->spi_cbs.rx_cb = app_spi_m0_callback_fun_rx;
        info->spi_cbs.err_cb = app_spi_m0_callback_fun_err;
        app_spi_m0_rx_busy_status = false;
        app_spi_m0_tx_busy_status = false;
    }
    else if(spi_id == SPI_MST_1)
    {
        info->spi_cbs.tx_cb = app_spi_m1_callback_fun_tx;
        info->spi_cbs.rx_cb = app_spi_m1_callback_fun_rx;
        info->spi_cbs.err_cb = app_spi_m1_callback_fun_err;
        app_spi_m1_rx_busy_status = false;
        app_spi_m1_tx_busy_status = false;
    }
    else if(spi_id == SPI_MST_2)
    {
        info->spi_cbs.tx_cb = app_spi_m2_callback_fun_tx;
        info->spi_cbs.rx_cb = app_spi_m2_callback_fun_rx;
        info->spi_cbs.err_cb = app_spi_m2_callback_fun_err;
        app_spi_m2_rx_busy_status = false;
        app_spi_m2_tx_busy_status = false;
    }

    return API_SUCC;
}

int8_t app_spi_m_close(SPI_MST_E spi_id)
{
    if(spi_id < SPI_MST_0 || spi_id > SPI_MST_2)
        return API_INVALID_ID;

	hx_drv_spi_mst_deinit((USE_DW_SPI_MST_E)spi_id);

    return API_SUCC;
}

int8_t app_spi_m_write(SPI_MST_E spi_id, uint8_t *tx_data, uint32_t tx_len, SPI_CMD_DATA_TYPE data_type)
{
    DEV_SPI_PTR dev_spi_m;
    uint32_t wait_count;
    bool *spi_m_tx_busy_status;
    
    typedef void (*func)(void);
    func *func_ptr = NULL;
    
    if(spi_id < SPI_MST_0 || spi_id > SPI_MST_2)
        return API_INVALID_ID;

    if(tx_data == NULL)
		return API_INVALID_DATA;
    
	if(tx_len == 0)
		return API_INVALID_SIZE;
    
    if(spi_id == SPI_MST_0)
        spi_m_tx_busy_status = &app_spi_m0_tx_busy_status;
    else if(spi_id == SPI_MST_1)
        spi_m_tx_busy_status = &app_spi_m1_tx_busy_status;
    else if(spi_id == SPI_MST_2)
        spi_m_tx_busy_status = &app_spi_m2_tx_busy_status;
    
    if(*spi_m_tx_busy_status == true)
        return API_ERROR;

    *spi_m_tx_busy_status = true;

    dev_spi_m = hx_drv_spi_mst_get_dev((USE_DW_SPI_MST_E)spi_id);
    
    if(spi_id == SPI_MST_0)
        func_ptr = app_spi_m0_cb_fun_tx;
    else if(spi_id == SPI_MST_1)
        func_ptr = app_spi_m1_cb_fun_tx;
    else if(spi_id == SPI_MST_2)
        func_ptr = app_spi_m2_cb_fun_tx;
    
    dev_spi_m->spi_control(SPI_CMD_SET_DMA_TXCB, (void *)func_ptr);
    hx_spimcomm_register_tx_cb(spi_id, (spimcomm_cb_t)func_ptr);
    
    if(data_type == 0x00)
    {
        int32_t *ret;
        
        ret = dev_spi_m->spi_write_dma(tx_data, tx_len, (void *)func_ptr);

        if(*ret < 0)
        {
            dbg_printf(DBG_LESS_INFO, "spi_write_dma() error\n");
            *spi_m_tx_busy_status = false;
            return API_ERROR;
        }
    }
    else
    {
        uint8_t spim_ptl_hdr_buf[7];
        int32_t ret;
        
        spim_ptl_hdr_buf[0] = 0xC0;                    // sync
        spim_ptl_hdr_buf[1] = 0x5A;                    // sync
        spim_ptl_hdr_buf[2] = (data_type) & 0xff;    // data_type
        spim_ptl_hdr_buf[3] = tx_len & 0xff;         // data size
        spim_ptl_hdr_buf[4] = (tx_len >> 8) & 0xff;  // data size
        spim_ptl_hdr_buf[5] = (tx_len >> 16) & 0xff; // data size
        spim_ptl_hdr_buf[6] = (tx_len >> 24) & 0xff; // data size

        ret = dev_spi_m->spi_write_ptl(spim_ptl_hdr_buf, 7, tx_data, tx_len, (void *)func_ptr);
        
        if(ret < 0)
        {
            dbg_printf(DBG_LESS_INFO, "spi_write_ptl() error\n");
            *spi_m_tx_busy_status = false;
            return API_ERROR;
        }
    }

    wait_count = 0;
    while(*spi_m_tx_busy_status == true)
    {
        board_delay_ms(10);

        if(wait_count++ >300)
        {
            dev_spi_m->spi_write_ptl_halt();
        	*spi_m_tx_busy_status = false;
            dbg_printf(DBG_LESS_INFO, "spi_write_ptl_halt()\n");
            return API_ERROR;
        }
    }
    
    return API_SUCC;
}

int8_t app_spi_m_read(SPI_MST_E spi_id, uint8_t *rx_data, uint32_t rx_len)
{
    DEV_SPI_PTR dev_spi_m;
    uint32_t wait_count;
    int32_t *ret;
    bool *spi_m_rx_busy_status;
    
    typedef void (*func)(void);
    func *func_ptr = NULL;
    
    if(spi_id < SPI_MST_0 || spi_id > SPI_MST_2)
        return API_INVALID_ID;

    if(rx_data == NULL)
		return API_INVALID_DATA;

    if(rx_len == 0)
		return API_INVALID_SIZE;
    
    if(spi_id == SPI_MST_0)
        spi_m_rx_busy_status = &app_spi_m0_rx_busy_status;
    else if(spi_id == SPI_MST_1)
        spi_m_rx_busy_status = &app_spi_m1_rx_busy_status;
    else if(spi_id == SPI_MST_2)
        spi_m_rx_busy_status = &app_spi_m2_rx_busy_status;

    if(*spi_m_rx_busy_status == true)
        return API_ERROR;
    
    *spi_m_rx_busy_status = true;
    
	if(spi_id == SPI_MST_0)
        func_ptr = app_spi_m0_cb_fun_rx;
	else if (spi_id == SPI_MST_1)
        func_ptr = app_spi_m1_cb_fun_rx;
    else if (spi_id == SPI_MST_2)
        func_ptr = app_spi_m2_cb_fun_rx;

    dev_spi_m = hx_drv_spi_mst_get_dev((USE_DW_SPI_MST_E)spi_id);
    
    dev_spi_m->spi_control(SPI_CMD_SET_DMA_RXCB, (void *)func_ptr);
    hx_spimcomm_register_rx_cb(spi_id, (spimcomm_cb_t)func_ptr);
    
	ret = dev_spi_m->spi_read_dma(rx_data, rx_len, func_ptr);
    
    if(*ret < 0)
    {
        dbg_printf(DBG_LESS_INFO, "spi_read_dma() error\n");
        *spi_m_rx_busy_status = false;
        return API_ERROR;
    }

    wait_count = 0;
    while(*spi_m_rx_busy_status == true)
    {
        board_delay_ms(10);
        
        if(wait_count++ > 100)
        {
            dbg_printf(DBG_LESS_INFO, "app_spi_m_read() wait time out\n");
            *spi_m_rx_busy_status = false;
            return API_ERROR;
        }
    }
    /*
    for(int i = 0; i < rx_len; i++)
	{
		dbg_printf(DBG_LESS_INFO, "%02x ", rx_data[i]);
        if((i%16)==15)
            dbg_printf(DBG_LESS_INFO, "\n");
	}
	dbg_printf(DBG_LESS_INFO, "\n");
	*/
    
    return API_SUCC;
}


#include "hx_drv_spi.h"
#include "hx_drv_timer.h"

#include "WE2_debug.h"

#include "app_api.h"
#include "app_spi_s.h"

typedef struct {
    uint8_t pin_num;
    HXGPIO_VALUE pin_actv_lvl;
}spi_s_handshake;

static bool app_spi_s_rx_busy_status = false;
static bool app_spi_s_tx_busy_status = false;
static spi_s_handshake app_tx_handshake = {HX_GPIO_INVAL, HX_GPIO_LOW};

static void app_spi_s_callback_fun_rx(void *status)
{
    dbg_printf(DBG_MORE_INFO, "SPI Slave RX CB_FUN\n");
    app_spi_s_rx_busy_status = false;
    return;
}

static void app_spi_s_callback_fun_tx(void *status)
{
    dbg_printf(DBG_LESS_INFO, "SPI Slave TX CB_FUN\n");
    
    if(app_tx_handshake.pin_num != HX_GPIO_INVAL)
        hx_drv_gpio_set_out_value((GPIO_INDEX_E)app_tx_handshake.pin_num, (app_tx_handshake.pin_actv_lvl == HX_GPIO_LOW)?GPIO_OUT_HIGH:GPIO_OUT_LOW);/*data sent done, restore to default level*/
    app_spi_s_tx_busy_status = false;
    return;
}

static void app_spi_s_callback_fun_err(void *status)
{
    dbg_printf(DBG_MORE_INFO, "SPI Slave ERR CB_FUN\n");
    return;
}

static void app_spi_s_cb_fun_tx(void)
{
    dbg_printf(DBG_LESS_INFO, "app_spi_s_cb_fun_tx\n");

    if(app_tx_handshake.pin_num != HX_GPIO_INVAL)
        hx_drv_gpio_set_out_value((GPIO_INDEX_E)app_tx_handshake.pin_num, (app_tx_handshake.pin_actv_lvl == HX_GPIO_LOW)?GPIO_OUT_HIGH:GPIO_OUT_LOW);/*data sent done, restore to default level*/
    app_spi_s_tx_busy_status = false;
    return;
}

static void app_spi_s_cb_fun_rx(void)
{
    dbg_printf(DBG_LESS_INFO, "app_spi_s_cb_fun_rx\n");
    app_spi_s_rx_busy_status = false;
    return;
}

int8_t app_spi_s_open()
{
    DEV_SPI_INFO_PTR info; 
    DEV_SPI_PTR dev_spi_s;
	
	hx_drv_spi_slv_init(USE_DW_SPI_SLV_0, DW_SSPI_S_RELBASE);
    dev_spi_s = hx_drv_spi_slv_get_dev(USE_DW_SPI_SLV_0);

    info = &(dev_spi_s->spi_info);

    info->spi_cbs.tx_cb = app_spi_s_callback_fun_tx;
    info->spi_cbs.rx_cb = app_spi_s_callback_fun_rx;
    info->spi_cbs.err_cb = app_spi_s_callback_fun_err;

    dev_spi_s->spi_open(DEV_SLAVE_MODE, SPI_CPOL_0_CPHA_0);
    
    dev_spi_s->spi_control(SPI_CMD_SET_DMA_TXCB, (void *)app_spi_s_callback_fun_tx);
    dev_spi_s->spi_control(SPI_CMD_SET_DMA_RXCB, (void *)app_spi_s_callback_fun_rx);

    app_spi_s_rx_busy_status = false;
    app_spi_s_tx_busy_status = false;

    return API_SUCC;
}

int8_t app_spi_s_close()
{
    hx_drv_spi_slv_deinit(USE_DW_SPI_SLV_0);
    return API_SUCC;
}

int8_t app_spi_s_write(uint8_t *tx_data, uint32_t tx_len, SPI_CMD_DATA_TYPE data_type)
{
    DEV_SPI_PTR dev_spi_s;
    uint32_t wait_count;
    int32_t ret;
    
    if(tx_data == NULL)
		return API_INVALID_DATA;
    
	if(tx_len == 0)
		return API_INVALID_SIZE;
    
    if(app_spi_s_tx_busy_status == true)
        return API_ERROR;
    
    app_spi_s_tx_busy_status = true;
    
    dev_spi_s = hx_drv_spi_slv_get_dev(USE_DW_SPI_SLV_0);

    dev_spi_s->spi_control(SPI_CMD_SET_DMA_TXCB, (void *)app_spi_s_cb_fun_tx);
    hx_spi_slv_comm_register_tx_cb((spimcomm_cb_t)app_spi_s_cb_fun_tx);

    if(app_tx_handshake.pin_num != HX_GPIO_INVAL)
    {
        dbg_printf(DBG_LESS_INFO, "hx_drv_gpio_set_out_value %x %u\n", app_tx_handshake.pin_num, app_tx_handshake.pin_actv_lvl);
        hx_drv_gpio_set_out_value((GPIO_INDEX_E)app_tx_handshake.pin_num, (app_tx_handshake.pin_actv_lvl == HX_GPIO_LOW)?GPIO_OUT_LOW:GPIO_OUT_HIGH); /*data available, set active level*/
    }
    
    if(data_type == 0x00)
    {
        ret = dev_spi_s->spi_write(tx_data, tx_len);
    }
    else
    {
        uint8_t spim_ptl_hdr_buf[7];
        
        spim_ptl_hdr_buf[0] = 0xC0;                    // sync
        spim_ptl_hdr_buf[1] = 0x5A;                    // sync
        spim_ptl_hdr_buf[2] = (data_type) & 0xff;    // data_type
        spim_ptl_hdr_buf[3] = tx_len & 0xff;         // data size
        spim_ptl_hdr_buf[4] = (tx_len >> 8) & 0xff;  // data size
        spim_ptl_hdr_buf[5] = (tx_len >> 16) & 0xff; // data size
        spim_ptl_hdr_buf[6] = (tx_len >> 24) & 0xff; // data size

        ret = dev_spi_s->spi_write_ptl(spim_ptl_hdr_buf, 7, tx_data, tx_len, (void *)app_spi_s_cb_fun_tx);
    }
    
    if(ret < 0)
    {
        if(app_tx_handshake.pin_num != HX_GPIO_INVAL)
        {
            dbg_printf(DBG_LESS_INFO, "hx_drv_gpio_set_out_value %x %u\n", app_tx_handshake.pin_num, (app_tx_handshake.pin_actv_lvl == HX_GPIO_LOW)?GPIO_OUT_HIGH:GPIO_OUT_LOW);
            hx_drv_gpio_set_out_value((GPIO_INDEX_E)app_tx_handshake.pin_num, (app_tx_handshake.pin_actv_lvl == HX_GPIO_LOW)?GPIO_OUT_HIGH:GPIO_OUT_LOW); /*fail, restore to default level*/
        }
        app_spi_s_tx_busy_status = false;
        dbg_printf(DBG_LESS_INFO, "spi_write_ptl FAILED.\n");
        return API_ERROR;
    }
    
    wait_count = 0;
    while(app_spi_s_tx_busy_status == true)
    {
    	board_delay_ms(10);
        
        if(wait_count++ > 300)
        {
            dev_spi_s->spi_write_ptl_halt();
            if(app_tx_handshake.pin_num != HX_GPIO_INVAL)
            {
                dbg_printf(DBG_LESS_INFO, "hx_drv_gpio_set_out_value %x %u\n", app_tx_handshake.pin_num, (app_tx_handshake.pin_actv_lvl == HX_GPIO_LOW)?GPIO_OUT_HIGH:GPIO_OUT_LOW);
                hx_drv_gpio_set_out_value((GPIO_INDEX_E)app_tx_handshake.pin_num, (app_tx_handshake.pin_actv_lvl == HX_GPIO_LOW)?GPIO_OUT_HIGH:GPIO_OUT_LOW); /*timeout, restore to default level*/
            }
            app_spi_s_tx_busy_status = false;
            dbg_printf(DBG_LESS_INFO, "SPI WRITE TIMEOUT(3000).\n");
            return API_ERROR;
        }
    }
    
    return API_SUCC;
}

int8_t app_spi_s_preset_write_handshake(uint8_t gpio_num, uint8_t gpio_actv_lvl)
{
    app_tx_handshake.pin_num = gpio_num;
    app_tx_handshake.pin_actv_lvl = (gpio_actv_lvl==0)?HX_GPIO_LOW:HX_GPIO_HIGH;
    
    dbg_printf(DBG_LESS_INFO, "app_tx_handshake.pin_num %02x\n", app_tx_handshake.pin_num);
    dbg_printf(DBG_LESS_INFO, "app_tx_handshake.pin_actv_lvl %u\n", app_tx_handshake.pin_actv_lvl);

    if(app_tx_handshake.pin_num != HX_GPIO_INVAL)
    {
        hx_drv_gpio_set_output((GPIO_INDEX_E)app_tx_handshake.pin_num, (app_tx_handshake.pin_actv_lvl == HX_GPIO_LOW)?GPIO_OUT_HIGH:GPIO_OUT_LOW); /*active level is contrary to preset default level*/
    }
    
    return API_SUCC;
}

int8_t app_spi_s_read(uint8_t *rx_data, uint32_t rx_len)
{
    DEV_SPI_PTR dev_spi_s;
    uint32_t wait_count = 0;
    int32_t ret;
    
    if(rx_data == NULL)
		return API_INVALID_DATA;

    if(rx_len == 0)
		return API_INVALID_SIZE;

    if(app_spi_s_rx_busy_status == true)
        return API_ERROR;
    
    app_spi_s_rx_busy_status = true;
    
    dev_spi_s = hx_drv_spi_slv_get_dev(USE_DW_SPI_SLV_0);
	
    ret = dev_spi_s->spi_read_dma(rx_data, rx_len, app_spi_s_cb_fun_rx);
	hx_InvalidateDCache_by_Addr((volatile void *)rx_data, sizeof(uint8_t) * rx_len);
	
    if(ret < 0)
    {
        app_spi_s_rx_busy_status = false;
        return API_ERROR;
    }

    wait_count = 0;
    while(app_spi_s_rx_busy_status == true)
    {
    	board_delay_ms(10);

        if(wait_count++ > 100)
        {
            app_spi_s_rx_busy_status = false;
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


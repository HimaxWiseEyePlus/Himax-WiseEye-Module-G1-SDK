#include "hx_drv_spi.h"
#include "hx_drv_timer.h"

#ifdef SPI_MASTER_SEND
#include "spi_master_protocol.h"
#endif
#ifdef SPI_SLAVE_SEND
#include "spi_slave_protocol.h"
#endif

#include "WE2_debug.h"
#include "app_api.h"

//#include "app_spi_m.h"
//#include "app_spi_s.h"
//#include "app_uart.h"
#include "app_xdma_cfg.h"

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"

#if defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND)
SemaphoreHandle_t mutex_spi = NULL;
#elif defined(UART_PROTOCOL)
extern SemaphoreHandle_t mutex_uart;
#endif
#endif

/********************** SPI *************************/
#if defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND)
APP_SPI_CFG_T g_spi_cfg = {
    #if defined(SPI_MASTER_SEND)
    .mst_id = SPI_MST_1,
    .mst_freq = 10000000,
    #endif
    #if defined(SPI_SLAVE_SEND)
    .slv_handshake_pin = HX_GPIO_INVAL,
    .slv_handshake_actv_lvl = HX_GPIO_HIGH,
    #endif
};

int32_t app_spi_init(APP_SPI_CFG_T *spi_cfg)
{
    int32_t ret = 0;
    
    #if defined(OS_FREERTOS) && (defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND))
    mutex_spi=xSemaphoreCreateMutex();
	if(mutex_spi == NULL)
	{
		dbg_printf(DBG_LESS_INFO, "mutex_spi error\n");
		return -1;
	}
    #endif

    if(spi_cfg != NULL)
    {
        #if defined(SPI_MASTER_SEND)
        g_spi_cfg.mst_id= spi_cfg->mst_id;
        g_spi_cfg.mst_freq = spi_cfg->mst_freq;
        #endif
        #if defined(SPI_SLAVE_SEND)
        g_spi_cfg.slv_handshake_pin = spi_cfg->slv_handshake_pin;
        g_spi_cfg.slv_handshake_actv_lvl = spi_cfg->slv_handshake_actv_lvl;
        #endif
    }
    
    #if defined(SPI_MASTER_SEND)
    ret = app_spi_m_open(g_spi_cfg.mst_id, g_spi_cfg.mst_freq); //10MHz
    dbg_printf(DBG_LESS_INFO, "app_spi_m_open() return %d\n", ret);
    #elif defined(SPI_SLAVE_SEND)
    ret = app_spi_s_open();
    dbg_printf(DBG_LESS_INFO, "app_spi_s_open() return %d\n", ret);
    if(g_spi_cfg.slv_handshake_pin != HX_GPIO_INVAL)
        app_spi_s_preset_write_handshake(g_spi_cfg.slv_handshake_pin, g_spi_cfg.slv_handshake_actv_lvl);

    dbg_printf(DBG_LESS_INFO, "g_spi_cfg.slv_handshake_pin %02x\n", g_spi_cfg.slv_handshake_pin);
    dbg_printf(DBG_LESS_INFO, "g_spi_cfg.slv_handshake_actv_lvl %u\n", g_spi_cfg.slv_handshake_actv_lvl);
    #endif
    
    return ret;
}

int32_t app_spi_write(uint32_t addr, uint32_t size, SPI_CMD_DATA_TYPE data_type)
{
    int8_t ret = 0;

    dbg_printf(DBG_LESS_INFO, "app_spi_write(addr = 0x%08x, size = %d, data_type = %d)\n", addr, size, data_type);

    #if defined(OS_FREERTOS) && (defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND))
    xSemaphoreTake(mutex_spi,portMAX_DELAY);
    #endif

    #if defined(SPI_MASTER_SEND)
    ret = app_spi_m_write(g_spi_cfg.mst_id, (uint8_t*)addr, size, data_type);
    dbg_printf(DBG_LESS_INFO, "app_spi_m_write return %d\n", ret);
    #elif defined(SPI_SLAVE_SEND)
    ret = app_spi_s_write((uint8_t*)addr, size, data_type);
    dbg_printf(DBG_LESS_INFO, "app_spi_s_write return %d\n", ret);
    #endif

    #if defined(OS_FREERTOS) && (defined(SPI_MASTER_SEND) || defined(SPI_SLAVE_SEND))
    xSemaphoreGive(mutex_spi);
    #endif 

    return ret;
}
#endif

/********************** UART *************************/
#if defined(UART_PROTOCOL)
static uint8_t tx_header[7];
#define BOARD_COMMAND_UART_ID UART_1_ID
int32_t app_uart_tx(uint32_t addr, uint32_t size, SPI_CMD_DATA_TYPE data_type)
{
    int32_t ret = 0;

    #if defined(OS_FREERTOS) && defined (UART_PROTOCOL)
    xSemaphoreTake(mutex_uart, portMAX_DELAY);
    #endif

    dbg_printf(DBG_LESS_INFO, "app_uart_write(size = %d, data_type = %d)\n", size, data_type);
    tx_header[0] = 0xc0;
    tx_header[1] = 0x5a;
    tx_header[2] = data_type;
    tx_header[3] = size&0xff;
	tx_header[4] = (size>>8)&0xff;
	tx_header[5] = (size>>16)&0xff;
	tx_header[6] = (size>>24)&0xff;

    ret = app_uart_write(BOARD_COMMAND_UART_ID, tx_header, sizeof(tx_header));
    dbg_printf(DBG_LESS_INFO, "app_uart_write(header) return %d\n", ret);
	hx_InvalidateDCache_by_Addr((void *)tx_header, sizeof(tx_header));
	
    ret = app_uart_write(BOARD_COMMAND_UART_ID, (uint8_t*)addr, size);
    dbg_printf(DBG_LESS_INFO, "app_uart_write(payload) return %d\n", ret);
	
	if(data_type == DATA_TYPE_JPG)
		hx_InvalidateDCache_by_Addr((void *)addr, WDMA2_SLOT_SIZE);
	else if(data_type == DATA_TYPE_RAW_IMG)
		hx_InvalidateDCache_by_Addr((void *)addr, WDMA3_SIZE);
    
    #if defined(OS_FREERTOS) && defined (UART_PROTOCOL)
    xSemaphoreGive(mutex_uart);
    #endif
    
    return ret;
}
#endif

/********************** GPIO *************************/
int8_t app_gpio_set_mode(HXGPIO gpio_num, HXGPIO_MODE mode, HXGPIO_VALUE def_val)
{    
    if(gpio_num != HX_GPIO_INVAL)
    {
        if(mode == HX_GPIO_INPUT)
            hx_drv_gpio_set_input(gpio_num);
        else if(mode == HX_GPIO_OUTPUT)
            hx_drv_gpio_set_output(gpio_num, def_val);
        
        return 0;
    }
    else
    {
        dbg_printf(DBG_LESS_INFO, "app_gpio_set_mode: invalid gpio_num(%u)\n", gpio_num);
        return -1;
    }
}

int8_t app_gpio_set_value(HXGPIO gpio_num, HXGPIO_VALUE out_value)
{
    if(gpio_num != HX_GPIO_INVAL)
    {
        hx_drv_gpio_set_out_value(gpio_num, (out_value == HX_GPIO_LOW)?GPIO_OUT_LOW:GPIO_OUT_HIGH);
        return 0;
    }
    else
    {
        dbg_printf(DBG_LESS_INFO, "app_gpio_set_value: invalid gpio_num(%u)\n", gpio_num);
        return -1;
    }
}

int8_t app_gpio_get_value(HXGPIO gpio_num, uint8_t *in_value)
{
    if(gpio_num != HX_GPIO_INVAL)
    {
        hx_drv_gpio_get_in_value(gpio_num, in_value);
        return 0;
    }
    else
    {
        dbg_printf(DBG_LESS_INFO, "app_gpio_get_value: invalid gpio_num(%u)\n", gpio_num);
        return -1;
    }
}

static void priv_gpio_cb(uint8_t group, uint8_t idx) /*for interrupt unregister use*/
{
}

void app_gpio_intr_register(HXGPIO gpio_num, HXGPIO_CB_T cb, HXGPIO_IRQ_TRIG_TYPE type)
{
    GPIO_INDEX_E index = (GPIO_INDEX_E)gpio_num;
    gpio_cb_t cb_fun = (gpio_cb_t)cb;
    GPIO_IRQ_TRIG_TYPE_E trig_type = (GPIO_IRQ_TRIG_TYPE_E)type;
    
    hx_drv_gpio_set_int_enable(index, 0);
    hx_drv_gpio_set_int_type(index, trig_type);
	hx_drv_gpio_set_irq_handle(index, GPIO_IRQ_HANDLE_CLR_INT/*GPIO_IRQ_HANDLE_NONE*/);
    hx_drv_gpio_cb_register(index, cb_fun);
    hx_drv_gpio_set_input(index);
    hx_drv_gpio_set_int_enable(index, 1);
}

void app_gpio_intr_unregister(HXGPIO gpio_num)
{
    GPIO_INDEX_E index = (GPIO_INDEX_E)gpio_num;
    
    hx_drv_gpio_set_int_enable(index, 0);
    hx_drv_gpio_cb_register(index, priv_gpio_cb);
}

/********************** LED *************************/
void app_led_init()
{
    hx_drv_gpio_set_output(HX_GPIO_0, GPIO_OUT_LOW);
    hx_drv_gpio_set_output(HX_GPIO_1, GPIO_OUT_LOW);
}

void app_led_green(uint8_t on)
{
    if(on == 0)
        hx_drv_gpio_set_out_value(HX_GPIO_0, GPIO_OUT_LOW);
    else
        hx_drv_gpio_set_out_value(HX_GPIO_0, GPIO_OUT_HIGH);
}

void app_led_blue(uint8_t on)
{
    if(on == 0)
        hx_drv_gpio_set_out_value(HX_GPIO_1, GPIO_OUT_LOW);
    else
        hx_drv_gpio_set_out_value(HX_GPIO_1, GPIO_OUT_HIGH);

}

/********************** Timer *************************/
uint32_t app_timer_get_current_time_ms()
{
#define CPU_CLK					(0xffffff+1)
#define BOARD_SYS_TIMER_US_HZ	(1000000)

	uint32_t total_us;
	uint32_t systick;
	uint32_t loop_cnt;
	uint32_t tick;

	SystemGetTick(&systick, &loop_cnt);
	tick = (CPU_CLK-systick) + (loop_cnt * CPU_CLK);
	total_us = (uint32_t)((float)tick*((float)BOARD_SYS_TIMER_US_HZ/(float)SystemCoreClock)); // /400

    return total_us/1000;
}

int8_t app_delay_ms(uint32_t delay_ms)
{
    board_delay_ms(delay_ms);
    return 0;
}

/********************** yuv422 packed to planar transform*************************/

void app_yuv422packed2planar(uint8_t *yuv_packed, app_yuv_planar_t *yuv_planar, uint32_t yuv_img_sz)
{
    //dbg_printf(DBG_LESS_INFO, "hm0435_cis_yuv422_packed2planar, yuv_img_sz= %d, yuv_packed= 0x%x, yuv_planar= 0x%x\n", yuv_img_sz, yuv_packed, yuv_planar);
    int idx = 0;
    uint32_t y_img_size = yuv_img_sz >> 1;
    uint32_t uv_img_size = yuv_img_sz >> 2;
    
    for(idx = 0; idx < y_img_size; idx++)
    {
        yuv_planar->pY[idx] = yuv_packed[idx*2];
        if(idx < uv_img_size) //both U, V are half of image size
        {
            yuv_planar->pU[idx] = yuv_packed[1+idx*4];
            yuv_planar->pV[idx] = yuv_packed[3+idx*4];
        }
    }
}

void app_yuv422packed2planar2(uint8_t *yuv_packed, app_yuv_planar_t *yuv_planar, uint32_t yuv_img_sz)
{
    //dbg_printf(DBG_LESS_INFO, "hm0435_cis_yuv422_packed2planar, yuv_img_sz= %d, yuv_packed= 0x%x, yuv_planar= 0x%x\n", yuv_img_sz, yuv_packed, yuv_planar);
    int idx = 0;
    uint32_t y_img_size = yuv_img_sz >> 1;
    uint32_t u_img_size = yuv_img_sz >> 2;
    uint32_t v_img_size = yuv_img_sz >> 2;
    uint32_t y_planar_idx = y_img_size-1;
    uint32_t u_planar_idx = u_img_size-1;
    uint32_t v_planar_idx = v_img_size-1;
    
    for(idx = yuv_img_sz-1; (idx >= 0) && (v_planar_idx >= 0); idx-=4)/*V*/
    {
        yuv_planar->pV[v_planar_idx--] = yuv_packed[idx];
    }

    for(idx = yuv_img_sz-3; (idx >= 0) && (u_planar_idx >= 0); idx-=4)/*U*/
    {
        yuv_planar->pU[u_planar_idx--] = yuv_packed[idx];
    }
    
    for(idx = yuv_img_sz-2; (idx >= 0) && (y_planar_idx >= 0); idx-=2)/*Y*/
    {
        yuv_planar->pY[y_planar_idx--] = yuv_packed[idx];
    }
}



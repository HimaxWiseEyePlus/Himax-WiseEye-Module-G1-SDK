#include "hx_drv_uart.h"

#include "WE2_debug.h"

#include "app_api.h"
#include "app_uart.h"

#if defined(OS_FREERTOS)
#include "FreeRTOS.h"
#include "semphr.h"
//#include "portmacro.h"

SemaphoreHandle_t mutex_uart;
#endif

/* uart sample functions */
#define RX_FIFO_SIZE (1024)
uint8_t rx_fifo[RX_FIFO_SIZE] = {0};
uint32_t rx_fifo_head = 0, rx_fifo_tail = 0;
uint32_t g_uart_id = UART_MAX_ID;

bool full = false;

static void uart_callback_fun_tx(void *ctx)
{
    //dbg_printf(DBG_LESS_INFO, "uart_callback_fun_tx()\n");
}

static void uart_callback_fun_rx(void *ctx)
{
    DEV_UART_PTR dev_uart_comm;
    uint32_t rd_avail;
    uint32_t buf_empty_size;
    uint32_t read_size;

    dev_uart_comm = hx_drv_uart_get_dev(g_uart_id);
    dev_uart_comm->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&rd_avail));

    if(rd_avail > 0 && full == false)
    {
        if(rx_fifo_head > rx_fifo_tail)
            buf_empty_size = rx_fifo_head - rx_fifo_tail;
        else
            buf_empty_size = (RX_FIFO_SIZE - rx_fifo_tail) + rx_fifo_head;
            
        if(buf_empty_size < rd_avail)
            read_size = buf_empty_size;
        else
            read_size = rd_avail;

        if(rx_fifo_tail + read_size > RX_FIFO_SIZE)
        {
            uint32_t sec_1 = RX_FIFO_SIZE - rx_fifo_tail;
            uint32_t sec_2 = read_size - sec_1;
            dev_uart_comm->uart_read((void *)&rx_fifo[rx_fifo_tail], sec_1);
            dev_uart_comm->uart_read((void *)rx_fifo, sec_2); 
        }
        else
        {
            dev_uart_comm->uart_read((void *)&rx_fifo[rx_fifo_tail], read_size);
        }

        if(rx_fifo_tail + read_size < RX_FIFO_SIZE)
            rx_fifo_tail += read_size;
        else
            rx_fifo_tail = rx_fifo_tail + read_size - RX_FIFO_SIZE;

        if(rx_fifo_tail == rx_fifo_head)
            full = true;
    }
}

static void uart_callback_fun_err(void *ctx)
{
    dbg_printf(DBG_LESS_INFO, "uart_callback_fun_err()\n");
}

int32_t app_uart_open(uint32_t uart_id, uint32_t baudrate)
{
    DEV_UART_PTR dev_uart_comm;
    int32_t ret;
    uint32_t uart_uart_base = HX_UART0_BASE;

    #if defined(OS_FREERTOS)
    mutex_uart=xSemaphoreCreateMutex();
    #endif
    
    if(uart_id < UART_0_ID || uart_id > UART_2_ID)
        return API_ERROR;

    g_uart_id = uart_id;
    if(uart_id == UART_0_ID)
        uart_uart_base = HX_UART0_BASE;
    else if(uart_id == UART_1_ID)
        uart_uart_base = HX_UART1_BASE;
    else if(uart_id == UART_2_ID)
        uart_uart_base = HX_UART2_BASE;

    ret = hx_drv_uart_init(uart_id, uart_uart_base);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "hx_drv_uart_init(),(%d)\n", ret); return API_ERROR;}
    
    dev_uart_comm = hx_drv_uart_get_dev(uart_id);
    
    ret = dev_uart_comm->uart_open(baudrate);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_open(),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_WORK_MODE, (void*)0);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_TRANSFER_MODE, UART_TRANSFER_INT),(%d)\n", ret); return API_ERROR;}

    ret = dev_uart_comm->uart_control(UART_CMD_SET_ERRCB, (void*)(uart_callback_fun_err));
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_ERRCB, uart_callback_fun_err),(%d)\n", ret); return API_ERROR;}
    //tx
    ret = dev_uart_comm->uart_control(UART_CMD_SET_TXINT, (void*)(DEV_DISABLED));
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_TXINT, DEV_DISABLED),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_TXINT_BUF, (void*)NULL);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_TXINT_BUF, NULL),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_TXCB, (void *)uart_callback_fun_tx);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_TXCB, uart_callback_fun_tx),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_TXINT, (void *)(DEV_ENABLED));
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_TXINT, DEV_ENABLED),(%d)\n", ret); return API_ERROR;}
    //rx
    ret = dev_uart_comm->uart_control(UART_CMD_SET_RXINT, (void*)(DEV_DISABLED));
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_RXINT, DEV_DISABLED),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_RXINT_BUF, (void*)NULL);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_RXINT_BUF, NULL),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_RXCB, (void *)uart_callback_fun_rx);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_RXCB, uart_callback_fun_rx),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_RXINT, (void *)(DEV_ENABLED));
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_RXINT, DEV_ENABLED),(%d)\n", ret); return API_ERROR;}
    
    dbg_printf(DBG_LESS_INFO, "app_uart_open() success\n");
    return API_SUCC;
}

int32_t app_uart_close(uint32_t uart_id)
{
    DEV_UART_PTR dev_uart_comm;
    int32_t ret;
    
    if(uart_id < UART_0_ID || uart_id > UART_2_ID)
        return API_ERROR;

    dev_uart_comm = hx_drv_uart_get_dev(uart_id);
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_TXINT, (void *)(DEV_DISABLED));
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_TXINT, DEV_DISABLED),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_RXINT, (void *)(DEV_DISABLED));
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_RXINT, DEV_DISABLED),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_TXCB, NULL);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_TXCB, NULL),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_RXCB, NULL);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_RXCB, NULL),(%d)\n", ret); return API_ERROR;}
    
    ret = dev_uart_comm->uart_control(UART_CMD_SET_ERRCB, NULL);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_control(UART_CMD_SET_ERRCB, NULL),(%d)\n", ret); return API_ERROR;}

    ret = dev_uart_comm->uart_close();
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "uart_close(),(%d)\n", ret); return API_ERROR;}

    ret = hx_drv_uart_deinit(uart_id);
    if(ret<0) {dbg_printf(DBG_LESS_INFO, "hx_drv_uart_deinit(),(%d)\n", ret); return API_ERROR;}
    
    dbg_printf(DBG_LESS_INFO, "app_uart_close() success\n");
    return API_SUCC;
}

int32_t app_uart_read(uint32_t uart_id, uint8_t *buf, uint32_t len)
{
    uint32_t data_avail = 0;
    uint32_t read_size = 0;

    if(uart_id < UART_0_ID || uart_id > UART_2_ID)
    {
        dbg_printf(DBG_LESS_INFO, "app_uart_read() uart_id error\n");
        return -1;
    }
    
    if(uart_id != g_uart_id)
    {
        dbg_printf(DBG_LESS_INFO, "uart_id[%u] != g_uart_id[%u]\n", uart_id, g_uart_id);
        return -1;
    }
    
    if(full)
        data_avail = RX_FIFO_SIZE;
    else if (rx_fifo_tail > rx_fifo_head)
        data_avail = rx_fifo_tail - rx_fifo_head;
    else if (rx_fifo_tail < rx_fifo_head)
        data_avail = (RX_FIFO_SIZE - rx_fifo_head) + rx_fifo_tail;
    else  /*rx_fifo_tail == rx_fifo_head*/
        return 0; /*data_avail == 0*/

    if(data_avail > len)
        read_size = len;
    else
        read_size = data_avail;
    
    if(rx_fifo_head + read_size < RX_FIFO_SIZE)
    {
        memcpy(buf, &rx_fifo[rx_fifo_head], read_size);
        rx_fifo_head += read_size;
    }
    else /*rx_fifo_head + read_size >= RX_FIFO_SIZE*/
    {
        uint32_t sec_1 = RX_FIFO_SIZE - rx_fifo_head;
        uint32_t sec_2 = read_size - sec_1;
        
        memcpy(buf, &rx_fifo[rx_fifo_head], sec_1);
        if(sec_2 > 0)
            memcpy(&buf[sec_1], rx_fifo, sec_2);
        rx_fifo_head = sec_2;
    }

    full = false;
    
    //for (int i = 0; i < read_size; i++)
    //    xprintf("%c", buf[i]);
        
    return read_size;
}

int32_t app_uart_write(uint32_t uart_id, uint8_t *buf, uint32_t len)
{
    DEV_UART_PTR dev_uart_comm;
    int32_t write_size = 0;
    
    if(uart_id < UART_0_ID || uart_id > UART_2_ID)
    {
        dbg_printf(DBG_LESS_INFO, "app_uart_write() uart_id error\n");
        return -1;
    }
    
    if(uart_id != g_uart_id)
    {
        dbg_printf(DBG_LESS_INFO, "uart_id[%u] != g_uart_id[%u]\n", uart_id, g_uart_id);
        return -1;
    }
    
    if(buf == 0 || len == 0)
    {
        dbg_printf(DBG_LESS_INFO, "app_uart_write() buf or len error\n");
        return -1;
    }

    dev_uart_comm = hx_drv_uart_get_dev(uart_id);
    write_size = dev_uart_comm->uart_write((const char *)buf, len);
    
    if(write_size < 0)
    {
        dbg_printf(DBG_LESS_INFO, "uart_write() error\n");
        return -1;
    }
    
    return write_size;
}


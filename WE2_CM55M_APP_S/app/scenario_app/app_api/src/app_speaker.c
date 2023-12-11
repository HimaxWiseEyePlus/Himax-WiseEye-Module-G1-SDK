#include "WE2_debug.h"

#include "hx_drv_iic.h"
#include "hx_drv_timer.h"
#include "hx_drv_i2s.h"
#include "hx_drv_scu_export.h"

#include "app_speaker.h"
#include "app_api.h"

uint32_t g_i2s_tx_addr = 0;
uint32_t g_i2s_tx_size = 0;
uint32_t g_frame_size_samples = 0;
uint8_t g_i2s_tx_busy = 0;

void i2s_tx_callback(uint32_t status)
{
	dbg_printf(DBG_LESS_INFO,"I2S TX CB g_i2s_tx_size: %x \n",g_i2s_tx_size);
    if(g_i2s_tx_size > 0)
    {
        g_i2s_tx_addr = g_i2s_tx_addr + g_frame_size_samples;

        if(g_i2s_tx_size > g_frame_size_samples)
        {
            g_i2s_tx_size -= g_frame_size_samples;
            hx_drv_i2s_int_tx_write((void *) g_i2s_tx_addr, g_frame_size_samples);
            //dbg_printf(DBG_LESS_INFO,"I2S TX CB (Addr:0x%08x, SZ: %x) \n", g_i2s_tx_addr, g_frame_size_samples);
        }
        else
        {
            hx_drv_i2s_int_tx_write((void *) g_i2s_tx_addr, g_i2s_tx_size);
            //dbg_printf(DBG_LESS_INFO,"I2S TX CB (Addr:0x%08x, SZ: %x) \n", g_i2s_tx_addr, g_i2s_tx_size);
            g_i2s_tx_size = 0;
        }
    }
    else
    {
        dbg_printf(DBG_LESS_INFO,"I2S TX CB (Done) \n");
        g_i2s_tx_busy = 0;
    }
    return;
}

void i2s_err_callback(uint32_t status)
{
    dbg_printf(DBG_LESS_INFO,"I2S ERROR CB \n");
    return;
}

void i2s_rx_callback(uint32_t status)
{
    dbg_printf(DBG_LESS_INFO,"I2S RX CB \n");
    return;
}

int32_t app_speaker_init()
{
    USE_DW_IIC_E iic_id = USE_DW_IIC_0;
    
    int32_t retI2C = 0;
    uint8_t dataLen = 1;
    uint8_t regLen = 1;
    uint8_t regAddr[16];
    uint8_t wBuffer[5];
    uint8_t rBuffer[5];

    regAddr[0] = 0x00;
    regAddr[1] = 0x02;
    regAddr[2] = 0x03;
    regAddr[3] = 0x08;
    regAddr[4] = 0x04;

    //readwrite
    regAddr[5] = 0x00;
    regAddr[6] = 0x02;
    regAddr[7] = 0x03;
    regAddr[8] = 0x08;
    regAddr[9] = 0x04;

    wBuffer[0] = 0x61;
    wBuffer[1] = 0x96;
    wBuffer[2] = 0x00;
    wBuffer[3] = 0x19;//0x1F;
    wBuffer[4] = 0x00;
    //readwrite

    rBuffer[0] = 0x00;
    rBuffer[1] = 0x00;
    rBuffer[2] = 0x00;
    rBuffer[3] = 0x00;
    rBuffer[4] = 0x00;

    /*write*/
	retI2C = hx_drv_i2cm_write_data(iic_id, I2S_SLAVE_ADDR, &regAddr[0], regLen, &wBuffer[0], dataLen);
	if(retI2C < E_OK)
	{
		dbg_printf(DBG_LESS_INFO,"[%s() - Line.%d] setRegisters I2C fail (%d)\n", __FUNCTION__, __LINE__, retI2C);
	}

	retI2C = hx_drv_i2cm_write_data(iic_id, I2S_SLAVE_ADDR, &regAddr[1], regLen, &wBuffer[1], dataLen);
	if(retI2C < E_OK)
	{
		dbg_printf(DBG_LESS_INFO,"[%s() - Line.%d] setRegisters I2C fail (%d)\n", __FUNCTION__, __LINE__, retI2C);
	}

	retI2C = hx_drv_i2cm_write_data(iic_id, I2S_SLAVE_ADDR, &regAddr[2], regLen, &wBuffer[2], dataLen);
	if(retI2C < E_OK)
	{
		dbg_printf(DBG_LESS_INFO,"[%s() - Line.%d] setRegisters I2C fail (%d)\n", __FUNCTION__, __LINE__, retI2C);
	}

	retI2C = hx_drv_i2cm_write_data(iic_id, I2S_SLAVE_ADDR, &regAddr[3], regLen, &wBuffer[3], dataLen);
	if(retI2C < E_OK)
	{
		dbg_printf(DBG_LESS_INFO,"[%s() - Line.%d] setRegisters I2C fail (%d)\n", __FUNCTION__, __LINE__, retI2C);
	}

	retI2C = hx_drv_i2cm_write_data(iic_id, I2S_SLAVE_ADDR, &regAddr[4], regLen, &wBuffer[4], dataLen);
	if(retI2C < E_OK)
	{
		dbg_printf(DBG_LESS_INFO,"[%s() - Line.%d] setRegisters I2C fail (%d)\n", __FUNCTION__, __LINE__, retI2C);
	}

	board_delay_ms(10);

	hx_drv_i2cm_write_restart_read(iic_id ,I2S_SLAVE_ADDR ,&regAddr[5] , 1, &rBuffer[0], 1);
	hx_drv_i2cm_write_restart_read(iic_id ,I2S_SLAVE_ADDR ,&regAddr[6] , 1, &rBuffer[1], 1);
	hx_drv_i2cm_write_restart_read(iic_id ,I2S_SLAVE_ADDR ,&regAddr[7] , 1, &rBuffer[2], 1);
	hx_drv_i2cm_write_restart_read(iic_id ,I2S_SLAVE_ADDR ,&regAddr[8] , 1, &rBuffer[3], 1);
	hx_drv_i2cm_write_restart_read(iic_id ,I2S_SLAVE_ADDR ,&regAddr[9] , 1, &rBuffer[4], 1);
	dbg_printf(DBG_LESS_INFO,"\nData Read: 0x%02x%02x%02x%02x%02x\n", rBuffer[0], rBuffer[1], rBuffer[2], rBuffer[3],rBuffer[4]);

	return API_SUCC;
}

int32_t app_speaker_i2s_init()
{
	int32_t retI2S = 0;

	hx_drv_i2s_init(BASE_ADDR_I2S_HOST, I2S_AUD_DATA_16B);
	hx_drv_i2s_set_pcm_freq(I2S_AUD_SR_16K);
    
	retI2S = hx_drv_i2s_register_cb(i2s_tx_callback, i2s_rx_callback, i2s_err_callback);
	if(retI2S < E_OK)
	{
		dbg_printf(DBG_LESS_INFO,"hx_drv_i2s_register_cb fail\r\n");
	}

	return API_SUCC;
}

int32_t app_speaker_output(uint32_t i2s_tx_addr, uint32_t i2s_tx_size, uint32_t frame_size_samples)
{
    if(g_i2s_tx_busy == 0)
    {
        g_i2s_tx_busy = 1;
        
        g_i2s_tx_addr = i2s_tx_addr;
    	g_frame_size_samples = frame_size_samples;
    	g_i2s_tx_size = i2s_tx_size;
    	if(g_i2s_tx_size > frame_size_samples)
    	    g_i2s_tx_size -= frame_size_samples;
    	else
    	    g_i2s_tx_size = 0;
    	
        //SCB_InvalidateDCache();		// PCM data load from IDE, need to invalidate D Cache
    	hx_drv_i2s_int_tx_write((void *) i2s_tx_addr, frame_size_samples);
		//hx_drv_i2s_tx_write((void *) i2s_tx_addr, i2s_tx_size);

    	return API_SUCC;
    }
    else
    {
        return API_ERROR;
    }
}

int32_t app_speaker_get_tx_status(void)
{
	return g_i2s_tx_busy;
}

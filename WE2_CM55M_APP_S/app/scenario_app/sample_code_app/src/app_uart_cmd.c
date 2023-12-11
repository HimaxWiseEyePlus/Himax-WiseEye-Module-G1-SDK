#include "WE2_debug.h"
#include "hx_drv_swreg_aon_export.h"
#include "hx_drv_swreg_aon.h"

#include "app_uart.h"
#include "app_uart_cmd.h"

#define UART_OTA_HEADER_SIZE     5

/* Flag Byte */
#define UART_OTA_FLAG_OFFSET   0
#define UART_OTA_FLAG_SIZE     1	
/* Feature Byte */
#define UART_OTA_FEATURE_OFFSET   1
#define UART_OTA_FEATURE_SIZE     1
/* Command Byte */
#define UART_OTA_COMMAND_OFFSET   2
#define UART_OTA_COMMAND_SIZE     1
/* Payload Length Bytes */
#define UART_OTA_PAYLOADLEN_LSB_OFFSET    3
#define UART_OTA_PAYLOADLEN_MSB_OFFSET    4
#define UART_OTA_PAYLOADLEN_SIZE          2
	
/* Payload */
#define UART_OTA_PAYLOAD_OFFSET          5


/**
 * \enum UARTCOMM_FEATURE_E
 * \brief this enumeration use in uart communication library, define the supported feature.
 */
typedef enum
{
	UARTCOMM_FEATURE_OTA_OP	     	  	= 0x50,
	UARTCOMM_FEATURE_OTA_UPG     	  	= 0x51,	
	UARTCOMM_FEATURE_MAX
} UARTCOMM_FEATURE_E;


/**
 * \enum UARTCOMM_OTA_CMD_E
 * \brief this enumeration use in i2c communication library, define the supported OTA command.
 */
typedef enum
{
    UARTCOMM_CMD_OTA_GET_VER         = 0x01,
    UARTCOMM_CMD_OTA_READ_CHIP_ID    = 0x02,
    UARTCOMM_CMD_OTA_READ_STATUS     = 0x03, 
    UARTCOMM_CMD_OTA_REBOOT          = 0x04,
    UARTCOMM_CMD_OTA_BUSY_PIN_SELECT = 0x05,
    UARTCOMM_CMD_OTA_READ_BUSY_PIN   = 0x06,
    UARTCOMM_CMD_OTA_JUMP2UPG        = 0x08,
    UARTCOMM_CMD_OTA_START           = 0x09,
    UARTCOMM_CMD_OTA_DATA            = 0x0A,
    UARTCOMM_CMD_OTA_END             = 0x0B,
    UARTCOMM_CMD_OTA_MAX,
} UARTCOMM_OTA_CMD_E;

typedef void (*funcptr_void)(void);

void app_uart_cmd_polling(uint32_t uart_id)
{
    uint8_t feature_type = 0, cmd_id = 0;
    uint16_t payload_len = 0;
    uint8_t header_buf[UART_OTA_HEADER_SIZE] = {0};
    int32_t read_len;

    read_len = app_uart_read(uart_id, header_buf, UART_OTA_HEADER_SIZE);
#if 0
    int i;
    for (i = 0; i < UART_OTA_HEADER_SIZE; i++) {
        dbg_printf(DBG_LESS_INFO, "header_buf[%d] = %02X\n", i, header_buf[i]);
    }
#endif

    if (UART_OTA_HEADER_SIZE == read_len)
    {
        feature_type = header_buf[UART_OTA_FEATURE_OFFSET];
        cmd_id = header_buf[UART_OTA_COMMAND_OFFSET];
        payload_len = header_buf[UART_OTA_PAYLOADLEN_LSB_OFFSET] | (header_buf[UART_OTA_PAYLOADLEN_MSB_OFFSET]<<8);

        dbg_printf(DBG_LESS_INFO, "[MSG] feature_type: 0x%x \n", feature_type);
        dbg_printf(DBG_LESS_INFO, "[MSG] cmd_id: 0x%x \n", cmd_id);
        dbg_printf(DBG_LESS_INFO, "[MSG] payload_len: 0x%x \n", payload_len);

        switch (feature_type)//Any modify need sync with i2ccomm_task
        {
            case UARTCOMM_FEATURE_OTA_OP:
                if(cmd_id == UARTCOMM_CMD_OTA_JUMP2UPG)
                {
                    funcptr_void ResetHandler;
                    
                    dbg_printf(DBG_LESS_INFO, "===UARTCOMM_CMD_OTA_JUMP2UPG===\n");
                    /* jump to 2ndloader */
                    hx_drv_swreg_aon_set_ota_flag(SWREG_AON_OTA_YES_FLAG);

                    ResetHandler = (funcptr_void)(*((uint32_t *)((BOOTLOADER_BASE) + 4U)));
                    ResetHandler();
                }
                break;
            default:
                dbg_printf(DBG_LESS_INFO, "uart_cmd: feature id 0x%02x is not registered!\n", feature_type);
                break;
        }
        
    }
}


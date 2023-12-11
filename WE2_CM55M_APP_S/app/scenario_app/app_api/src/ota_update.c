#include "i2c_comm.h"
#include "WE2_debug.h"
#include "app_i2c_cmd_def.h"
#include "app_i2c_cmd.h"
#include "ota_update.h"

#include "hx_drv_swreg_aon_export.h"
#include "hx_drv_swreg_aon.h"

I2CCOMM_ERROR_E i2ccomm_cmd_process_ota_flow(uint8_t *read_buf)
{
    I2CCOMM_ERROR_E retval = I2CCOMM_NO_ERROR;
    uint8_t cmd;

    cmd = read_buf[I2CFMT_COMMAND_OFFSET];

    retval = hx_lib_i2ccomm_validate_checksum(read_buf);

    if (retval != I2CCOMM_NO_ERROR)
    {
        return I2CCOMM_ERROR_CHECKSUM;
    }

    switch (cmd)
    {
        case I2CCOMM_CMD_OTA_JUMP2UPG:
            {
                funcptr_void ResetHandler;
                
                dbg_printf(DBG_LESS_INFO, "===I2CCOMM_CMD_OTA_JUMP2UPG===\n");            
                /* jump to 2ndloader */
                
                hx_drv_swreg_aon_set_ota_flag(SWREG_AON_OTA_YES_FLAG);
                
    	        ResetHandler = (funcptr_void)(*((uint32_t *)((BOOTLOADER_BASE) + 4U)));
    	        ResetHandler();
            }
            break;
        default:
            break;
    }

    return I2CCOMM_NO_ERROR;
}

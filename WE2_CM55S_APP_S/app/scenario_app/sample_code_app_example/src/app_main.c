#include "board.h"
#include "WE2_debug.h"
#include "i2c_comm.h"
#include "hx_drv_scu_export.h"
#include "app_i2c_cmd.h"

void app_main()
{
    /*uart pin mux to transmit debug msg. conflict with CM55M spi master/slave.*/
    #if 0
    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_UART1_RTS);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_UART1_CTS);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_UART1_RX);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_UART1_TX);
    #endif

    hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_UART1_RX);
    hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_UART1_TX);
    
    dbg_printf(DBG_LESS_INFO, "CM55S sample_code_app_dualcore_s\n");

    //app_i2c_cmd_init(USE_DW_IIC_SLV_0);
    
    while(1)
    {
        //app_i2c_cmd_polling(USE_DW_IIC_SLV_0);
        dbg_printf(DBG_LESS_INFO, ".");
        board_delay_ms(1000);
    }
}


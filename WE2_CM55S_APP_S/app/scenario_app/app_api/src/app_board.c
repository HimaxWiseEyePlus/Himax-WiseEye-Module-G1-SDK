#include "hx_drv_scu_export.h"
#include "hx_drv_scu.h"
#include "hx_drv_gpio.h"

void app_board_pinmux_settings(void)
{
    #if defined(IC_PACKAGE_WLCSP65)
    #ifdef SPI_MASTER_SEND /*spi master*/
    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1);
    #endif

    #ifdef SPI_SLAVE_SEND /*slave*/
    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_S_DO);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_S_DI);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_S_CLK);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_S_CS);
    
    hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0);
    hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_LOW);

    //hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_GPIO0);
    //hx_drv_gpio_set_output(GPIO0, GPIO_OUT_LOW);
    #endif
    #elif defined(IC_PACKAGE_LQFP128)
    #ifdef SPI_MASTER_SEND /*spi master*/
    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1);
    #endif
	
    #ifdef SPI_SLAVE_SEND /*slave*/
    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_S_DO);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_S_DI);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_S_CLK);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_S_CS);
    
    hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0);
    hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_LOW);

    //hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_GPIO0);
    //hx_drv_gpio_set_output(GPIO0, GPIO_OUT_LOW);
    #endif
    #endif
}


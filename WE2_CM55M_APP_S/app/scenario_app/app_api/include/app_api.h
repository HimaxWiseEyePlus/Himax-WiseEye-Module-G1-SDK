/**
 * @file app_api.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_API_H__
#define __APP_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

#include "hx_drv_gpio.h"

#include "dev_common.h"
#include "spi_protocol.h"
//#include "powermode.h"

#ifdef SPI_MASTER_SEND
#include "app_spi_m.h"
#endif
#ifdef SPI_SLAVE_SEND
#include "app_spi_s.h"
#endif
#ifdef UART_PROTOCOL
#include "app_uart.h"
#endif

#define API_SUCC            (0)     /**< APP API SUCCESS*/
#define API_ERROR           (-1)    /**< APP API ERROR*/
#define API_INVALID_ID      (-2)    /**< APP API INVALID ID*/
#define API_INVALID_ADDR    (-3)    /**< APP API INVALID ADDRESS*/
#define API_INVALID_SIZE    (-4)    /**< APP API INVALID SIZE*/
#define API_INVALID_DATA    (-5)    /**< APP API INVALID DATA*/

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

typedef struct{
    uint8_t *pY;
    uint8_t *pU;
    uint8_t *pV;
}app_yuv_planar_t;

typedef struct
{
    #ifdef SPI_MASTER_SEND /*master*/
    uint8_t mst_id; /**< id*/
    uint32_t mst_freq;  /**< frequency*/
    #endif
    
    #ifdef SPI_SLAVE_SEND /*slave*/
    uint8_t slv_handshake_pin;         /**< handshake gpio pin number*/
    uint8_t slv_handshake_actv_lvl;    /**< handshake gpio pin active level*/
    #endif
}APP_SPI_CFG_T;

/* SPI */
/**
 * \brief	app spi interface initialization
 *
 * \param[in]	spi_cfg	        set spi configuration
 * \return	app api error code
 */
int32_t app_spi_init(APP_SPI_CFG_T *spi_cfg);

/**
 * \brief	app spi master write data
 *
 * \param[in]	addr	        specified write data start address
 * \param[in]	size	        specified write data size
 * \param[in]	data_type	    specified write data type
 * \return	app api error code
 */
int32_t app_spi_write(uint32_t addr, uint32_t size, SPI_CMD_DATA_TYPE data_type);


/**
 * \brief	app uart write data
 *
 * \param[in]	addr	        specified write data start address
 * \param[in]	size	        specified write data size
 * \param[in]	data_type	    specified write data type
 * \return	app api error code
 */

int32_t app_uart_tx(uint32_t addr, uint32_t size, SPI_CMD_DATA_TYPE data_type);

/* GPIO */
/**
 * \enum HXGPIO
 * \brief gpio pin enum
 */
typedef enum
{
    /*GPIO*/
    HX_GPIO_0 = GPIO0,     /**< GPIO 0*/
    HX_GPIO_1 = GPIO1,     /**< GPIO 1*/
    HX_GPIO_2 = GPIO2,     /**< GPIO 2*/
    HX_GPIO_3 = GPIO3,     /**< GPIO 3*/
    HX_GPIO_4 = GPIO4,     /**< GPIO 4*/
    HX_GPIO_5 = GPIO5,     /**< GPIO 5*/
    HX_GPIO_6 = GPIO6,     /**< GPIO 6*/
    HX_GPIO_7 = GPIO7,     /**< GPIO 7*/
    HX_GPIO_8 = GPIO8,     /**< GPIO 8*/
    HX_GPIO_9 = GPIO9,     /**< GPIO 9*/
    HX_GPIO_10 = GPIO10,     /**< GPIO 10*/
    HX_GPIO_11 = GPIO11,     /**< GPIO 11*/
    HX_GPIO_12 = GPIO12,     /**< GPIO 12*/
    HX_GPIO_13 = GPIO13,     /**< GPIO 13*/
    HX_GPIO_14 = GPIO14,      /**< GPIO 14*/
    HX_GPIO_15 = GPIO15,     /**< GPIO 15*/
    HX_GPIO_16 = GPIO16,     /**< GPIO 16*/
    HX_GPIO_17 = GPIO17,     /**< GPIO 17*/
    HX_GPIO_18 = GPIO18,     /**< GPIO 18*/
    HX_GPIO_19 = GPIO19,     /**< GPIO 19*/
    HX_GPIO_20 = GPIO20,     /**< GPIO 20*/
    HX_GPIO_21 = GPIO21,     /**< GPIO 21*/
    HX_GPIO_22 = GPIO22,     /**< GPIO 22*/
    HX_GPIO_23 = GPIO23,     /**< GPIO 23*/
    HX_GPIO_24 = GPIO24,     /**< GPIO 24*/
    HX_GPIO_25 = GPIO25,     /**< GPIO 25*/
    HX_GPIO_26 = GPIO26,     /**< GPIO 26*/
    HX_GPIO_27 = GPIO27,     /**< GPIO 27*/
    HX_GPIO_28 = GPIO28,     /**< GPIO 28*/
    HX_GPIO_29 = GPIO29,     /**< GPIO 29*/
    /*SB GPIO*/
    HX_SB_GPIO_0 = SB_GPIO0,     /**< SB GPIO 0*/
    HX_SB_GPIO_1 = SB_GPIO1,     /**< SB GPIO 1*/
    /*AON GPIO*/
    HX_AON_GPIO_0 = AON_GPIO0,     /**< AON GPIO 0*/
    HX_AON_GPIO_1 = AON_GPIO1,     /**< AON GPIO 1*/
    HX_AON_GPIO_2 = AON_GPIO2,      /**< AON GPIO 2*/
    /*GPIO MAX*/
    HX_GPIO_MAX,
    /*GPIO INVALID*/
    HX_GPIO_INVAL = 0xFF      /**< GPIO invalid*/
} HXGPIO;

/**
 * \enum HXGPIO_MODE
 * \brief gpio pin direction mode enum
 */
typedef enum
{
    HX_GPIO_INPUT = GPIO_DIR_INPUT,  /**< GPIO direction input*/
    HX_GPIO_OUTPUT = GPIO_DIR_OUTPUT  /**< GPIO direction output*/
} HXGPIO_MODE;

/**
 * \enum HXGPIO_VALUE
 * \brief gpio pin level value enum
 */
typedef enum
{
    HX_GPIO_LOW = GPIO_OUT_LOW,   /**< GPIO output level low*/
    HX_GPIO_HIGH = GPIO_OUT_HIGH    /**< GPIO output level high*/
} HXGPIO_VALUE;

typedef enum
{
    HX_GPIO_IRQ_TRIG_TYPE_EDGE_RISING = GPIO_IRQ_TRIG_TYPE_EDGE_RISING,
    HX_GPIO_IRQ_TRIG_TYPE_EDGE_FALLING = GPIO_IRQ_TRIG_TYPE_EDGE_FALLING,
    HX_GPIO_IRQ_TRIG_TYPE_EDGE_BOTH = GPIO_IRQ_TRIG_TYPE_EDGE_BOTH,
    HX_GPIO_IRQ_TRIG_TYPE_LEVEL_HIGH = GPIO_IRQ_TRIG_TYPE_LEVEL_HIGH,
    HX_GPIO_IRQ_TRIG_TYPE_LEVEL_LOW = GPIO_IRQ_TRIG_TYPE_LEVEL_LOW
}HXGPIO_IRQ_TRIG_TYPE;

/**
 * \brief	GPIO interrupt callback function
 *
 * \return	void.
 */
typedef void (*HXGPIO_CB_T) (uint8_t group, uint8_t idx);

/**
 * \brief	set GPIO direction mode
 *
 * \param[in]	gpio_num	    specified gpio pin number
 * \param[in]	mode	        set gpio pin direction
 * \param[in]	def_lvl	        set gpio pin default level
 * \return	0:success -1:fail.
 */
int8_t app_gpio_set_mode(HXGPIO gpio_num, HXGPIO_MODE mode, HXGPIO_VALUE def_lvl);

/**
 * \brief	set GPIO output level
 *
 * \param[in]	gpio_num	    specified gpio pin number
 * \param[in]	out_value	    set gpio pin output level
 * \return	0:success -1:fail.
 */
int8_t app_gpio_set_value(HXGPIO gpio_num, HXGPIO_VALUE out_value);

/**
 * \brief	get GPIO input level
 *
 * \param[in]	gpio_num	    specified gpio pin number
 * \param[out]	*in_value	    get gpio pin input level
 * \return	0:success -1:fail.
 */
int8_t app_gpio_get_value(HXGPIO gpio_num, uint8_t *in_value);

void app_gpio_intr_register(HXGPIO gpio_num, HXGPIO_CB_T cb_fun, HXGPIO_IRQ_TRIG_TYPE trig_type);
void app_gpio_intr_unregister(HXGPIO gpio_num);


/* LED */
/**
 * \brief	led gpio initialization
 *
 * \return	void.
 */
void app_led_init();

/**
 * \brief	green led light on/off
 *
 * \param[in]	on      set green led on/off
 * \return  void.
 */
void app_led_green(uint8_t on);

/**
 * \brief	blue led light on/off
 *
 * \param[in]	on	    set blue led on/off
 * \return	void.
 */
void app_led_blue(uint8_t on);

/*Timer*/
/**
 * \brief	get current system time stamp in ms
 *
 * \return	current system time stamp in ms.
 */
uint32_t app_timer_get_current_time_ms();

/**
 * \brief	delay time in ms
 *
 * \return  0:success.
 */
int8_t app_delay_ms(uint32_t delay_ms);


#ifdef __cplusplus
}
#endif

#endif


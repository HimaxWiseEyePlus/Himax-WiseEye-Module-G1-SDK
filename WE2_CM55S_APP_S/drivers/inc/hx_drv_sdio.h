/*
 * hx_drv_sdio.h
 *
 *  Created on: 2021¦~9¤ë13¤é
 *      Author: 902449
 */

#ifndef DRIVERS_INC_HX_DRV_SDIO_H_
#define DRIVERS_INC_HX_DRV_SDIO_H_

#include <stdio.h>
#include <stdlib.h>

/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/
/**
 * \enum SDIO_ERROR_E
 * \brief SDIO Errors Type
 */
typedef enum {
	SDIO_PASS = 0, /**< No ERROR */
	SDIO_ERR_INVALID_PARA, /**< ERROR MSG: Invalid parameters */
	SDIO_ERR_FAIL, /**< ERROR MSG: Operation fail*/
} SDIO_ERROR_E;

typedef enum
{
    /** access mode - SDR12 default (CLK: max 25MHz, DT: max 12MB/s) */
    SDIO_ACCESS_MODE_SDR12 = 0U,
    /** access mode - SDR15 default (CLK: max 50MHz, DT: max 25MB/s) */
	SDIO_ACCESS_MODE_SDR25 = 1U,
    /** access mode - SDR50 default (CLK: max 100MHz, DT: max 50MB/s) */
	SDIO_ACCESS_MODE_SDR50 = 2U,
    /** access mode - SDR104 default (CLK: max 208MHz, DT: max 104MB/s) */
	SDIO_ACCESS_MODE_SDR104 = 3U,
	SDIO_ACCESS_MODE_MAXIMUM,

} SDIO_SpeedMode;

/****************************************************
 * Structure Definition                             *
 ***************************************************/

/****************************************************
 * Function Declaration                             *
 ***************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	SDIO card initialization
 *
 * \param[in] clk				clk value in Khz, you can set it to 1000Khz as default.
 * \param[in] bus_width			bus width of sdio, CSDD_BUS_WIDTH_1 = 1U, CSDD_BUS_WIDTH_4 = 4U, CSDD_BUS_WIDTH_8 = 8U, CSDD_BUS_WIDTH_4_DDR = 5U, CSDD_BUS_WIDTH_8_DDR = 6U
 * 								you can set bus_width to 1 as default.
 * \param[in] sdio_base_addr	sdio base address for further register control, it is need in driver level.
 * 								there is base address in WE2_device_addr.h - SD_REG_BASE for reference.
 *
 * \retval	SDIO_PASS			Operation success
 * \retval	SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_init(unsigned int clk, unsigned int bus_width, unsigned int sdio_base_addr);

/**
 * \brief	read data from SD memory card with given condition, data_size should be smaller than a sector size
 *
 * \param[in] sector_number		sector index to retrieve data
 * \param[in] data_size			data size in bytes to read, size should smaller than 512
 * \param[out] buffer			data array pointer
 *
 * \retval	SDIO_PASS				Operation success
 * \retval	SDIO_ERR_INVALID_PARA	sector size error
 * \retval	SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_mem_read(unsigned int sector_number, unsigned int data_size, unsigned char *buffer);

/**
 * \brief	write data to SD memory card with given condition, data_size should be smaller than a sector size
 *
 * \param[in] sector_number		sector index to write data
 * \param[in] data_size			data size in bytes to write, size should smaller than 512
 * \param[in] buffer			data array pointer
 *
 * \retval	SDIO_PASS				Operation success
 * \retval	SDIO_ERR_INVALID_PARA	sector size error
 * \retval	SDIO_ERR_FAIL		Operation fail
 */
extern SDIO_ERROR_E hx_drv_sdio_mem_write(unsigned int sector_number, unsigned int data_size, unsigned char *buffer);

extern SDIO_ERROR_E hx_drv_sdio_cccr_read(unsigned int addr, unsigned char *data, unsigned char sz);

extern SDIO_ERROR_E hx_drv_sdio_cccr_write(unsigned int addr, unsigned char *data, unsigned char sz);

/**
 * \brief	SDIO card change mode
 *
 * \param[in] mode			SD card mode,
 * 							0:CSDD_ACCESS_MODE_SDR12 /1:CSDD_ACCESS_MODE_SDR25
 * 							2:CSDD_ACCESS_MODE_SDR50 /3:CSDD_ACCESS_MODE_SDR104
 *
 * \param[in] clk			clk value in Khz, you can set it to 1000Khz as default.
 *
 * \retval	SDIO_PASS		Operation success
 * \retval	SDIO_ERR_FAIL	Operation fail
*/
extern SDIO_ERROR_E hx_drv_sdio_set_mode(SDIO_SpeedMode mode, unsigned int clk);
#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_INC_HX_DRV_SDIO_H_ */

#ifndef LIBRARY_SPI_EEPROM_COMM_H_
#define LIBRARY_SPI_EEPROM_COMM_H_

#include "dev_common.h"
#include "hx_drv_dmac.h"
#include "hx_drv_spi.h"
#include "WE2_core.h"
#include "cachel1_armv7.h"

#ifdef NSC
#include "veneer_clk_ctrl.h"
#else
#include "hx_drv_scu.h"
#endif

/** 
 * \defgroup SPI EEPRM library
 * \brief This library is based on Winbond W25Q128JW datasheet
 */


#define SPI_EEPROM_XIP_RELATED
#define SPI_EEPROM_WRITE_RELATED
#define SPI_EEPROM_READ_RELATED
#define SPI_EEPROM_WORD_W_R_RELATED
#define SPI_EEPROM_CHIP_ERASE_ERLATED
#define SPI_EEPROM_SECTOR_ERASE_RELATED

/**
 * \note if defined this flag, all functions will to set this quad enable bit first
 */
//#define XIP_SET_EEPROM_QUAD_MODE_BIT

#define LIB_SPI_EEPROM_USE_DMA

#ifdef BOOTROM
#undef SPI_EEPROM_WRITE_RELATED
#undef SPI_EEPROM_READ_RELATED
#undef SPI_EEPROM_WORD_W_R_RELATED
#undef SPI_EEPROM_CHIP_ERASE_ERLATED
#undef SPI_EEPROM_SECTOR_ERASE_RELATED
#undef XIP_SET_EEPROM_QUAD_MODE_BIT
#undef LIB_SPI_EEPROM_USE_DMA
#endif

#ifdef BOOTLOADER
#undef SPI_EEPROM_READ_RELATED
#undef SPI_EEPROM_WORD_W_R_RELATED
#undef SPI_EEPROM_CHIP_ERASE_ERLATED
#endif

#ifdef SECONDBOOTLOADER
#undef SPI_EEPROM_READ_RELATED
#undef SPI_EEPROM_WORD_W_R_RELATED
#endif

// only support OSPI QSPI
typedef void (*spi_eeprom_cb_t)(void);

#define DW_SPI_SPI_EEPROM_WINBOND_CH0 0xEF
#define DW_SPI_SPI_EEPROM_WINBOND_CH1 0x60
#define DW_SPI_SPI_EEPROM_WINBOND33V_CH1 0x40

#define DW_SPI_SPI_EEPROM_MXIC_CH0 0xC2
#define DW_SPI_SPI_EEPROM_GIGADEV_CH0 0xC8

#define DW_SPI_FLASH_WIP_POS 0
#define DW_SPI_FLASH_WRITE_ENABLE_POS 1
#define DW_SPI_MXIC_FLASH_QUAD_ENABLE_MASK 0x40
#define DW_SPI_MXIC_FLASH_QUAD_ENABLE_POS 6
#define DW_SPI_WINBOND_FLASH_QUAD_ENABLE_MASK 0x02
#define DW_SPI_WINBOND_FLASH_QUAD_ENABLE_POS 1

#define DW_SPI_FLASH_CMD_WRITE_STATUS 0x01
#define DW_SPI_FLASH_CMD_WRITE_STATUS2 0x31
#define DW_SPI_FLASH_CMD_VOLATILE_WRITE 0x50
#define DW_SPI_FLASH_CMD_SINGLE_WRITE 0x02
#define DW_SPI_FLASH_CMD_1X_ADDR_QUAD_WRITE 0x32
#define DW_SPI_FLASH_CMD_QUAD_WRITE 0x38
#define DW_SPI_FLASH_CMD_SINGLE_READ 0x03
#define DW_SPI_FLASH_CMD_DUAL_READ_SINGLE_ADDR 0x3B
#define DW_SPI_FLASH_CMD_DUAL_READ 0xBB
#define DW_SPI_FLASH_CMD_QUAD_READ 0xEB
#define DW_SPI_FLASH_CMD_STATUS_CHECK 0x05
#define DW_SPI_FLASH_CMD_STATUS2_CHECK 0x35
#define DW_SPI_FLASH_CMD_WRITE_ENABLE 0x06
#define DW_SPI_FLASH_CMD_ERASE_ALL 0x60
#define DW_SPI_FLASH_CMD_ERASE_SECTOR			0x20
#define DW_SPI_FLASH_CMD_ERASE_32KB				0x52
#define DW_SPI_FLASH_CMD_ERASE_64KB				0xD8
#define DW_SPI_FLASH_CMD_FLASH_INFO 0x9F
#define DW_SPI_FLASH_MXIC_PROTECT_MASK 0xBC
#define DW_SPI_FLASH_WINBOND_PROTECT_MASK 0xFC
#define DW_SPI_FLASH_WINBOND_PROTECT2_MASK 0x79
#define DW_SPI_FLASH_GIGADEV_PROTECT_MASK 0x9C

typedef enum CUR_FLASH_TYPE_S
{
    FLASH_TYPE_NONE = 0,
    FLASH_TYPE_MXIC,
    FLASH_TYPE_WINBOND,
    FLASH_TYPE_WINBOND_33V,
    FLASH_TYPE_GIGADEV,
} CUR_FLASH_TYPE_E;

typedef enum {
	FLASH_SECTOR = 0,
	FLASH_32KBLOCK,
	FLASH_64KBLOCK,
}FLASH_ERASE_SIZE_E;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initial QSPI/OSPI master for EEPROM, default speed is max 
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_open(USE_DW_SPI_MST_E spi_id);

/**
 * @brief initial QSPI/OSPI master for EEPROM, speed of spi is seted by user
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param clk_hz the output clock of SPI
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_open_speed(USE_DW_SPI_MST_E spi_id, uint32_t clk_hz);

#ifdef SPI_EEPROM_READ_RELATED
/**
 * @brief read eeprom id information
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param id_info Pointer to the buffer where the received EEPROM Device Identification  will be stored.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_read_ID(USE_DW_SPI_MST_E spi_id, uint8_t *id_info);

/**
 * @brief read eeprom using single mode with address, length information, data frame size= 1 btye
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param flash_addr  address of EEPROM 
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param len The length of the data that will be received from the EEPROM.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_1read(USE_DW_SPI_MST_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);
/**
 * @brief read eeprom using dual mode with address, length information, data frame size= 1 btye
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param flash_addr  address of EEPROM 
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param len The length of the data that will be received from the EEPROM.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_2read(USE_DW_SPI_MST_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);
/**
 * @brief read eeprom using quad mode with address, length information, data frame size= 1 btye
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param flash_addr  address of EEPROM 
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param len The length of the data that will be received from the EEPROM.
 * @return int32_t an error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_4read(USE_DW_SPI_MST_E spi_id, uint32_t flash_addr, uint8_t *data, uint32_t len);
#endif

#ifdef SPI_EEPROM_WORD_W_R_RELATED

/**
 * @brief word read eeprom using quad mode with address, length information, data frame size= 4 btye
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param addr  address of EEPROM 
 * @param data Pointer to the buffer where the received data from EEPROM will be stored
 * @param bytes_len The length of the data that will be received from the EEPROM.  \note bytes_len must be a multiple of 4.
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_word_read(USE_DW_SPI_MST_E spi_id, uint32_t addr, uint32_t *data, uint32_t bytes_len);

/**
 * @brief word write eeprom with address, length information
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param addr Address of EEPROM 
 * @param data Pointer to the data buffer to be written to the EEPROM device
 * @param bytes_len The length of the data that will be written to the EEPROM.  \note bytes_len must be a multiple of 4.
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_word_write(USE_DW_SPI_MST_E spi_id, uint32_t addr, uint32_t *data, uint32_t bytes_len);
#endif

#ifdef SPI_EEPROM_WRITE_RELATED
/**
 * @brief  write eeprom with address, length information. word_switch-when storage data in waord type, 
 *      change word_switch to true for little endian/big endian switch
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param addr Address of EEPROM 
 * @param data Pointer to the data buffer to be written to EEROM device
 * @param len The length of the data that will be writed from the EEPROM.
 * @param word_switch little endian/big endian switch.  
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_write(USE_DW_SPI_MST_E spi_id, uint32_t addr, uint8_t *data, uint32_t len,
                                        uint8_t word_switch);
/**
 * @brief send op command to select eeprom
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param data  Pointer to the data buffer to be written to EEROM device
 *      if OSPI is selected, buffer size is limited  to 256 bytes.
 * @param len The length of the data that will be writed from the EEPROM.
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_Send_Op_code(USE_DW_SPI_MST_E spi_id, uint8_t *data, uint32_t len);
#endif

#ifdef SPI_EEPROM_CHIP_ERASE_ERLATED
/**
 * @brief chip erase select eeprom
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_erase_all(USE_DW_SPI_MST_E spi_id);
#endif
#ifdef SPI_EEPROM_SECTOR_ERASE_RELATED
/**
 * @brief sector erase within a specified block by given flash address
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param addr Address of EEPROM 
 * @param sz Type of specified block.
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_erase_sector(USE_DW_SPI_MST_E spi_id, uint32_t addr,  FLASH_ERASE_SIZE_E sz);
#endif

#ifdef SPI_EEPROM_XIP_RELATED
/**
 * @brief set select eeprom to specific XIP mode
 * 
 * @param spi_id ID of spi, QSPI/OSPI
 * @param xip_enable set enable/disable XIP mode
 * @param xip_mode set XIP mode. dual or quad.
 * @param xip_cont set enable/disable continuous transfer in XIP mode
 * @return int32_t An error code \headerfile "dev_common.h"
 */
extern int32_t hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_E spi_id, bool xip_enable,
                                            FLASH_ACCESS_MODE_E xip_mode, bool xip_cont);
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBRARY_SPI_EEPROM_COMM_H_*/

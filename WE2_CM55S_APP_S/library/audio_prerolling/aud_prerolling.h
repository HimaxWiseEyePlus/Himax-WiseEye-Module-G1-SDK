/**
 ********************************************************************************************
 *  @file      aud_prerolling.h
 *  @details   This file contains PDM audio library related function
 *  @author    himax/902449
 *  @version   V1.0.0
 *  @date      28-Dec-2021
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/

#ifndef LIBRARY_AUDIO_PREROLLING_LIB_H_
#define LIBRARY_AUDIO_PREROLLING_LIB_H_

#include <stdint.h>
#include <stdbool.h>
#include "hx_drv_pdm_rx.h"

/****************************************************
 * ENUM Declaration                                 *
 ***************************************************/

/**
 * \enum HX_LIB_AUDIO_ERROR_E
 * \brief AUDIO API ERROR Code
 */
typedef enum {
    HX_LIB_AUDIO_NO_ERROR = 0,                 /**< No ERROR */
    HX_LIB_AUDIO_ERROR_NOT_INIT = 1,           /**< ERROR MSG: not init */
    HX_LIB_AUDIO_ERROR_DRIVER = 2,             /**< ERROR MSG: audio driver error */
    HX_LIB_AUDIO_ERROR_ALREADY_INIT = 3,       /**< ERROR MSG: already init */
    HX_LIB_AUDIO_ERROR_INVALID_PARAMETERS = 4, /**< ERROR MSG: Invalid parameters */
    HX_LIB_AUDIO_ERROR_NULL_POINTER = 5,       /**< ERROR MSG: NULL pointer */
    HX_LIB_AUDIO_TIMEOUT_ERROR = 6,            /**< ERROR MSG: timeout happen during progress */
    HX_LIB_AUDIO_UNKNOWN_ERROR = 7,            /**< ERROR MSG: UNKNOWN ERROR */
} HX_LIB_AUDIO_ERROR_E;

/**
 * \enum HX_LIB_AUDIO_CLK_SRC_E
 * \brief reference clock of AUDIO interface
 */
typedef enum {
    HX_LIB_AUDIO_BLK_OLD,       /**< old block index */
    HX_LIB_AUDIO_BLK_NEW,       /**< latest block index */
} HX_LIB_AUDIO_BLK_E;

typedef void (*AUD_LIB_PREROLL_CB)(void);

/***************************************************
 * Structure Definition
 **************************************************/
typedef struct {
    uint32_t *addr;             /* This constant defines the Audio buffer start address, please notice address buffer size should be provided with block_sz*block_num */
    uint32_t block_num;         /* This constant defines the total number of Audio buffer block, must >=2  */
    uint32_t block_sz;          /* This constant defines audio block size in byte */
} HX_LIB_AUDIO_BUFFER_CONFIG_T;

/**
 * \struct HX_LIB_AUDIO_CONFIG_T
 * \brief AUDIO configuration for prerolling
 */
typedef struct {
    HX_LIB_AUDIO_BUFFER_CONFIG_T    buffer;
    PDM_CFG_T                       pdm_config;
} HX_LIB_AUDIO_CONFIG_T;

/***********************************************
 * FUNCTION DECLARATION
 **********************************************/

/**
 * \brief       The function is used to initial Audio pre-rolling interface
 *
 * \param[in]   aud_if                  initial audio pre-rolling with selected interface, now support "HX_LIB_AUDIO_IF_PDM_OTHER" only
 * \param[in]   clk_src                 address
 * \retval      HX_LIB_AUDIO_NO_ERROR   success
 * \retval      others                  fail
 */
HX_LIB_AUDIO_ERROR_E hx_lib_audio_preroll_init(HX_LIB_AUDIO_CONFIG_T *aud_config);

/**
 * \brief       The function is used to de-initial Audio pre-rolling
 *
 * \retval      HX_LIB_AUDIO_NO_ERROR   success
 * \retval      others                  fail
 */
HX_LIB_AUDIO_ERROR_E hx_lib_audio_preroll_deinit(void);

/**
 * \brief       The function is used to start pre-rolling with selected sampling rate. This function default start pre-rolling silently (will not trigger
 *              interrupt when each block finish), you can trigger interrupt via "hx_lib_audio_preroll_set_interrupt()"
 *              After "hx_lib_audio_preroll_start" called, "hx_lib_audio_preroll_deinit" and re-initial is needed to start audio pre-rolling again
 *
 * \retval      HX_LIB_AUDIO_NO_ERROR   success
 * \retval      others                  fail
 */
HX_LIB_AUDIO_ERROR_E hx_lib_audio_preroll_start(void);

/**
 * \brief       The function set pre-rolling interrupt flag, by setting enable flag to true, it triggers callback when each block received.
 *              When set enable flag to false, callback parameter will be ignore.
 *
 * \param[in]   enable                  interrupt flag
 * \retval      HX_LIB_AUDIO_NO_ERROR   success
 * \retval      others                  fail
 */
HX_LIB_AUDIO_ERROR_E hx_lib_audio_preroll_set_interrupt(bool enable, AUD_LIB_PREROLL_CB cb);

/**
 * \brief       The function will treat current time block as block index 0 and update block count in the buffer when new block
 *              received. Please call this API "with enable flag = true" first when HWVAD wake you up. Once you finish use the pre-rolling
 *              buffer and want go back to sleep, please call this API "with enable flag = false".
 *
 * \retval      HX_LIB_AUDIO_NO_ERROR   success
 * \retval      others                  fail
 */
HX_LIB_AUDIO_ERROR_E hx_lib_audio_preroll_update_index(bool enable);

/**
 * \brief       The function response block count index, you can get last count index or latest count index
 *
 * \param[in]   blk     selection of oldest or latest, First block is not ready for retrieve if latest index is smaller than zero.
 *
 * \retval      return index number
 */
int32_t hx_lib_audio_preroll_get_index(HX_LIB_AUDIO_BLK_E blk);

/**
 * \brief       The function feedback block pointer information with selected block index
 *              !!please remember to invalidate memory area before read memory data!!
 *
 * \param[in]   index       index number
 * \retval      return buffer address, otherwise return 0
 */
uint32_t* hx_lib_audio_preroll_read_ptr(int32_t index);


#endif /* LIBRARY_AUDIO_PREROLLING_LIB_H_ */

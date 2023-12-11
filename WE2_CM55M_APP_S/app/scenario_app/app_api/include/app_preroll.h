/**
 * @file app_datapath.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_PREROLL_H__
#define __APP_PREROLL_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    uint32_t wdma2_data; /**< wdam2 base address */
    uint32_t jpeg_auto_fill_data; /**< jpeg auto fill data base address */
}app_auto_preroll_data_t;

typedef struct
{
    uint32_t slot_size; /**< each slot size (byte)*/
    uint8_t total_slot; /**< total slot number */
    uint8_t first_frame_cap; /**< the first frame captured flag */
    uint8_t next_frame_no; /**< the next slot number to place frame */
    uint8_t start_frame_no; /**< start slot number of frame */
    uint8_t loop_flag; /**< all slot filled flag */
    app_auto_preroll_data_t preroll_data;
}app_auto_preroll_status_t;

typedef struct
{
    uint32_t wdma1_data; /**< wdma1 base address*/
    uint32_t wdma2_data; /**< preroll buffer base address*/
    uint32_t wdma3_data; /**< wdma3 base address*/
    uint32_t jpeg_auto_fill_data;/**< jpeg auto fill data base address */
    uint32_t slot_size; /**< preroll buffer each slot size*/
    uint8_t slot_num; /**< preroll buffer total slot number*/
}app_auto_preroll_config_t;

/**
 * \brief	set preroll buffer base address and total slot number
 *
 * \param[in]	*config	    preroll init config settings
 * \return	app api error code
 */
int8_t app_auto_preroll_init(app_auto_preroll_config_t *config);

/**
 * \brief	set data path for preroll
 *
 * \return	app api error code
 */
void app_auto_preroll_set_datapath(void);

/**
 * \brief	get current preroll status
 *
 * \param[out]	*status	    preroll status
 * \return	app api error code
 */
int8_t app_auto_preroll_get_status(app_auto_preroll_status_t* status);

/**
 * \brief	retain this latest one frame and update current preroll status
 *
 * \return	app api error code
 */
int8_t app_auto_preroll_retain_frame_updadte_status(void);

/**
 * \brief	reset preroll
 * 
 * \return	app api error code
 */
int8_t app_auto_preroll_reset_status(void);

#ifdef __cplusplus
}
#endif

#endif /*__APP_PREROLL_H__*/

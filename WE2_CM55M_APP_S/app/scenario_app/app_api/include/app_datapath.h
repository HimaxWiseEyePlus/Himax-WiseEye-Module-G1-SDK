/**
 * @file app_datapath.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_DATAPATH_H__
#define __APP_DATAPATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_inp.h"
#include "hx_drv_jpeg.h"
#include "hx_drv_CIS_common.h"
#include "sensor_dp_lib.h"

#include "app_cisdp_common_def.h"

/**
 * \struct app_dp_cfg_t
 * \brief data path config settings
 */
typedef struct
{
    APP_INIT_TYPE_E init_type;
    SENSORDPLIB_SENSOR_E sensor_type;
    SENSORDPLIB_STREAM_E stream_type;
    INP_SUBSAMPLE_E inp_subsample_type;
    INP_CROP_T crop;
    #if (IC_VERSION >= 30)
    INP_BINNING_E inp_bin;
    #endif
    GPIO_INDEX_E xshutdown_pin;
    
    uint8_t mclk_div;
    uint16_t sclk_utg;
    uint16_t h_fporch;
    uint16_t sensor_id;         /**< cis sensor id*/
    uint16_t rev_id;            /**< cis sensor revision id*/
    uint32_t sensor_width;      /**< cis sensor width*/
    uint32_t sensor_height;     /**< cis sendor height*/
    uint32_t inp_out_width;     /**< inp output width*/
    uint32_t inp_out_height;    /**< inp output height*/
    
    uint32_t wdma1;
    uint32_t wdma2;
    uint32_t wdma3;
    uint32_t jpeg_auto_fill_data;
    uint8_t wdma2_cyclic_buffer_cnt;
}app_dp_cfg_t;

/**
 * \struct app_dp_cap_t
 * \brief data path capture frame settings
 */
typedef struct
{
    uint8_t frame_no;                   /**< JPEG Encoding frame number*/
    uint16_t hw55_crop_stx;             /**< HW5x5 ROI Start x-position*/
    uint16_t hw55_crop_sty;             /**< HW5x5 ROI Start y-position*/
    uint16_t hw55_in_width;             /**< HW5x5 ROI width*/
    uint16_t hw55_in_height;            /**< HW5x5 ROI height*/
    uint16_t enc_width;                 /**< JPEG Encoding width(divide by 16)*/
    uint16_t enc_height;                /**< JPEG Encoding height(divide by 16)*/
    uint8_t hw55_path;                  /**< HW5x5 path*/
    uint8_t hw55_y_only;                /**< HW5x5 de-mosaic output Y channel only*/
    JPEG_ENC_TYPE_T jpeg_enctype;       /**< JPEG Encoding Color format Selection*/
    JPEG_ENC_QTABLE_T jpeg_encqtable;   /**< JPEG Encoding quantization table Selection*/
}app_dp_cap_t;

/**
 * \struct app_dp_jpg_enc_t
 * \brief data path jpeg encoding settings
 */
typedef struct
{
    uint8_t frame_no;                   /**< JPEG Encoding frame number*/
    uint16_t enc_width;                 /**< JPEG Encoding width(divide by 16)*/
    uint16_t enc_height;                /**< JPEG Encoding height(divide by 16)*/
    JPEG_ENC_TYPE_T jpeg_enctype;       /**< JPEG Encoding Color format Selection*/
    JPEG_ENC_QTABLE_T jpeg_encqtable;   /**< JPEG Encoding quantization table Selection*/
}app_dp_jpg_enc_t;

/**
 * \struct app_dp_jpg_dec_t
 * \brief data path jpeg decoding settings
 */
typedef struct
{
    uint8_t frame_no;                   /**< JPEG Decoding frame number*/
    uint16_t dec_roi_stx;               /**< JPEG Decoding ROI Start x-position(divide by 16)*/
    uint16_t dec_roi_sty;               /**< JPEG Decoding ROI Start y-position(divide by 16)*/
    uint16_t dec_width;                 /**< JPEG Decoding ROI width(divide by 16)*/
    uint16_t dec_height;                /**< JPEG Decoding ROI height(divide by 16)*/
    JPEG_ENC_TYPE_T jpeg_enctype;       /**< JPEG Encoding Color format Selection*/
    JPEG_ENC_QTABLE_T jpeg_encqtable;   /**< JPEG Encoding quantization table Selection*/
}app_dp_jpg_dec_t;

/**
 * \brief	sensor initialization
 *
 * \param[in]	sensor_type             sensor type which indicate the mode for sensor_control.
 * \return	void
 */
void app_dp_cis_init(SENSORDPLIB_SENSOR_E sensor_type);

/**
 * \brief	enable sensor streaming mode
 *
 * \param[in]	sensor_type             sensor type which indicate the mode for sensor_control.
 * \return	void
 */
void app_dp_cis_streaming_on(SENSORDPLIB_STREAM_E stream_type);

/**
 * \brief	data path initialization
 *
 * \param[in]	*dp_init_cfg	    data path init settings
 * \return	app api error code
 */
int8_t app_dp_init(const app_dp_cfg_t* dp_init_cfg);

/**
 * \brief	data path capture frame 
 *
 * \param[in]	*dp_cap_settings	    data path capture frame settings
 * \return	app api error code
 */
int8_t app_dp_capture_frame(const app_dp_cap_t* dp_cap_settings);

/**
 * \brief	data path capture jpeg frame 
 *
 * \param[in]	*dp_cap_settings	    data path capture jpeg frame settings
 * \return	app api error code
 */
int8_t app_dp_capture_jpeg_frame(const app_dp_cap_t* dp_cap_settings);

/**
 * \brief	data path capture raw frame 
 *
 * \param[in]	*dp_cap_settings	    data path capture raw frame settings
 * \return	app api error code
 */
int8_t app_dp_capture_raw_frame(const app_dp_cap_t* dp_cap_settings);

/**
 * \brief	data path capture frame with cdm 
 *
 * \param[in]	*dp_cap_settings	    data path capture frame settings
 * \return	app api error code
 */
int8_t app_dp_capture_frame_with_cdm(const app_dp_cap_t* dp_cap_settings);

/**
 * \brief	get data path frame back
 *
 * \param[out]	*praw_addr	    frame raw data
 * \param[out]	*pimg_width	    frame raw data width
 * \param[out]	*pimg_height	frame raw data height
 * \param[out]	*pjpg_addr	    frame jpeg data
 * \param[out]	*pjpg_size	    frame jpeg data size
 * \return	app api error code
 */
int8_t app_dp_get_frame(uint32_t *praw_addr, uint32_t *pimg_width, uint32_t *pimg_height, uint32_t *pjpg_addr, uint32_t *pjpg_size);

/**
 * \brief	data path decode jpeg frame
 *
 * \param[in]	*dp_jpg_dec_settings	    data path jepg decoding settings
 * \return	app api error code
 */
int8_t app_dp_decode_jpeg(const app_dp_jpg_dec_t* dp_jpg_dec_settings);

/**
 * \brief	data path encode jpeg frame
 *
 * \param[in]	*dp_jpg_enc_settings	    data path jepg encoding settings
 * \return	app api error code
 */
int8_t app_dp_encode_jpeg(const app_dp_jpg_enc_t* dp_jpg_enc_settings);

/**
 * \brief	get data path init default settings
 *
 * \param[out]	*dp_init_cfg	    data path init default settings
 * \return	app api error code
 */
int8_t app_dp_get_def_init_cfg(app_dp_cfg_t* dp_init_cfg);

/**
 * \brief	get data path capture frame default settings
 *
 * \param[out]	*dp_cap_settings	    data path capture frame default settings
 * \return	app api error code
 */
int8_t app_dp_get_def_cap_settings(app_dp_cap_t* dp_cap_settings);

/**
 * \brief	get data path jpeg decode default settings
 *
 * \param[out]	*dp_jpg_dec_settings	    data path jpeg decode default settings
 * \return	app api error code
 */
int8_t app_dp_get_def_dec_jpeg_settings(app_dp_jpg_dec_t* dp_jpg_dec_settings);

/**
 * \brief	get data path jpeg encode default settings
 *
 * \param[out]	*dp_jpg_enc_settings	    data path jpeg encode default settings
 * \return	app api error code
 */
int8_t app_dp_get_def_enc_jpeg_settings(app_dp_jpg_enc_t* dp_jpg_enc_settings);

#ifdef __cplusplus
}
#endif

#endif /*__APP_DATAPATH_H__*/

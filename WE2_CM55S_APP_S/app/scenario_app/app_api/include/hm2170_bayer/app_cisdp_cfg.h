/*
 * app_cisdp_cfg.h
 *
 *  Created on: 2022¦~11¤ë24¤é
 *      Author: 901912
 *
 *      HW5X5
 *      RGB: R = W*H byte, G = W*H byte, B = W*H byte
 *      YUV422: Y= W*H byte, U = ((W*H)>>1) byte, V = ((W*H)>>1) byte
 *      YUV420: Y= W*H byte, U = ((W*H)>>2) byte, V = ((W*H)>>2) byte
 *
 *      JPEG
 *      RAW8(YUV400) x4 Compress = ((613+ (W/16)*(H/16)* 64 + 35) >>2 ) <<2 byte
 *      RAW8(YUV400) x10 Compress = ((613+ (W/16)*(H/16)* 24 + 35) >>2 ) <<2 byte
 *      YUV422 x4 Compress = ((623+ (W/16)*(H/16)* 128 + 35) >>2 ) <<2 byte
 *      YUV422 x10 Compress = ((623+ (W/16)*(H/16)* 50 + 35) >>2 ) <<2  byte
 *      YUV420 x4 Compress = ((623+ (W/16)*(H/16)* 96 + 35) >>2 ) <<2  byte
 *      YUV420 x10 Compress = ((623+ (W/16)*(H/16)* 38 + 35) >>2 ) <<2  byte
 *
 *      CDM
 *      Hot Pixel No Pack + No Meta Data: High= W*H byte, Low= W*H byte, Hot Pixel = W*H byte
 *      Hot Pixel Pack + No Meta Data: High= W*H byte, Low= W*H byte, Hot Pixel = W*H/8 byte
 *      Hot Pixel No Pack + Meta Data: High= W*H byte, Low= W*H byte, Hot Pixel = W*H+3*4+H+W+(H<<1) byte
 *      Hot Pixel Pack + Meta Data: High= W*H byte, Low= W*H byte, Hot Pixel = (W*H>>3)+3*4+H+W+(H<<1) byte
 */

#ifndef APP_CISDP_CFG_H_
#define APP_CISDP_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hx_drv_gpio.h"
#include "hx_drv_sensorctrl.h"
#include "hx_drv_inp.h"
#include "hx_drv_hw2x2.h"
#include "hx_drv_cdm.h"
#include "hx_drv_hw5x5.h"
#include "hx_drv_jpeg.h"

#include "sensor_dp_lib.h"

#include "app_cisdp_common_def.h"
#include "app_datapath.h"

#define CIS_MIRROR_SETTING		(0x00) //0x00: off/0x01:H-Mirror/0x02:V-Mirror/0x03:HV-Mirror
#define CIS_I2C_ID				(0x24)
#define CIS_ENABLE_MIPI_INF		(0x00) //0x00: off/0x01: on
#define CIS_ENABLE_HX_AUTOI2C	(0x00) //0x00: off/0x01: on/0x2: on and XSLEEP KEEP HIGH

//#define CISDP_INIT_TYPE                 (APP_INIT_TYPE_VIDEO_STREAM)
#define CISDP_INIT_TYPE                 (APP_INIT_TYPE_AOS)

#define CISDP_SENSORCTRL_MCLK_DIV       (SENSORCTRL_MCLK_DIV1)
#define CISDP_CIS_XHSUTDOWN_PIN         (AON_GPIO2)

/*
 * HM11B1/HM2170 1-bit parser setting
 */
#define SCLK_72M_UTG 			(5) 	//8*24/72 if < 5, set 5
#define SCLK_36M_UTG 			(6) 	//8*24/36
#define SCLK_9M_UTG 			(20) 	//ceil(8*24/9.375 = 20.48) = 20
#define SCLK_UTG				(SCLK_72M_UTG)
/*
* HM2170 1-bit parser setting: PACK MODE
* REG_INP_RX_LBUF_INSERT_H_FPORCH = freq(inp_cclk) * sensor_hblank_time / 2
*/
#define SCLK_9M_FPORCH					(2047)
#define SCLK_72M_FPORCH					(542)
#define RX_LBUF_INSERT_H_FPORCH			(SCLK_72M_FPORCH)

#if (CIS_ENABLE_MIPI_INF != 0x00)
/*
 * DP SENCTRL CFG
 */
#define SENCTRL_SENSOR_TYPE			SENSORDPLIB_SENSOR_HM2170_MIPI
#define SENCTRL_STREAM_TYPE			SENSORDPLIB_STREAM_NONEAOS
#if (CIS_ENABLE_HX_AUTOI2C == 0x01)
#define SENCTRL_STREAM_TYPE			SENSORDPLIB_STREAM_NONEAOS_AUTOI2C
#elif (CIS_ENABLE_HX_AUTOI2C == 0x02)
#define SENCTRL_STREAM_TYPE			SENSORDPLIB_STREAM_NONEAOS_AUTOI2C_XSLEEP_HIGH
#endif
#define SENCTRL_SENSOR_WIDTH 		1928
#define SENCTRL_SENSOR_HEIGHT 		1088


/*
 * DP INP CFG
 *
 * SENSOR --> INP_CROP --> INP_BINNING --> INP_SUBSAMPLE
 *
 * CROP DISABLE: DP_INP_CROP_START_X/DP_INP_CROP_START_Y/DP_INP_CROP_WIDTH/DP_INP_CROP_HEIGHT all 0
 */
#define DP_INP_SUBSAMPLE			INP_SUBSAMPLE_DISABLE
#define DP_INP_BINNING				INP_BINNING_6TO2_B
#define DP_INP_CROP_START_X			0
#define DP_INP_CROP_START_Y			0
#define DP_INP_CROP_WIDTH          	1920
#define DP_INP_CROP_HEIGHT         	1080
#define DP_INP_OUT_WIDTH 		    640
#define DP_INP_OUT_HEIGHT 		    360


/*
 * DP HW2X2 CFG
 *
 * LIMITATION:
 * 2X2 SUBSAMPLE LT,LB,RT,RBBINNING (WIDTH/2, HEIGHT/2, MIN:2X2, MAX:640X480)
 * 1/3 SUBSAMPLE (WIDTH/3, HEIGHT/3, MIN:3X3, MAX:639X480)
 */
#define DP_HW2X2_PATH				HW2x2_PATH_THROUGH
#define DP_HW2X2_PROCESS_MODE		HW2x2_MODE_BINNING
#define DP_HW2X2_CROP_START_X		0
#define DP_HW2X2_CROP_START_Y		0
#define DP_HW2X2_CROP_WIDTH			DP_INP_OUT_WIDTH
#define DP_HW2X2_CROP_HEIGHT		DP_INP_OUT_HEIGHT
#define DP_HW2X2_ROUND_MODE			HW2x2_ROUNDMODE_FLOOR
#define DP_HW2X2_OUT_WIDTH          (DP_INP_OUT_WIDTH/2)
#define DP_HW2X2_OUT_HEIGHT         (DP_INP_OUT_HEIGHT/2)


/*
 * DP CDM CFG
 *
 * LIMITATION:
 * MAX: 480X270
 * MIN: 8X3
 * WIDTH/8
 */
#define DP_CDM_ENABLE				CDM_ENABLE_ON
#define DP_CDM_IN_START_X			0
#define DP_CDM_IN_START_Y			0
#define DP_CDM_IN_WIDTH 			DP_HW2X2_OUT_WIDTH
#define DP_CDM_IN_HEIGHT			DP_HW2X2_OUT_HEIGHT
#define DP_CDM_META_DUMP 			CDM_ENABLE_MATA_DUMP_ON
#define DP_CDM_HT_PACKING 			CDM_ENABLE_HT_PACKING_ON
#define DP_CDM_MIN_ALLOW_DIS 		3
#define DP_CDM_TOLERANCE 			3
#define DP_CDM_REACTANCE 			2
#define DP_CDM_RELAXATION 			1
#define DP_CDM_EROS_TH 				3
#define DP_CDM_NUM_HT_TH 			10
#define DP_CDM_NUM_HT_VECT_TH_X 	8
#define DP_CDM_NUM_HT_VECT_TH_Y 	4
#define DP_CDM_NUM_CONS_HT_BIN_TH_X 1
#define DP_CDM_NUM_CONS_HT_BIN_TH_Y 1
#define DP_CDM_CPU_ACTIVEFLAG 		CDM_CPU_ACTFLAG_SLEEP
#define DP_CDM_INIT_MAP_FLAG 		CDM_INIMAP_FLAG_ON


/*
 * DP HW5X5 CFG
 *
 * LIMITATION:
 * MIN: 8X8
 * WIDTH/8
 * HEIGHT/4
 */
#define DP_HW5X5_PATH				HW5x5_PATH_THROUGH_DEMOSAIC
#define DP_HW5X5_DEMOS_BNDMODE		DEMOS_BNDODE_REFLECT
#define DP_HW5X5_DEMOS_COLORMODE	DEMOS_COLORMODE_YUV422

#if (CIS_MIRROR_SETTING == 0x01)
#define DP_HW5X5_DEMOS_PATTERN		DEMOS_PATTENMODE_GBRG
#elif (CIS_MIRROR_SETTING == 0x02)
#define DP_HW5X5_DEMOS_PATTERN		DEMOS_PATTENMODE_GRBG
#elif (CIS_MIRROR_SETTING == 0x03)
#define DP_HW5X5_DEMOS_PATTERN		DEMOS_PATTENMODE_RGGB
#else
#define DP_HW5X5_DEMOS_PATTERN		DEMOS_PATTENMODE_BGGR
#endif

#define DP_HW5X5_DEMOSLPF_ROUNDMODE DEMOSLPF_ROUNDMODE_FLOOR
#define DP_HW5X5_CROP_START_X 		0
#define DP_HW5X5_CROP_START_Y 		0
#define DP_HW5X5_CROP_WIDTH 		640
#define DP_HW5X5_CROP_HEIGHT 		352
#define DP_HW5X5_OUT_WIDTH 			640
#define DP_HW5X5_OUT_HEIGHT 		352

/*
 * DP JPEG CFG
 *
 * LIMITATION:
 * MAX:640X640
 * MIN: 16X16
 * WIDTH/16
 * HEIGHT/16
 */
#define DP_JPEG_PATH				JPEG_PATH_ENCODER_EN
#define DP_JPEG_ENC_WIDTH 			DP_HW5X5_OUT_WIDTH
#define DP_JPEG_ENC_HEIGHT 			DP_HW5X5_OUT_HEIGHT
#define DP_JPEG_ENCTYPE 			JPEG_ENC_TYPE_YUV422
#define DP_JPEG_ENCQTABLE 			JPEG_ENC_QTABLE_10X

#define DP_WDMA_SIZE_CFG            JPEG_ENC_YUV422_10X

#define DP_JPEG_DEC_ROI_START_X     0
#define DP_JPEG_DEC_ROI_START_Y     0
#define DP_JPEG_DEC_WIDTH 			640
#define DP_JPEG_DEC_HEIGHT 			352
#define DP_JPEG_DECTYPE 			JPEG_DEC_TYPE_YUV422
#define DP_JPEG_DECCQTABLE 			JPEG_DEC_QTABLE_10X
#else
/*
 * DP SENCTRL CFG
 */
#define SENCTRL_SENSOR_TYPE					SENSORDPLIB_SENSOR_HM2170_SERIAL_MSB
#define SENCTRL_STREAM_TYPE					SENSORDPLIB_STREAM_HM11B1_LOWPOWER //SENSORDPLIB_STREAM_NONEAOS //SENSORDPLIB_STREAM_HM11B1_LOWPOWER
//#define SENCTRL_STREAM_TYPE_NONAOS		SENSORDPLIB_STREAM_NONEAOS
#if (CIS_ENABLE_HX_AUTOI2C == 0x01)
#define SENCTRL_STREAM_TYPE					SENSORDPLIB_STREAM_NONEAOS_AUTOI2C
#elif (CIS_ENABLE_HX_AUTOI2C == 0x02)
#define SENCTRL_STREAM_TYPE					SENSORDPLIB_STREAM_NONEAOS_AUTOI2C_XSLEEP_HIGH
#endif
#define SENCTRL_SENSOR_WIDTH 				480
#define SENCTRL_SENSOR_HEIGHT 				272


/*
 * DP INP CFG
 *
 * SENSOR --> INP_CROP --> INP_BINNING --> INP_SUBSAMPLE
 *
 * CROP DISABLE: DP_INP_CROP_START_X/DP_INP_CROP_START_Y/DP_INP_CROP_WIDTH/DP_INP_CROP_HEIGHT all 0
 */
#define DP_INP_SUBSAMPLE			INP_SUBSAMPLE_DISABLE
#define DP_INP_BINNING				INP_BINNING_DISABLE
#define DP_INP_CROP_START_X			0
#define DP_INP_CROP_START_Y			0
#define DP_INP_CROP_WIDTH          	0
#define DP_INP_CROP_HEIGHT         	0
#define DP_INP_OUT_WIDTH 		    480
#define DP_INP_OUT_HEIGHT 		    272


/*
 * DP HW2X2 CFG
 *
 * LIMITATION:
 * 2X2 SUBSAMPLE LT,LB,RT,RBBINNING (WIDTH/2, HEIGHT/2, MIN:2X2, MAX:640X480)
 * 1/3 SUBSAMPLE (WIDTH/3, HEIGHT/3, MIN:3X3, MAX:639X480)
 */
#define DP_HW2X2_PATH				HW2x2_PATH_THROUGH
#define DP_HW2X2_PROCESS_MODE		HW2x2_MODE_BINNING
#define DP_HW2X2_CROP_START_X		0
#define DP_HW2X2_CROP_START_Y		0
#define DP_HW2X2_CROP_WIDTH			DP_INP_OUT_WIDTH
#define DP_HW2X2_CROP_HEIGHT		270
#define DP_HW2X2_ROUND_MODE			HW2x2_ROUNDMODE_FLOOR
#define DP_HW2X2_OUT_WIDTH          (DP_INP_OUT_WIDTH/2)
#define DP_HW2X2_OUT_HEIGHT         (270/2)


/*
 * DP CDM CFG
 *
 * LIMITATION:
 * MAX: 480X270
 * MIN: 8X3
 * WIDTH/8
 */
#define DP_CDM_ENABLE				CDM_ENABLE_ON
#define DP_CDM_IN_START_X			0
#define DP_CDM_IN_START_Y			0
#define DP_CDM_IN_WIDTH 			DP_HW2X2_OUT_WIDTH
#define DP_CDM_IN_HEIGHT			DP_HW2X2_OUT_HEIGHT
#define DP_CDM_META_DUMP 			CDM_ENABLE_MATA_DUMP_ON
#define DP_CDM_HT_PACKING 			CDM_ENABLE_HT_PACKING_ON
#define DP_CDM_MIN_ALLOW_DIS 		15
#define DP_CDM_TOLERANCE 			3
#define DP_CDM_REACTANCE 			2
#define DP_CDM_RELAXATION 			1
#define DP_CDM_EROS_TH 				3
#define DP_CDM_NUM_HT_TH 			10
#define DP_CDM_NUM_HT_VECT_TH_X 	8
#define DP_CDM_NUM_HT_VECT_TH_Y 	4
#define DP_CDM_NUM_CONS_HT_BIN_TH_X 1
#define DP_CDM_NUM_CONS_HT_BIN_TH_Y 1
#define DP_CDM_CPU_ACTIVEFLAG 		CDM_CPU_ACTFLAG_ACTIVE
#define DP_CDM_INIT_MAP_FLAG 		CDM_INIMAP_FLAG_ON_ONEFRAME_ONLY


/*
 * DP HW5X5 CFG
 *
 * LIMITATION:
 * MIN: 8X8
 * WIDTH/8
 * HEIGHT/4
 */
#define DP_HW5X5_PATH				HW5x5_PATH_THROUGH_DEMOSAIC
#define DP_HW5X5_DEMOS_BNDMODE		DEMOS_BNDODE_REFLECT
//#define DP_HW5X5_DEMOS_COLORMODE	DEMOS_COLORMODE_YUV422
#define DP_HW5X5_DEMOS_COLORMODE	DEMOS_COLORMODE_YUV400

#if (CIS_MIRROR_SETTING == 0x01)
#define DP_HW5X5_DEMOS_PATTERN		DEMOS_PATTENMODE_GBRG
#elif (CIS_MIRROR_SETTING == 0x02)
#define DP_HW5X5_DEMOS_PATTERN		DEMOS_PATTENMODE_GRBG
#elif (CIS_MIRROR_SETTING == 0x03)
#define DP_HW5X5_DEMOS_PATTERN		DEMOS_PATTENMODE_RGGB
#else
#define DP_HW5X5_DEMOS_PATTERN		DEMOS_PATTENMODE_BGGR
#endif

#define DP_HW5X5_DEMOSLPF_ROUNDMODE DEMOSLPF_ROUNDMODE_FLOOR
#define DP_HW5X5_CROP_START_X 		0
#define DP_HW5X5_CROP_START_Y 		0
#define DP_HW5X5_CROP_WIDTH 		480
#define DP_HW5X5_CROP_HEIGHT 		272
#define DP_HW5X5_OUT_WIDTH 			(480)
#define DP_HW5X5_OUT_HEIGHT 		(272)

/*
 * DP JPEG CFG
 *
 * LIMITATION:
 * MAX:640X640
 * MIN: 16X16
 * WIDTH/16
 * HEIGHT/16
 */
#define DP_JPEG_PATH				JPEG_PATH_ENCODER_EN
#define DP_JPEG_ENC_WIDTH 			DP_HW5X5_OUT_WIDTH
#define DP_JPEG_ENC_HEIGHT 			DP_HW5X5_OUT_HEIGHT
#define DP_JPEG_ENCTYPE 			JPEG_ENC_TYPE_YUV400
//#define DP_JPEG_ENCTYPE 			JPEG_ENC_TYPE_YUV422

#define DP_JPEG_ENCQTABLE 			JPEG_ENC_QTABLE_10X

//#define DP_WDMA_SIZE_CFG            JPEG_ENC_YUV422_10X
#define DP_WDMA_SIZE_CFG            JPEG_ENC_YUV400_10X

#define DP_JPEG_DEC_ROI_START_X     0
#define DP_JPEG_DEC_ROI_START_Y     0
#define DP_JPEG_DEC_WIDTH 			480
#define DP_JPEG_DEC_HEIGHT 			272
//#define DP_JPEG_DECTYPE 			JPEG_DEC_TYPE_YUV422
#define DP_JPEG_DECTYPE 			JPEG_DEC_TYPE_YUV400
#define DP_JPEG_DECCQTABLE 			JPEG_DEC_QTABLE_10X
#endif

/*
 * SENDPLIB PARAMETERS
 */
#define SENDPLIB_MIPIRX_LANE_NB 			2

/*
 * FUNCTION PROTOTYPES
 */
#if (CIS_ENABLE_HX_AUTOI2C != 0x00)
void app_cisdp_set_hxautoi2c(APP_INIT_TYPE_E type);
#endif

int app_cisdp_datapath_init(const app_dp_cfg_t* dp_init_cfg);
int app_cisdp_sensor_init(const app_dp_cfg_t* dp_init_cfg);
void app_cisdp_sensor_start(void);
void app_cisdp_sensor_start_AOS(void);
void app_cisdp_sensor_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_CISDP_CFG_H_ */

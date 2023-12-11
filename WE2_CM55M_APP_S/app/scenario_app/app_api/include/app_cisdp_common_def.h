#ifndef __APP_CISDP_COMMON_DEF_H__
#define __APP_CISDP_COMMON_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define JPEG_ENC_YUV400_4X      (0) //JPEG_ENCODE_YUV400 + JPEG_ENCODE_Q_4X
#define JPEG_ENC_YUV400_10X     (1) //JPEG_ENCODE_YUV400 + JPEG_ENCODE_Q_10X
#define JPEG_ENC_YUV420_4X      (2) //JPEG_ENCODE_YUV420 + JPEG_ENCODE_Q_4X
#define JPEG_ENC_YUV420_10X     (3) //JPEG_ENCODE_YUV420 + JPEG_ENCODE_Q_10X
#define JPEG_ENC_YUV422_4X      (4) //JPEG_ENCODE_YUV422 + JPEG_ENCODE_Q_4X
#define JPEG_ENC_YUV422_10X     (5) //JPEG_ENCODE_YUV422 + JPEG_ENCODE_Q_10X

/**
 * \enum APP_INIT_TYPE_E
 * \brief
 */
typedef enum APP_INIT_TYPE_S
{
	APP_INIT_TYPE_NONE          = 0x00,
	APP_INIT_TYPE_VIDEO_STREAM  = 0x01,	/*For CPU usage*/
	APP_INIT_TYPE_AOS           = 0x02, /*For PMU USAGE*/
} APP_INIT_TYPE_E;

#ifdef __cplusplus
}
#endif

#endif /*__APP_CISDP_COMMON_DEF_H__*/


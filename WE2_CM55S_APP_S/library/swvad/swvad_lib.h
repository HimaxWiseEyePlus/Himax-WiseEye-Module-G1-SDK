/** ********************************************************************************************
 *  @file      swvad_lib.h
 *  @details   This file contains all swvad related function and variable declaration
 *  @author    himax/904289
 *  @version   V1.0.0
 *  @date      16-May-2023
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/
/****************************************************
 * Variable Declaration                             *
 ***************************************************/
#define DATA_LEN            16000
#define FS                  16000
#define DEGREE              3
#define NORMALIZE           32767
#define DIFF_ORDER          1
#define VOL_WEIGHT          0.76f
#define MIN_MAX_PERCENTAGE  2.3f
#define SILENCE_SECOND     (5 * 31) /* 1 sec: 31 frames */

#define CPU_CLK             (0xffffff+1)
#define FRAME_LEN           31//(size_t)(1.0 * DATA_LEN / WINDOW_SIZE)
#define WINDOW_SIZE (size_t)(1.0 * 256 / 8000 * FS)

#define FRAME_DATA                  512
#define ONE_DATA_BYTE               2
#define FRAME_DATA_SZ               (FRAME_DATA * ONE_DATA_BYTE)
/*
 * \struct volume_hod
 * \brief volume and hod for VAD Input
 */
struct volume_hod
{
    float volume;
    float hod;
};

typedef struct corr {
    float ACF_peak;
    float ACF_diff;
    float NSDF_peak;
} corr;

/*
 * \struct EPD_result
 * \brief VAD algorithm (EPD) result
 */
struct EPD_result
{
    bool sound;
    int start;
    int end;
    float volume_max;
    float hod_max;
};

/*
 * \struct vad_cof
 * \brief arm_matrix_instance parameter for vad matrix coefficient
 */
struct vad_cof
{
    arm_matrix_instance_f32 XTMXIMXT;
    arm_matrix_instance_f32 Y;
    arm_matrix_instance_f32 COF;
};

/*
 * \struct vad_cof_mat_ptr
 * \brief vad matrix coefficient pointer
 */
struct vad_cof_mat_ptr
{
     float32_t* XTMXIMXT_f32;
     float32_t* Y_f32;
     float32_t* COF_f32;
};

typedef struct volhod_param {
    float vol_dc, hod_dc, vhRatio;
    int8_t init_counter;
} volhod_param;

typedef struct corr_param {
    float ACF_th, NSDF_th;
    float ACF_th_min;
    float NSDF_th_min, NSDF_th_max;
    float alpha_1, alpha_2;
    float beta_1, beta_2;
    int8_t init_counter;
    int8_t peak_detect_param;
    int8_t pass_counter;
} corr_param;

/*
 * \brief       Initial VAD coefficient parameter
 * \param[in]  volhod_param         volume and hod parameter
 * \param[in]  corr_param           auto correlation parameter
 * \param[in]  x                    Polynomial fitting parameter
 * \param[in] mat_ptr               vad matrix coefficient pointer
 * \param[in] para                  arm_matrix_instance parameter for vad matrix coefficient
 * \param[in] VAD_INPUT             31 frames of VAD_INPUT with hod and volume value
 * \param[in] VAD_OUTPUT            VAD algorithm (EPD) result
 * \param[in] clear_frame           which frame index to be clear
 * \retval true                     Initial success
 *         false                    Initial fail
 *
 *
 */
bool  VAD_init(volhod_param* volhod_param,
        corr_param* corr_param,
        float* x,
        struct vad_cof_mat_ptr* mat_ptr,
        struct vad_cof* para ,
        struct volume_hod*  VAD_INPUT,
        struct EPD_result* VAD_OUTPUT);



/*
 * \brief       Calculate single frame of hod and volume and auto correlation parameter update
 * \param[in] AudioInput            single frame pointer about AudioInput, which size is WINDOW_SIZE(512)
 * \param[in]  x                    Polynomial fitting parameter
 * \param[in] para                  arm_matrix_instance parameter for vad matrix coefficient
 * \param[in] mat_ptr               vad matrix coefficient pointer
 * \param[in] autocorr_param        auto correlation parameter update
 * \param[in] index                 frame index to check current frame is first three frame or the last frame to update autocorr threshold and volhod_dc value init
 *
 *
 * \param[out] struct volume_hod    volume and hod at one frame
 * \retval  volume
 * \retval  hod
 *
 */
struct volume_hod  VAD(const int16_t* AudioInput,
        float* x,
        struct vad_cof para,
        struct vad_cof_mat_ptr mat_ptr,
        corr_param* autocorr_param,
        int index,
        volhod_param* volhod_param);

/*
 * \brief       Clear auto correlation parameter and volume and hod dc parameter
 * Update one frame once
 * \param[in] VAD_INPUT         31 frames of VAD_INPUT with hod and volume value
 * \param[in] volhod_param      volume and hod dc parameter
 * \param[in] corr_param        auto correlation parameter
 * \param[in] VAD_OUTPUT        VAD algorithm (EPD) result
 *
 */
void VAD_clear(volhod_param* volhod_param, corr_param* corr_param);

/*
 * \brief       Calculate VAD result
 * \param[in] VAD_INPUT         31 frames of VAD_INPUT with hod and volume value
 * \param[in] volhod_param      volume and hod dc parameter
 * \param[in] corr_param        auto correlation parameter
 * \param[out] VAD_OUTPUT
 * \retval                      VAD_OUTPUT.sound:  0 (no sound) 1 (has sound)
 *                              VAD_OUTPUT.start:  denote which frame start with sound  (VAD_OUTPUT.start*WINDOW_SIZE is the actual start pointer rec_sz_cntition  at AudioInput)
 *                              VAD_OUTPUT.end: denote which frame end up with sound    ((VAD_OUTPUT.end+1)*WINDOW_SIZE is the actual end pointer rec_sz_cntition  at AudioInput)
 *                              VAD_OUTPUT.volume_max:  the maximum value about volume
 *                              VAD_OUTPUT.hod_max:  the maximum value about hod
 *
 */
void VAD_result(struct volume_hod*  VAD_INPUT,
        struct EPD_result* VAD_OUTPUT,
        volhod_param* volhod_param,
        corr_param* corr_param);
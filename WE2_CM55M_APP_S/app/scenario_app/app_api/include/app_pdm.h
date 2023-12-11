/**
 * @file app_pdm.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */
 
#ifndef __APP_PDM_H__
#define __APP_PDM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aud_lib.h"

typedef void (*AUDIO_RX_CALLBACK)(void*);

typedef struct app_pdm_config {
    PDM_PCM_FREQ_E sample_rate; /* This constant defines the Audio sample rate */
    uint32_t *buffer_addr;      /* This constant defines the Audio buffer start address */
    uint32_t block_num;         /* This constant defines the total number of Audio buffer block */
    uint32_t block_sz;          /* This constant defines audio block size, max is 8KB for uDMA single transfer */
    uint32_t cb_evt_blk;        /* This constant defines the Audio buffer block fill callback event */
    AUDIO_RX_CALLBACK g_audio_rx_cb_fn;
} app_pdm_config_t;

void app_pdm_init(app_pdm_config_t *app_pdm_cfg);
void app_pdm_rec_start();
void app_pdm_rec_stop();
void app_pdm_get_cfg(app_pdm_config_t *app_pdm_cfg);
uint8_t app_pdm_is_buf_ready(uint32_t *address, uint32_t *block_num);
void app_pdm_proc_buf_done(uint32_t block_num);

#ifdef __cplusplus
}
#endif

#endif /*__APP_PDM_H__*/



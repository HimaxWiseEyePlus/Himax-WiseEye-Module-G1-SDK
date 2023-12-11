/**
 * @file app_speaker.h
 * @brief 
 * @version V1.0.0
 * @date 2022-07-14
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef __APP_SPEAKER_H__
#define __APP_SPEAKER_H__

#ifdef __cplusplus
extern "C" {
#endif

// LM49450 speaker
#define I2S_SLAVE_ADDR		0x7D                    /**< audio amplifier device i2c slave address */

typedef unsigned int UINT32_T;

struct buffer {
  unsigned char const *start;
  unsigned long length;
};

/**
 * \brief	audio speaker amplifiler initialization
 *
 * \return	void
 */
int32_t app_speaker_init();

/**
 * \brief	audio speaker amplifiler i2s channel initialization
 *
 * \return	void
 */
int32_t app_speaker_i2s_init();

/**
 * \brief	send audio content to speaker amplifiler
 *
 * \param[in]	i2s_tx_addr	                    audio content transmit address
 * \param[in]	i2s_tx_size	                audio content transmit size
 * \param[in]	frame_size_samples	        audio content samples per frame
 * \return	app api error code
 */
int32_t app_speaker_output(uint32_t i2s_tx_addr, uint32_t i2s_tx_size, uint32_t frame_size_samples);

/**
 * \brief	Get current i2s TX status
 *
 * \return	int 0: available; 1: busy
 */

int32_t app_speaker_get_tx_status(void);

#ifdef __cplusplus
}
#endif

#endif /*__APP_SPEAKER_H__*/


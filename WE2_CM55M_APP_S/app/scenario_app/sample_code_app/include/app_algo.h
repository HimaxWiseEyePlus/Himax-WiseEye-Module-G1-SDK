#ifndef __ALGO_H__
#define __ALGO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "algo_metadata.h"

int32_t app_algo_init();
int32_t app_algo_run(uint32_t image_addr, uint32_t image_width, uint32_t image_height, ALGO_RESULT *algoresult);

#ifdef __cplusplus
}
#endif

#endif

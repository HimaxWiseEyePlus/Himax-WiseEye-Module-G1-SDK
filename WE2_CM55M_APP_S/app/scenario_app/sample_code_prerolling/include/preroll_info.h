#ifndef __PREROLL_INFO_H__
#define __PREROLL_INFO_H__

typedef struct _SENSOR_INFO
{
    uint16_t exposure;
    uint16_t again;
    uint16_t dgain;
    uint8_t reserved[26];
} SENSOR_INFO;

typedef struct _IMAGES_INFO
{
    uint16_t width;
    uint16_t height;
    uint8_t frame_num;
    uint8_t compress_rate;
    uint8_t reserved[26];
} IMAGES_INFO;

typedef struct _ALS_INFO
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t w;
    uint32_t ir;
    int32_t lux;
    uint8_t mode; //0: day mode; 1: night mode;
    uint8_t reserved[7];
} ALS_INFO;

typedef struct _PREROLL_INFO
{
    SENSOR_INFO sensor_info;
    IMAGES_INFO images_info;
    ALS_INFO als_info;
    uint8_t reserved[160];
} PREROLL_INFO;

#endif
#ifndef _ALGO_METADATA_H_
#define _ALGO_METADATA_H_

#define MAXCOUNT 	 10

#define HUMAN_PRESENCE				1
#define HUMAN_NOT_PRESENCE			0


#define EXTRA_DATA_TYPE_UNUSED		0
#define EXTRA_DATA_TYPE_RESERVED	1
#define EXTRA_DATA_TYPE_GENDER_AGE	2
#define EXTRA_DATA_TYPE_FR			3

#define EXTRA_DATA_RESERVED_SIZE	48

//-------------------------------------------------------------------
#pragma pack(1)
typedef struct _BOX
{
	unsigned short x;
	unsigned short y;
	unsigned short width;
	unsigned short height;
} BOX;

typedef struct _FR_INFO
{
	BOX				lefteye;
	BOX				righteye;
	BOX				mouth;
	unsigned int	register_id;
	unsigned int 	view_counts;
	unsigned char	is_registered;
	struct
	{
		short		yaw;
		short		pitch;
		short		roll;
	}pose;
	unsigned char	reserved[EXTRA_DATA_RESERVED_SIZE - 39];
} FR_INFO;

typedef struct _GENDER_AGE_INFO
{
	unsigned char	gender;
	unsigned char	age;
	unsigned char	reserved[EXTRA_DATA_RESERVED_SIZE - 2];
} GENDER_AGE_INFO;

typedef struct _GESTURE_INFO
{
	unsigned char	gesture;
	unsigned char	reserved[EXTRA_DATA_RESERVED_SIZE - 1];
} GESTURE_INFO;

//-------------------------------------------------------------------
typedef struct _DETECT_INFO_EXTRA_DATA
{
	unsigned char	type;

	union _DATA
	{
		unsigned char	reserved[EXTRA_DATA_RESERVED_SIZE];
		FR_INFO 		fr;
		GENDER_AGE_INFO gender_age;
		GESTURE_INFO 	gesture;
	} data;
} EXTRA_DATA;

//-------------------------------------------------------------------
typedef struct _DETECT_INFO
{
	BOX 			box;
	unsigned int	id;
	unsigned char	reliable;
	unsigned char	zone;
	short			confidence;
	short			score;

	EXTRA_DATA		extra;
} DETECT_INFO;

typedef struct _DETECT_INFO_EX
{
	BOX 				detectregion;
	short				num_of_detection;

	DETECT_INFO 		info[MAXCOUNT];
} DETECT_INFO_EX;

//-------------------------------------------------------------------
typedef struct _ALGO_RESULT
{
	unsigned char		humanPresence;		//1: HUMAN_PRESENCE, 0: HUMAN_NOT_PRESENCE

	DETECT_INFO_EX		bd;
	DETECT_INFO_EX		fd;
	DETECT_INFO_EX		motiontarget;
} ALGO_RESULT;

#pragma pack()

#endif
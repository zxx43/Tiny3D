/*
 * constants.h
 *
 *  Created on: 2017-4-1
 *      Author: a
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#ifndef NONE
#define NONE 0
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4
#define MNEAR 5
#define MFAR 6
#define MNONE 7

#define PI 3.1415926f
#define PI2 6.2831852f
#define A2R 0.01745329222f
#define R2A 57.2957804905f

#define SCR_WIDTH 640
#define SCR_HEIGHT 480

#define D_ROTATION 1.0f
#define D_DISTANCE 1.0f
#define L_DISTANCE 0.0025f
#define MIN_HEIGHT 10.0f

#define LOW_PRE 0
#define HIGH_PRE 1
#define FLOAT_PRE 2
#define NEAREST 0
#define LINEAR 1
#endif

#ifndef NULL
#define NULL 0
#endif

const int InvalidInsId = 1024;

typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long long u64;
typedef long long i64;

#endif /* CONSTANTS_H_ */

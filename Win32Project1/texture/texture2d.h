/*
 * texture2d.h
 *
 *  Created on: 2017-8-19
 *      Author: a
 */

#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#ifndef TEXTURE_TYPE_COLOR
#define TEXTURE_TYPE_COLOR 1
#define TEXTURE_TYPE_DEPTH 2
#define TEXTURE_TYPE_ANIME 3
#endif

#include "../render/glheader.h"
#include "../constants/constants.h"
#include <string>

class Texture2D {
private:
	GLenum texType, depthType, format;
public:
	uint id;
	u64 hnd;
	uint width, height, channel;
	int type, precision;
public:
	Texture2D(uint w,uint h, bool useMip,int t,int p,int c,int filter, int wrapMode, bool clearWhite = true, void* initData=NULL);
	~Texture2D();
public:
	void copyDataFrom(Texture2D* src);
	void readData(int bitSize, void* ret);
private:
	u64 genBindless();
	void releaseBindless(u64 texHnd);
};

#endif /* TEXTURE2D_H_ */

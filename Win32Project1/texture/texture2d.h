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

#include "texture.h"
#include <string>

class Texture2D: public Texture {
private:
	GLenum depthType, preDepth;
public:
	int type;
public:
	Texture2D(uint w,uint h, bool useMip,int t,int p,int c,int filter, int wrapMode, bool clearWhite = true, void* initData=NULL);
	virtual ~Texture2D();
public:
	virtual void readData(int bitSize, void* ret);
	virtual u64 genBindless();
	virtual void releaseBindless(u64 texHnd);
};

#endif /* TEXTURE2D_H_ */

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
#define LOW_PRE 0
#define HIGH_PRE 1
#define FLOAT_PRE 2
#endif

#include "../render/glheader.h"

class Texture2D {
private:
	GLenum texType, depthType;
	int buffSize;
public:
	unsigned int id;
	float width, height, channel;
	GLenum preColor, preDepth, format;
	int type, precision;
public:
	Texture2D(float w,float h,int t,int p,int c,bool clampBorder=true);
	~Texture2D();

	void copyDataFrom(Texture2D* src);
};

#endif /* TEXTURE2D_H_ */

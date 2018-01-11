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
#endif

#include "../render/glheader.h"

class Texture2D {
public:
	unsigned int id;
	float width,height;
	int type;

	Texture2D(float w,float h,int t);
	~Texture2D();

	void use(int slot);
};

#endif /* TEXTURE2D_H_ */

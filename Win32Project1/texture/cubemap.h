/*
 * cubemap.h
 *
 *  Created on: 2017-9-20
 *      Author: a
 */

#ifndef CUBEMAP_H_
#define CUBEMAP_H_

#include "../render/glheader.h"
#include "../constants/constants.h"
#include "bmpimage.h"

class CubeMap {
private:
	BmpImage* xposImg;
	BmpImage* xnegImg;
	BmpImage* yposImg;
	BmpImage* ynegImg;
	BmpImage* zposImg;
	BmpImage* znegImg;
public:
	unsigned int id;
	u64 hnd;
private:
	u64 genBindless();
	void releaseBindless(u64 texHnd);
public:
	CubeMap(const char* xpos,const char* xneg,
			const char* ypos,const char* yneg,
			const char* zpos,const char* zneg);
	~CubeMap();
};

#endif /* CUBEMAP_H_ */

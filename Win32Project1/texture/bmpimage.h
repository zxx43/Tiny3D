/*
 * texture.h
 *
 *  Created on: 2017-4-21
 *      Author: a
 */

#ifndef BMPIMAGE_H_
#define BMPIMAGE_H_

#include "bmploader.h"

class BmpImage {
private:
	BmpLoader* loader;
public:
	int width,height;
	unsigned char* data;

	BmpImage(const char* path);
	~BmpImage();
};


#endif /* BMPIMAGE_H_ */

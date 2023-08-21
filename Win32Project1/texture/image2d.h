#ifndef IMAGE2D_H_
#define IMAGE2D_H_

#include "texture.h"

class Image2D: public Texture {
public:
	Image2D(uint w, uint h, int p, int c, int layout, int filter, int wrapMode, void* initData = NULL);
	virtual ~Image2D();
public:
	void setLayout(int layout);
	void updateData(void* data);
	void readData(int bitSize, void* ret);
};

#endif
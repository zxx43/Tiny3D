#ifndef IMAGE2D_H_
#define IMAGE2D_H_

#include "../render/glheader.h"
#include "../constants/constants.h"

class Image2D {
private:
	GLenum texType, format, preColor;
private:
	u64 genBindless();
	void releaseBindless(u64 texHnd);
public:
	uint id;
	u64 hnd;
	uint width, height, channel;
	int precision;
public:
	Image2D(uint w, uint h, int p, int c, int layout, int filter, int wrapMode, void* initData = NULL);
	~Image2D();
public:
	void setLayout(int layout);
	void updateData(void* data);
	void readData(int bitSize, void* ret);
};

#endif
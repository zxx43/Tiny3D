#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "../render/glheader.h"
#include "../constants/constants.h"

class Texture {
public:
	GLenum texType, format, preColor;
public:
	uint id;
	u64 hnd;
	uint width, height, channel;
	int precision;
public:
	Texture(uint w, uint h, int p, int c);
	virtual ~Texture();
public:
	void copyDataFrom(Texture* src);
	virtual void readData(int bitSize, void* ret) = 0;
	virtual u64 genBindless() = 0;
	virtual void releaseBindless(u64 texHnd) = 0;
};

#endif

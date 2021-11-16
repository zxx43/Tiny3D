#ifndef TEXTURE_BUFFER_H_
#define TEXTURE_BUFFER_H_

#include "../render/glheader.h"
#include "../constants/constants.h"

class TextureBuffer {
private:
	int precision, component, maxSize;
	uint bitSize;
	uint buffId;
public:
	uint id;
	u64 hnd;
public:
	TextureBuffer(int p, int c, int size, void* data = NULL);
	~TextureBuffer();
public:
	void update(uint size, void* data);
private:
	u64 genBindless();
	void releaseBindless(u64 texHnd);
};

#endif // !TEXTURE_BUFFER_H_


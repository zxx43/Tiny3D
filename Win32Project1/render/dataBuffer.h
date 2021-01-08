#ifndef DATA_BUFFER_H_
#define DATA_BUFFER_H_

#include "../constants/constants.h"
#include "../util/util.h"

#define STATICS_BUFFER 1
#define ANIMATE_BUFFER 2

class DataBuffer {
public:
	float* vertexBuffer;
	half* normalBuffer;
	half* tangentBuffer;
	float* texcoordBuffer;
	float* texidBuffer;
	byte* colorBuffer;
	ushort* indexBuffer;
public:
	int indexCount, vertexCount, maxCount;
	int type;
public:
	DataBuffer(int t);
	virtual ~DataBuffer() {}
public:
	virtual void releaseDatas();
};

#endif

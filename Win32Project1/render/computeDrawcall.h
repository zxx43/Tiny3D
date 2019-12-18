#ifndef COMPUTE_DRAWCALL_H_
#define COMPUTE_DRAWCALL_H_

#include "drawcall.h"

struct ArrayIndirect {
	uint count;
	uint instanceCount;
	uint first;
	uint baseInstance;
};

struct BufferData {
	float* data;
	int size;
	int channel, row;
	BufferData(float* d, int s, int c, int r) :data(d), size(s), channel(c), row(r) {}
};

class ComputeDrawcall :public Drawcall {
private:
	half* vertBuf;
	ArrayIndirect *indirectBuf, *readBuf;
	int channelCount;
	BufferData* exData;
private:
	RenderBuffer* createBuffers(int vertexCount);
public:
	ComputeDrawcall(BufferData* exBuff);
	virtual ~ComputeDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void update();
};

#endif
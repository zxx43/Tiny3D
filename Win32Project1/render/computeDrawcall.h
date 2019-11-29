#ifndef COMPUTE_DRAWCALL_H_
#define COMPUTE_DRAWCALL_H_

#include "drawcall.h"

struct ArrayIndirect {
	uint count;
	uint instanceCount;
	uint first;
	uint baseInstance;
};

class ComputeDrawcall :public Drawcall {
private:
	ArrayIndirect *indirectBuf, *readBuf;
private:
	RenderBuffer* createBuffers(int vertexCount);
public:
	ComputeDrawcall();
	virtual ~ComputeDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void update();
};

#endif
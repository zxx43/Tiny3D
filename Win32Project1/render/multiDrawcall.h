#ifndef MULTI_DRAWCALL_H_
#define MULTI_DRAWCALL_H_

class MultiInstance;

#include "drawcall.h"

class MultiDrawcall: public Drawcall {
private:
	int vertexCount, indexCount, maxObjectCount;
	MultiInstance* multiRef;
	Indirect* readBuf;
	bool mapIndirect;
	GLbitfield barrier;
private:
	RenderBuffer* indirectBuffer;
	int meshCount;
private:
	RenderBuffer* createBuffers(MultiInstance* multi, int vertexCount, int indexCount, int objectCount);
	void updateIndirect(Render* render, RenderState* state);
public:
	MultiDrawcall(MultiInstance* multi);
	virtual ~MultiDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void update(Render* render, RenderState* state);
};

#endif

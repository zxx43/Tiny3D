#ifndef MULTI_DRAWCALL_H_
#define MULTI_DRAWCALL_H_

class MultiInstance;

#include "drawcall.h"

class MultiDrawcall: public Drawcall {
private:
	int vertexCount, indexCount, maxObjectCount;
	MultiInstance* multiRef;
private:
	RenderBuffer* indirectBuffer;
	int meshCount;
private:
	RenderBuffer* dataBuffer2;
	RenderBuffer* indirectBuffer2;
	RenderBuffer* dataBufferDraw;
	RenderBuffer* indirectBufferDraw;
	RenderBuffer* dataBufferPrepare;
	RenderBuffer* indirectBufferPrepare;
private:
	RenderBuffer* createBuffers(MultiInstance* multi, int vertexCount, int indexCount, int maxObjects, RenderBuffer* ref = NULL);
	RenderBuffer* createIndirects(MultiInstance* multi);
	void swapBuffers();
	void updateIndirect(Render* render, RenderState* state);
	void prepareRenderData(Render* render, RenderState* state);
public:
	MultiDrawcall(MultiInstance* multi);
	virtual ~MultiDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void update(Render* render, RenderState* state);
};

#endif

#ifndef MULTI_DRAWCALL_H_
#define MULTI_DRAWCALL_H_

class MultiInstance;

#include "drawcall.h"

class MultiDrawcall: public Drawcall {
private:
	int vertexCount, indexCount;
	MultiInstance* multiRef;
private:
	RenderBuffer* singleBuffer;
	RenderBuffer* billBuffer;
	RenderBuffer* animBuffer;
private:
	RenderBuffer* indirectBuffer;
	RenderBuffer* uniformBuffer;
	int meshCount;
private:
	RenderBuffer* createBuffers(MultiInstance* multi, int vertexCount, int indexCount, uint inIndex, uint outIndex, uint maxCount, RenderBuffer* ref = NULL);
	RenderBuffer* createIndirects(MultiInstance* multi);
	RenderBuffer* createUniforms(MultiInstance* multi);
	void updateIndirect(Render* render, RenderState* state);
	void prepareRenderData(Camera* camera, Render* render, RenderState* state);
public:
	MultiDrawcall(MultiInstance* multi);
	virtual ~MultiDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void update(Camera* camera, Render* render, RenderState* state);
	MultiInstance* getInstance() { return multiRef; }
};

#endif

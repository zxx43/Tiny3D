#ifndef WATER_DRAWCALL_H_
#define WATER_DRAWCALL_H_

#include "drawcall.h"
#include "../mesh/water.h"
#include "../batch/batch.h"

class WaterDrawcall : public Drawcall {
private:
	Water* mesh;
	Batch* data;
	int vertexCount, maxIndexCount, chunkCount;
	RenderBuffer* ssBuffer;
	Indirect* indirectBuffer;
private:
	RenderBuffer* createBuffers();
	RenderBuffer* createSSBuffers();
public:
	WaterDrawcall(Water* water, Batch* batch);
	~WaterDrawcall();
public:
	void update(Camera* camera, Render* render, RenderState* state);
	virtual void draw(Render* render, RenderState* state, Shader* shader);
};

#endif

#ifndef TERRAIN_DRAWCALL_H_
#define TERRAIN_DRAWCALL_H_

#include "drawcall.h"
#include "../mesh/terrain.h"
#include "../batch/batch.h"

class TerrainDrawcall : public Drawcall {
private:
	Terrain* mesh;
	Batch* data;
	int vertexCount, maxIndexCount, chunkCount;
	RenderBuffer* ssBuffer;
	Indirect* indirectBuffer;
private:
	RenderBuffer* createBuffers();
	RenderBuffer* createSSBuffers();
public:
	TerrainDrawcall(Terrain* terrain, Batch* batch);
	virtual ~TerrainDrawcall();
public:
	void update(Camera* camera, Render* render, RenderState* state);
	virtual void draw(Render* render, RenderState* state, Shader* shader);
};

#endif

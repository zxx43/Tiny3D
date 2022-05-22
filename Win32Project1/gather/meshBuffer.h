#ifndef MESH_BUFFER_H_
#define MESH_BUFFER_H_

#include "meshGather.h"

struct RenderBuffer;
class MeshBuffer {
public:
	const static uint GroupDataIndex, BoardDataIndex;
	const static uint VertexIndex, NormalIndex, TangentIndex, TexcoordIndex, EboIndex, BoneIndex, WeightIndex;
	const static uint OutputSlot;
public:
	RenderBuffer* uniforms;
	RenderBuffer* meshVBs;
	RenderBuffer* animVBs;
private:
	const MeshGather* meshDB;
public:
	MeshBuffer(const MeshGather* meshGather);
	~MeshBuffer();
public:
	void release();
};

#endif // !MESH_BUFFER_H_


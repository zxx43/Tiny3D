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
private:
	void addGroupData();
	void addBoardData();
	void addAttriBuff(RenderBuffer* target, int attrInex, int slot, uint type, int vertexCount, int channel, void* data);
	void addIndexBuff(RenderBuffer* target, int eboIndex, uint type, int indexCount, void* data);
public:
	MeshBuffer(const MeshGather* meshGather);
	~MeshBuffer();
public:
	void release();
};

#endif // !MESH_BUFFER_H_


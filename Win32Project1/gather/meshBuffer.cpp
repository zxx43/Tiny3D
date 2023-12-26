#include "meshBuffer.h"
#include "../render/renderBuffer.h"

const uint MeshBuffer::GroupDataIndex = 0;
const uint MeshBuffer::BoardDataIndex = 1;

const uint MeshBuffer::VertexIndex = 0;
const uint MeshBuffer::NormalIndex = 1;
const uint MeshBuffer::TangentIndex = 2;
const uint MeshBuffer::TexcoordIndex = 3;
const uint MeshBuffer::EboIndex = 4;
const uint MeshBuffer::BoneIndex = 5;
const uint MeshBuffer::WeightIndex = 6;

const uint VertexSlot = 0;
const uint NormalSlot = 1;
const uint TexcoordSlot = 2;
const uint TangentSlot = 3;
const uint BoneSlot = 4;
const uint WeightSlot = 5;
const uint MeshBuffer::OutputSlot = 6;

void MeshBuffer::addGroupData() {
	uint groupCount = meshDB->groupData->count;
	int channelCount = meshDB->groupData->channel;
	void* data = meshDB->groupData->buffer;
	if (groupCount > 0) uniforms->setBufferData(GL_UNIFORM_BUFFER, MeshBuffer::GroupDataIndex, GL_INT, groupCount, channelCount, GL_STATIC_DRAW, data);
}

void MeshBuffer::addBoardData() {
	uint boardCount = meshDB->boardData->count;
	int channelCount = meshDB->boardData->channel;
	void* data = meshDB->boardData->buffer;
	if (boardCount > 0) uniforms->setBufferData(GL_UNIFORM_BUFFER, MeshBuffer::BoardDataIndex, GL_FLOAT, boardCount, channelCount, GL_STATIC_DRAW, data);
}

void MeshBuffer::addAttriBuff(RenderBuffer* target, int attrIndex, int slot, uint type, int vertexCount, int channel, void* data) {
	target->setAttribData(GL_ARRAY_BUFFER, attrIndex, slot, type, vertexCount, channel, 1, false, GL_STATIC_DRAW, 0, data);
}

void MeshBuffer::addIndexBuff(RenderBuffer* target, int eboIndex, uint type, int indexCount, void* data) {
	target->setBufferData(GL_ELEMENT_ARRAY_BUFFER, eboIndex, type, indexCount, GL_STATIC_DRAW, data);
}

MeshBuffer::MeshBuffer(const MeshGather* meshGather) {
	meshDB = meshGather;
	uniforms = NULL, meshVBs = NULL, animVBs = NULL;

	uint groupCount = meshDB->groupData->count, boardCount = meshDB->boardData->count;
	if (groupCount > 0 || boardCount > 0) {
		uniforms = new RenderBuffer(2, false);
		addGroupData();
		addBoardData();
		uniforms->unuseAs(GL_UNIFORM_BUFFER);
	}

	uint meshVertexCount = meshDB->meshData->vertexCount, meshIndexCount = meshDB->meshData->indexCount;
	if (meshVertexCount > 0 && meshIndexCount > 0) {
		meshVBs = new RenderBuffer(5);
		addAttriBuff(meshVBs, MeshBuffer::VertexIndex, VertexSlot, GL_FLOAT, meshVertexCount, 3, meshDB->meshData->vertexBuffer);
		addAttriBuff(meshVBs, MeshBuffer::NormalIndex, NormalSlot, GL_HALF_FLOAT, meshVertexCount, 3, meshDB->meshData->normalBuffer);
		addAttriBuff(meshVBs, MeshBuffer::TexcoordIndex, TexcoordSlot, GL_FLOAT, meshVertexCount, 4, meshDB->meshData->texcoordBuffer);
		addAttriBuff(meshVBs, MeshBuffer::TangentIndex, TangentSlot, GL_HALF_FLOAT, meshVertexCount, 3, meshDB->meshData->tangentBuffer);
		addIndexBuff(meshVBs, MeshBuffer::EboIndex, GL_UNSIGNED_SHORT, meshIndexCount, meshDB->meshData->indexBuffer);
		meshVBs->unuse();
	}

	uint animVertexCount = meshDB->animData->vertexCount, animIndexCount = meshDB->animData->indexCount;
	if (animVertexCount > 0 && animIndexCount > 0) {
		animVBs = new RenderBuffer(7);
		addAttriBuff(animVBs, MeshBuffer::VertexIndex, VertexSlot, GL_FLOAT, animVertexCount, 3, meshDB->animData->vertexBuffer);
		addAttriBuff(animVBs, MeshBuffer::NormalIndex, NormalSlot, GL_HALF_FLOAT, animVertexCount, 3, meshDB->animData->normalBuffer);
		addAttriBuff(animVBs, MeshBuffer::TexcoordIndex, TexcoordSlot, GL_FLOAT, animVertexCount, 4, meshDB->animData->texcoordBuffer);
		addAttriBuff(animVBs, MeshBuffer::TangentIndex, TangentSlot, GL_HALF_FLOAT, animVertexCount, 3, meshDB->animData->tangentBuffer);
		addIndexBuff(animVBs, MeshBuffer::EboIndex, GL_UNSIGNED_SHORT, animIndexCount, meshDB->animData->indexBuffer);
		addAttriBuff(animVBs, MeshBuffer::BoneIndex, BoneSlot, GL_UNSIGNED_BYTE, animVertexCount, 4, meshDB->animData->boneids);
		addAttriBuff(animVBs, MeshBuffer::WeightIndex, WeightSlot, GL_HALF_FLOAT, animVertexCount, 4, meshDB->animData->weights);
		animVBs->unuse();
	}
}

MeshBuffer::~MeshBuffer() {
	release();
}

void MeshBuffer::release() {
	if (uniforms) delete uniforms; uniforms = NULL;
	if (meshVBs) delete meshVBs; meshVBs = NULL;
	if (animVBs) delete animVBs; animVBs = NULL;
}
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

MeshBuffer::MeshBuffer(const MeshGather* meshGather) {
	meshDB = meshGather;
	uniforms = NULL, meshVBs = NULL, animVBs = NULL;

	uint groupCount = meshDB->groupData->count, boardCount = meshDB->boardData->count;
	if (groupCount > 0 || boardCount > 0) {
		uniforms = new RenderBuffer(2, false);
		if (groupCount > 0) uniforms->setBufferData(GL_UNIFORM_BUFFER, MeshBuffer::GroupDataIndex, GL_INT, groupCount, meshDB->groupData->channel, GL_STATIC_DRAW, meshDB->groupData->buffer);
		if (boardCount > 0) uniforms->setBufferData(GL_UNIFORM_BUFFER, MeshBuffer::BoardDataIndex, GL_FLOAT, boardCount, meshDB->boardData->channel, GL_STATIC_DRAW, meshDB->boardData->buffer);
		uniforms->unuseAs(GL_UNIFORM_BUFFER);
	}

	uint meshVertexCount = meshDB->meshData->vertexCount, meshIndexCount = meshDB->meshData->indexCount;
	if (meshVertexCount > 0 && meshIndexCount > 0) {
		meshVBs = new RenderBuffer(5);
		meshVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::VertexIndex, VertexSlot, GL_FLOAT, meshVertexCount, 3, 1, false, GL_STATIC_DRAW, 0, meshDB->meshData->vertexBuffer);
		meshVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::NormalIndex, NormalSlot, GL_HALF_FLOAT, meshVertexCount, 3, 1, false, GL_STATIC_DRAW, 0, meshDB->meshData->normalBuffer);
		meshVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::TexcoordIndex, TexcoordSlot, GL_FLOAT, meshVertexCount, 4, 1, false, GL_STATIC_DRAW, 0, meshDB->meshData->texcoordBuffer);
		meshVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::TangentIndex, TangentSlot, GL_HALF_FLOAT, meshVertexCount, 3, 1, false, GL_STATIC_DRAW, 0, meshDB->meshData->tangentBuffer);
		meshVBs->setBufferData(GL_ELEMENT_ARRAY_BUFFER, MeshBuffer::EboIndex, GL_UNSIGNED_SHORT, meshIndexCount, GL_STATIC_DRAW, meshDB->meshData->indexBuffer);
		meshVBs->unuse();
	}

	uint animVertexCount = meshDB->animData->vertexCount, animIndexCount = meshDB->animData->indexCount;
	if (animVertexCount > 0 && animIndexCount > 0) {
		animVBs = new RenderBuffer(7);
		animVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::VertexIndex, VertexSlot, GL_FLOAT, animVertexCount, 3, 1, false, GL_STATIC_DRAW, 0, meshDB->animData->vertexBuffer);
		animVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::NormalIndex, NormalSlot, GL_HALF_FLOAT, animVertexCount, 3, 1, false, GL_STATIC_DRAW, 0, meshDB->animData->normalBuffer);
		animVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::TexcoordIndex, TexcoordSlot, GL_FLOAT, animVertexCount, 4, 1, false, GL_STATIC_DRAW, 0, meshDB->animData->texcoordBuffer);
		animVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::TangentIndex, TangentSlot, GL_HALF_FLOAT, animVertexCount, 3, 1, false, GL_STATIC_DRAW, 0, meshDB->animData->tangentBuffer);
		animVBs->setBufferData(GL_ELEMENT_ARRAY_BUFFER, MeshBuffer::EboIndex, GL_UNSIGNED_SHORT, animIndexCount, GL_STATIC_DRAW, meshDB->animData->indexBuffer);
		animVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::BoneIndex, BoneSlot, GL_UNSIGNED_BYTE, animVertexCount, 4, 1, false, GL_STATIC_DRAW, 0, meshDB->animData->boneids);
		animVBs->setAttribData(GL_ARRAY_BUFFER, MeshBuffer::WeightIndex, WeightSlot, GL_HALF_FLOAT, animVertexCount, 4, 1, false, GL_STATIC_DRAW, 0, meshDB->animData->weights);
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
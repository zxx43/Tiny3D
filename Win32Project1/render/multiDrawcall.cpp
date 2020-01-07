#include "multiDrawcall.h"
#include "../instance/multiInstance.h"
#include "../render/render.h"

// Attribute slots
const uint VertexSlot = 0;
const uint NormalSlot = 1;
const uint TexcoordSlot = 2;
const uint TexidSlot = 3;
const uint ColorSlot = 4;
const uint TangentSlot = 5;
const uint PositionSlot = 6;

// VBO index
const uint VertexIndex = 0;
const uint NormalIndex = 1;
const uint TexcoordIndex = 2;
const uint TexidIndex = 3;
const uint ColorIndex = 4;
const uint TangentIndex = 5;
const uint PositionIndex = 6;
const uint Index = 7;
const uint PositionOutIndex = 8;

// Indirect vbo index
const uint IndirectNormalIndex = 0;
const uint IndirectSingleIndex = 1;

MultiDrawcall::MultiDrawcall(MultiInstance* multi) :Drawcall() {
	multiRef = multi;
	vertexCount = multiRef->vertexCount;
	indexCount = multiRef->indexCount;
	maxObjectCount = multiRef->maxInstance;

	readBuf = (Indirect*)malloc(multiRef->indirectCount * sizeof(Indirect));
	dataBuffer = createBuffers(multiRef, vertexCount, indexCount, maxObjectCount);
	
	indirectBuffer = new RenderBuffer(2, false);
	indirectBuffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectNormalIndex, GL_ONE, multiRef->normalCount * sizeof(Indirect), GL_DYNAMIC_DRAW, NULL);
	indirectBuffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectSingleIndex, GL_ONE, multiRef->singleCount * sizeof(Indirect), GL_DYNAMIC_DRAW, NULL);

	setType(MULTI_DC);
	multiRef->releaseInstanceData();
}

MultiDrawcall::~MultiDrawcall() {
	if (readBuf) free(readBuf);
	if (indirectBuffer) free(indirectBuffer);
}

RenderBuffer* MultiDrawcall::createBuffers(MultiInstance* multi, int vertexCount, int indexCount, int objectCount) {
	RenderBuffer* buffer = new RenderBuffer(9);
	buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex, VertexSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, multi->vertexBuffer);
	buffer->setAttribData(GL_ARRAY_BUFFER, NormalIndex, NormalSlot, GL_HALF_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, multi->normalBuffer);
	buffer->setAttribData(GL_ARRAY_BUFFER, TexcoordIndex, TexcoordSlot, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, multi->texcoordBuffer);
	buffer->setAttribData(GL_ARRAY_BUFFER, TexidIndex, TexidSlot, GL_FLOAT, vertexCount, 2, 1, false, GL_STATIC_DRAW, 0, multi->texidBuffer);
	buffer->setAttribData(GL_ARRAY_BUFFER, ColorIndex, ColorSlot, GL_UNSIGNED_BYTE, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, multi->colorBuffer);
	buffer->setAttribData(GL_ARRAY_BUFFER, TangentIndex, TangentSlot, GL_HALF_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, multi->tangentBuffer);
	buffer->setBufferData(GL_ELEMENT_ARRAY_BUFFER, Index, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, multi->indexBuffer);

	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, PositionIndex, GL_FLOAT, objectCount, 16, GL_DYNAMIC_DRAW, NULL);
	buffer->setAttribData(GL_SHADER_STORAGE_BUFFER, PositionOutIndex, PositionSlot, GL_FLOAT, objectCount, 4, 4, false, GL_STREAM_DRAW, 1, NULL);
	buffer->useAs(PositionOutIndex, GL_ARRAY_BUFFER);
	buffer->setAttrib(PositionOutIndex);
	buffer->unuse();
	return buffer;
}

void MultiDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	indirectBuffer->updateBufferMap(GL_SHADER_STORAGE_BUFFER, IndirectNormalIndex, multiRef->normalCount * sizeof(Indirect), (void*)multiRef->indirectsNormal);
	indirectBuffer->updateBufferMap(GL_SHADER_STORAGE_BUFFER, IndirectSingleIndex, multiRef->singleCount * sizeof(Indirect), (void*)multiRef->indirectsSingle);
	//updateNormal(render, state);
	//updateSingle(render, state);

	if (frame < DELAY_FRAME) frame++;
	else {
		dataBuffer->use();
		dataBuffer->setShaderBase(PositionIndex, 1);
		dataBuffer->setShaderBase(PositionOutIndex, 2);
		indirectBuffer->setShaderBase(IndirectNormalIndex, 3);
		indirectBuffer->setShaderBase(IndirectSingleIndex, 4);
		
		render->useShader(state->shaderMulti);
		glDispatchCompute(objectCount, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		render->useShader(shader);
		indirectBuffer->useAs(IndirectNormalIndex, GL_DRAW_INDIRECT_BUFFER);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->normalCount, 0);

		if (state->pass < COLOR_PASS) render->setCullMode(CULL_BACK);
		else render->setCullState(false);
		indirectBuffer->useAs(IndirectSingleIndex, GL_DRAW_INDIRECT_BUFFER);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->singleCount, 0);
	}
}

void MultiDrawcall::update(Render* render, RenderState* state) {
	multiRef->updateTransform();
	objectCount = multiRef->instanceCount;
	dataBuffer->updateBufferData(PositionIndex, objectCount, (void*)(multiRef->transforms));
}

void MultiDrawcall::updateNormal(Render* render, RenderState* state) {
	indirectBuffer->useAs(IndirectNormalIndex, GL_SHADER_STORAGE_BUFFER);
	indirectBuffer->setShaderBase(IndirectNormalIndex, 1);
	render->useShader(state->shaderFlush);
	render->setShaderUintv(state->shaderFlush, "uBases", multiRef->normalCount, multiRef->normalBases);
	glDispatchCompute(multiRef->normalCount, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void MultiDrawcall::updateSingle(Render* render, RenderState* state) {
	indirectBuffer->useAs(IndirectSingleIndex, GL_SHADER_STORAGE_BUFFER);
	indirectBuffer->setShaderBase(IndirectSingleIndex, 1);
	render->useShader(state->shaderFlush);
	render->setShaderUintv(state->shaderFlush, "uBases", multiRef->singleCount, multiRef->singleBases);
	glDispatchCompute(multiRef->singleCount, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


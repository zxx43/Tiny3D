#include "computeDrawcall.h"
#include "render.h"

// Attribute slots
const uint VertexSlot = 0;
const uint NormalSlot = 1;

// VBO index
const uint PointsOutIndex = 0;
const uint IndirectBufIndex = 1;

const uint MaxSize = 1024;

ComputeDrawcall::ComputeDrawcall() :Drawcall() {
	indirectBuf = (ArrayIndirect*)malloc(sizeof(ArrayIndirect));
	indirectBuf->count = 0;
	indirectBuf->instanceCount = 1;
	indirectBuf->first = 0;
	indirectBuf->baseInstance = 0;

	readBuf = (ArrayIndirect*)malloc(sizeof(ArrayIndirect));
	readBuf->count = 0;
	readBuf->instanceCount = 0;
	readBuf->first = 0;
	readBuf->baseInstance = 0;

	dataBuffer = createBuffers(MaxSize * MaxSize);
}

ComputeDrawcall::~ComputeDrawcall() {
	free(indirectBuf);
	free(readBuf);
}

RenderBuffer* ComputeDrawcall::createBuffers(int vertexCount) {
	RenderBuffer* buffer = new RenderBuffer(2);
	buffer->setAttribData(GL_SHADER_STORAGE_BUFFER, PointsOutIndex, VertexSlot, GL_FLOAT, vertexCount, 4, 2, false, GL_STREAM_DRAW, 1, NULL);
	buffer->useAs(PointsOutIndex, GL_ARRAY_BUFFER);
	buffer->setAttrib(PointsOutIndex);
	buffer->setBufferData(GL_DRAW_INDIRECT_BUFFER, IndirectBufIndex, GL_ONE, sizeof(ArrayIndirect), GL_DYNAMIC_DRAW, indirectBuf);
	return buffer;
}

void ComputeDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	dataBuffer->use();

	dataBuffer->setShaderBase(PointsOutIndex, 1);
	dataBuffer->setShaderBase(IndirectBufIndex, 2);

	render->useShader(state->shaderCompute);
	glDispatchCompute(MaxSize, MaxSize, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

	render->useShader(shader);
	glDrawArraysIndirect(GL_TRIANGLES, 0);
}

void ComputeDrawcall::update() {
	indirectBuf->count = 0;
	dataBuffer->updateBufferMap(GL_DRAW_INDIRECT_BUFFER, IndirectBufIndex, sizeof(ArrayIndirect), (void*)indirectBuf);
}
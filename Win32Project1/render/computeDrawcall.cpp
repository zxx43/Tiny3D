#include "computeDrawcall.h"
#include "render.h"

// Attribute slots
const uint VertexSlot = 0;
const uint PositionSlot = 1;

// VBO index
const uint VertexIndex = 0;
const uint PositionOutIndex = 1;
const uint IndirectBufIndex = 2;

ComputeDrawcall::ComputeDrawcall(BufferData* exBuff, int max) :Drawcall() {
	setType(COMPUTE_DC);
	channelCount = 3;

	indirectBuf = (ArrayIndirect*)malloc(sizeof(ArrayIndirect));
	indirectBuf->count = channelCount;
	indirectBuf->instanceCount = 0;
	indirectBuf->first = 0;
	indirectBuf->baseInstance = 0;

	readBuf = (ArrayIndirect*)malloc(sizeof(ArrayIndirect));
	memset(readBuf, 0, sizeof(ArrayIndirect));

	vertBuf = (half*)malloc(6 * sizeof(half));
	float buf[6] = { -1.0, 0.0,
					  0.0, 1.0,
					  1.0, 0.0 };
	Float2Halfv(buf, vertBuf, 6);

	exData = exBuff;
	maxCount = max;
	dispatchCount = maxCount / COMP_GROUPE_SIZE;
	dataBuffer = createBuffers(maxCount * maxCount);
}

ComputeDrawcall::~ComputeDrawcall() {
	free(indirectBuf);
	free(readBuf);
	free(vertBuf);
}

RenderBuffer* ComputeDrawcall::createBuffers(int objCount) {
	RenderBuffer* buffer = new RenderBuffer(3);
	buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex, VertexSlot, GL_HALF_FLOAT, channelCount, 2, 1, false, GL_STATIC_DRAW, 0, vertBuf);
	buffer->setAttribData(GL_SHADER_STORAGE_BUFFER, PositionOutIndex, PositionSlot, GL_FLOAT, objCount, 4, 4, false, GL_STREAM_DRAW, 1, NULL);
	buffer->useAs(PositionOutIndex, GL_ARRAY_BUFFER);
	buffer->setAttrib(PositionOutIndex);
	buffer->setBufferData(GL_DRAW_INDIRECT_BUFFER, IndirectBufIndex, GL_ONE, sizeof(ArrayIndirect), GL_DYNAMIC_DRAW, indirectBuf);
	buffer->unuse();
	return buffer;
}

void ComputeDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	dataBuffer->use();

	dataBuffer->setShaderBase(PositionOutIndex, 1);
	dataBuffer->setShaderBase(IndirectBufIndex, 2);

	render->useShader(state->shaderCompute);
	state->shaderCompute->setFloat("fullSize", maxCount);

	glDispatchCompute(dispatchCount, dispatchCount, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

	render->useShader(shader);
	glDrawArraysIndirect(GL_TRIANGLES, 0);
}

void ComputeDrawcall::update() {
	indirectBuf->instanceCount = 0;
	dataBuffer->updateBufferMap(GL_DRAW_INDIRECT_BUFFER, IndirectBufIndex, sizeof(ArrayIndirect), (void*)indirectBuf);
}
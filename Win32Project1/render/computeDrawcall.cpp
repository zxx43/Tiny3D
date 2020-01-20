#include "computeDrawcall.h"
#include "render.h"

// Attribute slots
const uint VertexSlot = 0;
const uint ExSlot = 1;
const uint PositionSlot = 2;

// VBO index
const uint VertexIndex = 0;
const uint ExIndex = 1;
const uint PositionOutIndex = 2;
const uint IndirectBufIndex = 3;

const uint MaxSize = 512;

ComputeDrawcall::ComputeDrawcall(BufferData* exBuff) :Drawcall() {
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
	dataBuffer = createBuffers(MaxSize * MaxSize);
}

ComputeDrawcall::~ComputeDrawcall() {
	free(indirectBuf);
	free(readBuf);
	free(vertBuf);
}

RenderBuffer* ComputeDrawcall::createBuffers(int objCount) {
	RenderBuffer* buffer = new RenderBuffer(4);
	buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex, VertexSlot, GL_HALF_FLOAT, channelCount, 2, 1, false, GL_STATIC_DRAW, 0, vertBuf);
	buffer->setAttribData(GL_SHADER_STORAGE_BUFFER, ExIndex, ExSlot, GL_FLOAT, exData->size / (exData->channel * exData->row), 
		exData->channel, exData->row, false, GL_STATIC_DRAW, 0, exData->data);
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
	dataBuffer->setShaderBase(ExIndex, 2);
	dataBuffer->setShaderBase(IndirectBufIndex, 3);

	render->useShader(state->shaderCompute);
	state->shaderCompute->setFloat("fullSize", MaxSize);

	static int dispatchSize = MaxSize / COMP_GROUPE_SIZE;
	glDispatchCompute(dispatchSize, dispatchSize, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

	render->useShader(shader);
	glDrawArraysIndirect(GL_TRIANGLES, 0);
}

void ComputeDrawcall::update() {
	indirectBuf->instanceCount = 0;
	dataBuffer->updateBufferMap(GL_DRAW_INDIRECT_BUFFER, IndirectBufIndex, sizeof(ArrayIndirect), (void*)indirectBuf);
}
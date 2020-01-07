#include "instanceDrawcall.h"
#include "../instance/instance.h"
#include "render.h"

// Attribute slots
const uint BillboardVertexSlot = 0;
const uint BillboardTexcoordSlot = 1;
const uint BillboardInfoSlot = 2;
//----------------------------------------
const uint VertexSlot = 0;
const uint NormalSlot = 1;
const uint TexcoordSlot = 2;
const uint TexidSlot = 3;
const uint ColorSlot = 4;
const uint TangentSlot = 5;
const uint PositionSlot = 6;

// VBO index
const uint BillboardVertexIndex = 0;
const uint BillboardTexcoordIndex = 1;
const uint BillboardInfoIndex = 2;
const uint BillboardIndex = 3;
//----------------------------------------
const uint VertexIndex = 0;
const uint NormalIndex = 1;
const uint TexcoordIndex = 2;
const uint TexidIndex = 3;
const uint ColorIndex = 4;
const uint TangentIndex = 5;
const uint PositionIndex = 6;
const uint Index = 7;
const uint PositionOutIndex = 8;
const uint IndirectBufIndex = 9;

InstanceDrawcall::InstanceDrawcall(Instance* instance) :Drawcall() {
	instanceRef = instance;
	vertexCount = instanceRef->vertexCount;
	indexCount = instanceRef->indexCount;
	setBillboard(instanceRef->isBillboard);

	objectCount = instanceRef->maxInstanceCount;
	objectToPrepare = 0;

	indirectBuf = (Indirect*)malloc(sizeof(Indirect));
	indirectBuf->count = indexCount;
	indirectBuf->primCount = 0;
	indirectBuf->firstIndex = 0;
	indirectBuf->baseVertex = 0;
	indirectBuf->baseInstance = 0;

	readBuf = (Indirect*)malloc(sizeof(Indirect));
	memset(readBuf, 0, sizeof(Indirect));

	dataBuffer = createBuffers(instanceRef, vertexCount, indexCount);

	setType(INSTANCE_DC);
	instanceRef->releaseInstanceData();
}

InstanceDrawcall::~InstanceDrawcall() {
	free(indirectBuf);
	free(readBuf);
}

RenderBuffer* InstanceDrawcall::createBuffers(Instance* instance, int vertexCount, int indexCount) {
	RenderBuffer* buffer = NULL;
	GLenum draw = GL_DYNAMIC_DRAW;
	if (isBillboard()) {
		buffer = new RenderBuffer(4);
		buffer->setAttribData(GL_ARRAY_BUFFER, BillboardVertexIndex, BillboardVertexSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->vertexBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, BillboardTexcoordIndex, BillboardTexcoordSlot, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, instanceRef->texcoordBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, BillboardInfoIndex, BillboardInfoSlot, GL_HALF_FLOAT, objectCount, 3, 2, false, draw, 1, instanceRef->billboards);
		buffer->setBufferData(GL_ELEMENT_ARRAY_BUFFER, BillboardIndex, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, instanceRef->indexBuffer);
	} else {
		buffer = new RenderBuffer(10);
		buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex, VertexSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->vertexBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, NormalIndex, NormalSlot, GL_HALF_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->normalBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexcoordIndex, TexcoordSlot, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, instanceRef->texcoordBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexidIndex, TexidSlot, GL_FLOAT, vertexCount, 2, 1, false, GL_STATIC_DRAW, 0, instanceRef->texidBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, ColorIndex, ColorSlot, GL_UNSIGNED_BYTE, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->colorBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TangentIndex, TangentSlot, GL_HALF_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->tangentBuffer);
		buffer->setBufferData(GL_ELEMENT_ARRAY_BUFFER, Index, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, instanceRef->indexBuffer);
		
		buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, PositionIndex, GL_FLOAT, objectCount, 16, draw, instanceRef->modelTransform);
		buffer->setAttribData(GL_SHADER_STORAGE_BUFFER, PositionOutIndex, PositionSlot, GL_FLOAT, objectCount, 4, 4, false, GL_STREAM_DRAW, 1, NULL);
		buffer->useAs(PositionOutIndex, GL_ARRAY_BUFFER);
		buffer->setAttrib(PositionOutIndex);
		buffer->setBufferData(GL_DRAW_INDIRECT_BUFFER, IndirectBufIndex, GL_ONE, sizeof(Indirect), GL_DYNAMIC_DRAW, indirectBuf);
	}
	buffer->unuse();
	return buffer;
}

void InstanceDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	if (frame < DELAY_FRAME) frame++;
	else {
		dataBuffer->use();

		if (instanceRef->instanceMesh->singleFaces.size() == 0)
			render->setCullState(state->enableCull);
		else 
			render->setCullState(false);

		if (isBillboard()) {
			render->useShader(shader);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0, objectToPrepare);
		} else {
			dataBuffer->setShaderBase(PositionIndex, 1);
			dataBuffer->setShaderBase(PositionOutIndex, 2);
			dataBuffer->setShaderBase(IndirectBufIndex, 3);

			render->useShader(state->shaderCompute);
			glDispatchCompute(ceilf((float)objectToPrepare / WORKGROUPE_SIZE), 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

			render->useShader(shader);
			glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0);

			/*
			dataBuffer->readBufferData(GL_DRAW_INDIRECT_BUFFER, IndirectBufIndex, sizeof(Indirect), readBuf);
			if (state->pass == COLOR_PASS)
				printf("culled %d\n", objectToPrepare - (int)readBuf->primCount);
			//*/
		}
	}
}

void InstanceDrawcall::updateInstances(Instance* instance, int pass) {
	if (isBillboard()) 
		dataBuffer->updateBufferData(BillboardInfoIndex, objectToPrepare, (void*)(instance->billboards));
	else {
		indirectBuf->primCount = 0;
		dataBuffer->updateBufferMap(GL_DRAW_INDIRECT_BUFFER, IndirectBufIndex, sizeof(Indirect), (void*)indirectBuf);
		dataBuffer->updateBufferData(PositionIndex, objectToPrepare, (void*)(instance->modelTransform));
	}
}

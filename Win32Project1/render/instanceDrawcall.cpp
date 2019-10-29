#include "instanceDrawcall.h"
#include "../instance/instance.h"
#include "render.h"

const uint BillboardVertexSlot = 0;
const uint BillboardTexcoordSlot = 1;
const uint BillboardPositionSlot = 2;
const uint BillboardInfoSlot = 3;
const uint BillboardIndexSlot = 4;

const uint VertexSlot = 0;
const uint NormalSlot = 1;
const uint TexcoordSlot = 2;
const uint TexidSlot = 3;
const uint ColorSlot = 4;
const uint TangentSlot = 5;
const uint PositionSlot = 6;
const uint IndexSlot = 7;

InstanceDrawcall::InstanceDrawcall(Instance* instance) :Drawcall() {
	instanceRef = instance;
	dynDC = instanceRef->isDynamic;
	isSimple = instanceRef->isSimple;
	isGrass = instanceRef->isGrass;
	vertexCount = instanceRef->vertexCount;
	indexCount = instanceRef->indexCount;

	//printf("create mesh vao: %s?\n", instanceRef->instanceMesh->getName().data());

	objectCount = dynDC ? instanceRef->maxInstanceCount : instanceRef->instanceCount;
	objectToPrepare = 0, objectToDraw = 0;
	if (!dynDC) {
		objectToPrepare = instanceRef->instanceCount;
		objectToDraw = instanceRef->instanceCount;
	}

	setBillboard(instanceRef->isBillboard);
	
	doubleBuffer = false;
	dataBuffer = createBuffers(instanceRef, dynDC, vertexCount, indexCount, NULL);
	if (dynDC && doubleBuffer)
		dataBuffer2 = createBuffers(instanceRef, dynDC, vertexCount, indexCount, dataBuffer);
	else
		dataBuffer2 = NULL;

	dataBufferDraw = dataBuffer;
	if (dataBuffer2)
		dataBufferPrepare = dataBuffer2;
	else
		dataBufferPrepare = dataBuffer;

	setType(INSTANCE_DC);

	instanceRef->releaseInstanceData();
	//printf("create mesh vao: %s!\n", instanceRef->instanceMesh->getName().data());
}

InstanceDrawcall::~InstanceDrawcall() {
	if(dataBuffer2) delete dataBuffer2;
}

RenderBuffer* InstanceDrawcall::createBuffers(Instance* instance, bool dyn, int vertexCount, int indexCount, RenderBuffer* dupBuf) {
	RenderBuffer* buffer = NULL;
	GLenum draw = dyn ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	if (isBillboard()) {
		buffer = new RenderBuffer(5);
		if (!dupBuf) {
			buffer->setAttribData(BillboardVertexSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->vertexBuffer);
			buffer->setAttribData(BillboardTexcoordSlot, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, instanceRef->texcoordBuffer);
		} else {
			buffer->setAttribData(BillboardVertexSlot, dupBuf->streamDatas[BillboardVertexSlot]);
			buffer->setAttribData(BillboardTexcoordSlot, dupBuf->streamDatas[BillboardTexcoordSlot]);
		}
		buffer->setAttribData(BillboardPositionSlot, GL_FLOAT, objectCount, 3, 1, false, draw, 1, instanceRef->positions);
		buffer->setAttribData(BillboardInfoSlot, GL_FLOAT, objectCount, 4, 1, false, draw, 1, instanceRef->billboards);

		if(!dupBuf) buffer->setIndexData(BillboardIndexSlot, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, instanceRef->indexBuffer);
		else buffer->setIndexData(BillboardIndexSlot, dupBuf->streamDatas[BillboardIndexSlot]);
	} else {
		buffer = new RenderBuffer(8);
		if (!dupBuf) {
			buffer->setAttribData(VertexSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->vertexBuffer);
			buffer->setAttribData(NormalSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->normalBuffer);
			buffer->setAttribData(TexcoordSlot, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, instanceRef->texcoordBuffer);
			buffer->setAttribData(TexidSlot, GL_FLOAT, vertexCount, 2, 1, false, GL_STATIC_DRAW, 0, instanceRef->texidBuffer);
			buffer->setAttribData(ColorSlot, GL_UNSIGNED_BYTE, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->colorBuffer);
			buffer->setAttribData(TangentSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->tangentBuffer);
			buffer->setIndexData(IndexSlot, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, instanceRef->indexBuffer);
		} else {
			buffer->setAttribData(VertexSlot, dupBuf->streamDatas[VertexSlot]);
			buffer->setAttribData(NormalSlot, dupBuf->streamDatas[NormalSlot]);
			buffer->setAttribData(TexcoordSlot, dupBuf->streamDatas[TexcoordSlot]);
			buffer->setAttribData(TexidSlot, dupBuf->streamDatas[TexidSlot]);
			buffer->setAttribData(ColorSlot, dupBuf->streamDatas[ColorSlot]);
			buffer->setAttribData(TangentSlot, dupBuf->streamDatas[TangentSlot]);
			buffer->setIndexData(IndexSlot, dupBuf->streamDatas[IndexSlot]);
		}
		if (!isSimple) buffer->setAttribData(PositionSlot, GL_FLOAT, objectCount, 4, 3, false, draw, 1, instanceRef->modelMatrices);
		else buffer->setAttribData(PositionSlot, GL_FLOAT, objectCount, 4, 1, false, draw, 1, instanceRef->modelMatrices);
	}
	buffer->unuse();
	return buffer;
}

void InstanceDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	if (frame < DELAY_FRAME) frame++;
	else {
		dataBufferDraw->use();
		if (isGrass) {
			float* boundInfo = instanceRef->instanceMesh->bounding;
			if (boundInfo) {
				shader->setVector3v("boundPos", boundInfo);
				shader->setVector3v("boundScl", boundInfo + 3);
			}
		}

		if (instanceRef->instanceMesh->singleFaces.size() == 0) {
			render->setCullState(state->enableCull);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0, objectToDraw);
		}
		else if (instanceRef->instanceMesh->normalFaces.size() == 0) {
			render->setCullState(false);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0, objectToDraw);
		}
		else {
			render->setCullState(false);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0, objectToDraw);
			/*
			for (uint i = 0; i < instanceRef->instanceMesh->normalFaces.size(); i++) {
				FaceBuf* fb = instanceRef->instanceMesh->normalFaces[i];
				render->setCullState(state->enableCull);
				glDrawElementsInstancedBaseVertex(GL_TRIANGLES, fb->count, GL_UNSIGNED_SHORT, 0, objectToDraw, fb->start);
			}
			for (uint i = 0; i < instanceRef->instanceMesh->singleFaces.size(); i++) {
				FaceBuf* fb = instanceRef->instanceMesh->singleFaces[i];
				render->setCullState(false);
				glDrawElementsInstancedBaseVertex(GL_TRIANGLES, fb->count, GL_UNSIGNED_SHORT, 0, objectToDraw, fb->start);
			}
			//*/
		}
	}

	if (dynDC && doubleBuffer) objectToDraw = objectToPrepare;
}

void InstanceDrawcall::updateInstances(Instance* instance, int pass) {
	if (!dynDC) return;
	
	if (doubleBuffer) {
		dataBufferDraw = dataBufferDraw == dataBuffer ? dataBuffer2 : dataBuffer;
		dataBufferPrepare = dataBufferPrepare == dataBuffer ? dataBuffer2 : dataBuffer; 
	} else {
		dataBufferDraw = dataBuffer;
		dataBufferPrepare = dataBuffer;
		objectToDraw = objectToPrepare;
	}

	if (!isBillboard()) 
		dataBufferPrepare->updateAttribData(PositionSlot, objectToPrepare, (void*)(instance->modelMatrices));
	else {
		dataBufferPrepare->updateAttribData(BillboardPositionSlot, objectToPrepare, (void*)(instance->positions));
		dataBufferPrepare->updateAttribData(BillboardInfoSlot, objectToPrepare, (void*)(instance->billboards));
	}
}

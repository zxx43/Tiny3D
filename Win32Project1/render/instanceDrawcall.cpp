#include "instanceDrawcall.h"
#include "../instance/instance.h"

InstanceDrawcall::InstanceDrawcall(Instance* instance) :Drawcall() {
	instanceRef = instance;
	dynDC = instanceRef->isDynamic;
	isSimple = instanceRef->isSimple;
	vertexCount = instanceRef->vertexCount;
	indexCount = instanceRef->indexCount;
	indexed = indexCount > 0 ? true : false;

	objectCount = dynDC ? MAX_INSTANCE_COUNT : instanceRef->instanceCount;
	objectToPrepare = 0, objectToDraw = 0;
	if (!dynDC) {
		objectToPrepare = instanceRef->instanceCount;
		objectToDraw = instanceRef->instanceCount;
	}

	setBillboard(instanceRef->isBillboard);
	
	dataBuffer = createBuffers(instanceRef, dynDC, indexed, vertexCount, indexCount);
	if (dynDC)
		dataBuffer2 = createBuffers(instanceRef, dynDC, indexed, vertexCount, indexCount);
	else
		dataBuffer2 = NULL;

	dataBufferDraw = dataBuffer;
	if (dataBuffer2)
		dataBufferPrepare = dataBuffer2;
	else
		dataBufferPrepare = dataBuffer;

	setType(INSTANCE_DC);

	instanceRef->releaseInstanceData();
}

InstanceDrawcall::~InstanceDrawcall() {
	if(dataBuffer2) delete dataBuffer2;
}

RenderBuffer* InstanceDrawcall::createBuffers(Instance* instance, bool dyn, bool indexed, int vertexCount, int indexCount) {
	RenderBuffer* buffer = NULL;
	GLenum draw = dyn ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	if (isBillboard()) {
		buffer = new RenderBuffer(indexed ? 5 : 4);
		buffer->setAttribData(0, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->vertexBuffer);
		buffer->setAttribData(1, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, instanceRef->texcoordBuffer);
		buffer->setAttribData(2, GL_FLOAT, objectCount, 3, 1, false, draw, 1, instanceRef->positions);
		buffer->setAttribData(3, GL_FLOAT, objectCount, 4, 1, false, draw, 1, instanceRef->billboards);
		if (indexed)
			buffer->setIndexData(4, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, instanceRef->indexBuffer);
	}
	else {
		buffer = new RenderBuffer(indexed ? 6 : 5);
		buffer->setAttribData(0, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->vertexBuffer);
		buffer->setAttribData(1, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->normalBuffer);
		buffer->setAttribData(2, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, instanceRef->texcoordBuffer);
		buffer->setAttribData(3, GL_UNSIGNED_BYTE, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instanceRef->colorBuffer);
		if (!isSimple)
			buffer->setAttribData(4, GL_FLOAT, objectCount, 4, 3, false, draw, 1, instanceRef->modelMatrices);
		else
			buffer->setAttribData(4, GL_FLOAT, objectCount, 4, 1, false, draw, 1, instanceRef->modelMatrices);
		if (indexed)
			buffer->setIndexData(5, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, instanceRef->indexBuffer);
	}
	buffer->unuse();
	return buffer;
}

void InstanceDrawcall::draw(Shader* shader,int pass) {
	dataBufferDraw->use();
	if(indexed)
		glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0, objectToDraw);
	else
		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, objectToDraw);

	if(dynDC) objectToDraw = objectToPrepare;
}

void InstanceDrawcall::updateInstances(Instance* instance, int pass) {
	if (!dataBuffer2) return;
	
	dataBufferDraw = dataBufferDraw == dataBuffer ? dataBuffer2 : dataBuffer;
	dataBufferPrepare = dataBufferPrepare == dataBuffer ? dataBuffer2 : dataBuffer;

	if (!isBillboard()) 
		dataBufferPrepare->updateAttribData(4, objectToPrepare, (void*)(instance->modelMatrices));
	else {
		dataBufferPrepare->updateAttribData(2, objectToPrepare, (void*)(instance->positions));
		dataBufferPrepare->updateAttribData(3, objectToPrepare, (void*)(instance->billboards));
	}
}

#include "instanceDrawcall.h"
#include "../instance/instance.h"

InstanceDrawcall::InstanceDrawcall(Instance* instance) :Drawcall() {
	vertexCount = instance->vertexCount;
	indexCount = instance->indexCount;
	indexed = indexCount > 0 ? true : false;
	objectCount = MAX_INSTANCE_COUNT;
	objectToDraw = 0;
	setBillboard(instance->isBillboard);

	if (isBillboard()) {
		dataBuffer = new RenderBuffer(indexed ? 5 : 4);
		dataBuffer->setAttribData(0, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instance->vertexBuffer);
		dataBuffer->setAttribData(1, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, instance->texcoordBuffer);
		dataBuffer->setAttribData(2, GL_FLOAT, objectCount, 3, 1, false, GL_DYNAMIC_DRAW, 1, instance->positions);
		dataBuffer->setAttribData(3, GL_FLOAT, objectCount, 4, 1, false, GL_DYNAMIC_DRAW, 1, instance->billboards);
		if (indexed)
			dataBuffer->setIndexData(4, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, instance->indexBuffer);
		dataBuffer->unuse();
	} else {
		dataBuffer = new RenderBuffer(indexed ? 6 : 5);
		dataBuffer->setAttribData(0, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instance->vertexBuffer);
		dataBuffer->setAttribData(1, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instance->normalBuffer);
		dataBuffer->setAttribData(2, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, instance->texcoordBuffer);
		dataBuffer->setAttribData(3, GL_UNSIGNED_BYTE, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, instance->colorBuffer);
		dataBuffer->setAttribData(4, GL_FLOAT, objectCount, 4, 3, false, GL_DYNAMIC_DRAW, 1, instance->modelMatrices);
		if (indexed)
			dataBuffer->setIndexData(5, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, instance->indexBuffer);
		dataBuffer->unuse();
	}

	setType(INSTANCE_DC);
}

InstanceDrawcall::~InstanceDrawcall() {
	
}

void InstanceDrawcall::draw(Shader* shader,int pass) {
	dataBuffer->use();
	if(indexed)
		glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0, objectToDraw);
	else
		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, objectToDraw);
}

void InstanceDrawcall::updateInstances(Instance* instance, int pass) {
	if (!isBillboard()) 
		dataBuffer->updateAttribData(4, objectToDraw, (void*)(instance->modelMatrices), GL_DYNAMIC_DRAW);
	else {
		dataBuffer->updateAttribData(2, objectToDraw, (void*)(instance->positions), GL_DYNAMIC_DRAW);
		dataBuffer->updateAttribData(3, objectToDraw, (void*)(instance->billboards), GL_DYNAMIC_DRAW);
	}
}

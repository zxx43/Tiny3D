#include "instanceDrawcall.h"

InstanceDrawcall::InstanceDrawcall(Instance* instance) :Drawcall() {
	vertexCount = instance->vertexCount;
	indexCount = instance->indexCount;
	indexed = indexCount > 0 ? true : false;
	objectCount = instance->instanceCount;
	this->instance = instance;

	dataBuffer = new RenderBuffer(indexed ? 6 : 5);
	dataBuffer->pushData(0, new RenderData(VERTEX_LOCATION, GL_FLOAT, vertexCount, 3, 1, 
		dataBuffer->vbos[0], false, GL_STATIC_DRAW, 0, instance->vertexBuffer));
	dataBuffer->pushData(1, new RenderData(NORMAL_LOCATION, GL_FLOAT, vertexCount, 3, 1,
		dataBuffer->vbos[1], false, GL_STATIC_DRAW, 0, instance->normalBuffer));
	dataBuffer->pushData(2, new RenderData(TEXCOORD_LOCATION, GL_FLOAT, vertexCount, instance->textureChannel, 1,
		dataBuffer->vbos[2], false, GL_STATIC_DRAW, 0, instance->texcoordBuffer));
	dataBuffer->pushData(3, new RenderData(COLOR_LOCATION, GL_UNSIGNED_BYTE, vertexCount, 3, 1, 
		dataBuffer->vbos[3], false, GL_STATIC_DRAW, 0, instance->colorBuffer));
	dataBuffer->pushData(4, new RenderData(MODEL_MATRIX_LOCATION, GL_FLOAT, objectCount, 4, 3,
		dataBuffer->vbos[4], false, GL_STATIC_DRAW, 1, instance->modelMatrices));
	if (indexed)
		dataBuffer->pushData(5, new RenderData(GL_UNSIGNED_SHORT, indexCount,
			dataBuffer->vbos[5], GL_STATIC_DRAW, instance->indexBuffer));

	createSimple();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setType(INSTANCE_DC);
}

InstanceDrawcall::~InstanceDrawcall() {
	releaseSimple();
	delete dataBuffer;
}

void InstanceDrawcall::createSimple() {
	simpleBuffer = new RenderBuffer(indexed ? 4 : 3);
	simpleBuffer->pushData(0, new RenderData(0, GL_FLOAT, vertexCount, 3, 1,
		simpleBuffer->vbos[0], false, GL_STATIC_DRAW, 0, instance->vertexBuffer));
	simpleBuffer->pushData(1, new RenderData(1, GL_FLOAT, vertexCount, instance->textureChannel, 1,
		simpleBuffer->vbos[1], false, GL_STATIC_DRAW, 0, instance->texcoordBuffer));
	simpleBuffer->pushData(2, new RenderData(2, GL_FLOAT, objectCount, 4, 3,
		simpleBuffer->vbos[2], false, GL_STATIC_DRAW, 1, instance->modelMatrices));
	if (indexed)
		simpleBuffer->pushData(3, new RenderData(GL_UNSIGNED_SHORT, indexCount,
			simpleBuffer->vbos[3], GL_STATIC_DRAW, instance->indexBuffer));
}

void InstanceDrawcall::releaseSimple() {
	delete simpleBuffer;
}

void InstanceDrawcall::draw(Shader* shader,bool simple) {
	if (!simple)
		dataBuffer->use();
	else
		simpleBuffer->use();
	if(!indexed)
		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, objectCount);
	else
		glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0, objectCount);
}

void InstanceDrawcall::updateMatrices(Instance* instance, bool updateNormals) {
	dataBuffer->streamDatas[4]->updateAttrBuf(objectCount, instance->modelMatrices, GL_DYNAMIC_DRAW);
	simpleBuffer->streamDatas[2]->updateAttrBuf(objectCount, instance->modelMatrices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

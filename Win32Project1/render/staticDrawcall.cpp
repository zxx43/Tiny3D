#include "staticDrawcall.h"
#include <string.h>
#include <stdlib.h>

StaticDrawcall::StaticDrawcall() {}

StaticDrawcall::StaticDrawcall(Batch* batch) :Drawcall() {
	vertexCount = batch->vertexCount;
	indexCount = batch->indexCount;
	objectCount = batch->objectCount;
	indexed = indexCount > 0 ? true : false;
	this->batch = batch;
	setFullStatic(batch->fullStatic);

	int bufCount = 4; 
	bufCount = !isFullStatic() ? bufCount + 1 : bufCount;
	bufCount = indexed ? bufCount + 1 : bufCount;
	dataBuffer = new RenderBuffer(bufCount);
	dataBuffer->pushData(0, new RenderData(VERTEX_LOCATION, GL_FLOAT, vertexCount, 3, 1,
		dataBuffer->vbos[0], false, GL_STATIC_DRAW, -1, batch->vertexBuffer));
	dataBuffer->pushData(1, new RenderData(NORMAL_LOCATION, GL_FLOAT, vertexCount, 3, 1,
		dataBuffer->vbos[1], false, GL_STATIC_DRAW, -1, batch->normalBuffer));
	dataBuffer->pushData(2, new RenderData(TEXCOORD_LOCATION, GL_FLOAT, vertexCount, batch->textureChannel, 1,
		dataBuffer->vbos[2], false, GL_STATIC_DRAW, -1, batch->texcoordBuffer));
	dataBuffer->pushData(3, new RenderData(COLOR_LOCATION, GL_UNSIGNED_BYTE, vertexCount, 3, 1,
		dataBuffer->vbos[3], false, GL_STATIC_DRAW, -1, batch->colorBuffer));

	int indexVBO = 5;
	if (!isFullStatic()) 
		dataBuffer->pushData(4, new RenderData(OBJECTID_LOCATION, GL_UNSIGNED_BYTE, vertexCount, 1, 1,
			dataBuffer->vbos[4], false, GL_STATIC_DRAW, -1, batch->objectidBuffer));
	else
		indexVBO = 4;

	if (indexed)
		dataBuffer->pushData(indexVBO, new RenderData(GL_UNSIGNED_INT, indexCount,
			dataBuffer->vbos[indexVBO], GL_STATIC_DRAW, batch->indexBuffer));

	createSimple();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uModelMatrix = (float*)malloc(MAX_OBJECT_COUNT * 12 * sizeof(float));
	uNormalMatrix = (float*)malloc(MAX_OBJECT_COUNT * 9 * sizeof(float));
	memcpy(uModelMatrix, batch->modelMatrices, batch->objectCount * 12 * sizeof(float));
	memcpy(uNormalMatrix, batch->normalMatrices, batch->objectCount * 9 * sizeof(float));
	
	setType(STATIC_DC);
}

StaticDrawcall::~StaticDrawcall() {
	releaseSimple();
	delete dataBuffer;
}

void StaticDrawcall::createSimple() {
	int bufCount = 2;
	bufCount = !isFullStatic() ? bufCount + 1 : bufCount;
	bufCount = indexed ? bufCount + 1 : bufCount;
	simpleBuffer = new RenderBuffer(bufCount);
	simpleBuffer->pushData(0, new RenderData(0, GL_FLOAT, vertexCount, 3, 1,
		simpleBuffer->vbos[0], false, GL_STATIC_DRAW, -1, batch->vertexBuffer));
	simpleBuffer->pushData(1, new RenderData(1, GL_FLOAT, vertexCount, batch->textureChannel, 1,
		simpleBuffer->vbos[1], false, GL_STATIC_DRAW, -1, batch->texcoordBuffer));

	int indexVBO = 3;
	if (!isFullStatic()) {
		simpleBuffer->pushData(2, new RenderData(2, GL_UNSIGNED_BYTE, vertexCount, 1, 1,
			simpleBuffer->vbos[2], false, GL_STATIC_DRAW, -1, batch->objectidBuffer));
	} else
		indexVBO = 2;

	if (indexed) {
		simpleBuffer->pushData(indexVBO, new RenderData(GL_UNSIGNED_INT, indexCount,
			simpleBuffer->vbos[indexVBO], GL_STATIC_DRAW, batch->indexBuffer));
	}
}

void StaticDrawcall::releaseSimple() {
	delete simpleBuffer;
}

void StaticDrawcall::draw(Shader* shader,int pass) {
	switch (pass) {
		case 1:
		case 2:
		case 3:
		case 5:
			simpleBuffer->use();
			break;
		case 4:
			dataBuffer->use();
			break;
	}
	if(!indexed)
		glDrawArrays(GL_TRIANGLES,0,vertexCount);
	else
		glDrawElements(GL_TRIANGLES,indexCount,GL_UNSIGNED_INT,0);
}

void StaticDrawcall::updateMatrices(Batch* batch) {
	memcpy(uModelMatrix, batch->modelMatrices, batch->objectCount * 12 * sizeof(float));
}

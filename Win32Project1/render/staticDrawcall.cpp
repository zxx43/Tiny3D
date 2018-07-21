#include "staticDrawcall.h"
#include "../batch/batch.h"
#include <string.h>
#include <stdlib.h>

StaticDrawcall::StaticDrawcall(Batch* batch) :Drawcall() {
	vertexCount = batch->vertexCount;
	indexCount = batch->indexCount;
	objectCount = batch->objectCount;
	indexed = indexCount > 0 ? true : false;
	this->batch = batch;
	setFullStatic(batch->fullStatic);

	int vertCount = batch->isDynamic() ? MAX_VERTEX_COUNT : vertexCount;
	int indCount = batch->isDynamic() ? MAX_INDEX_COUNT : indexCount;
	GLenum drawType = batch->isDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	dataBuffers = new RenderBuffer*[bufferCount];

	int bufCount = 1;
	bufCount = !isFullStatic() ? bufCount + 1 : bufCount;
	bufCount = indexed ? bufCount + 1 : bufCount;
	dataBuffers[0] = new RenderBuffer(bufCount);
	dataBuffers[0]->pushData(0, new RenderData(0, GL_FLOAT, vertCount, 3, 1,
		dataBuffers[0]->vbos[0], false, drawType, -1, batch->vertexBuffer));
	if (!isFullStatic())
		dataBuffers[0]->pushData(1, new RenderData(1, GL_UNSIGNED_BYTE, vertCount, 1, 1,
			dataBuffers[0]->vbos[1], false, drawType, -1, batch->objectidBuffer));
	if (indexed)
		dataBuffers[0]->pushData(bufCount - 1, new RenderData(GL_UNSIGNED_INT, indCount,
			dataBuffers[0]->vbos[bufCount - 1], drawType, batch->indexBuffer));
	dataBuffers[0]->unuse();

	bufCount = 2;
	bufCount = !isFullStatic() ? bufCount + 1 : bufCount;
	bufCount = indexed ? bufCount + 1 : bufCount;
	dataBuffers[1] = new RenderBuffer(bufCount);
	dataBuffers[1]->pushData(0, new RenderData(0, GL_FLOAT, vertCount, 3, 1,
		dataBuffers[1]->vbos[0], false, drawType, -1, batch->vertexBuffer));
	dataBuffers[1]->pushData(1, new RenderData(1, GL_FLOAT, vertCount, batch->textureChannel, 1,
		dataBuffers[1]->vbos[1], false, drawType, -1, batch->texcoordBuffer));
	if (!isFullStatic())
		dataBuffers[1]->pushData(2, new RenderData(2, GL_UNSIGNED_BYTE, vertCount, 1, 1,
			dataBuffers[1]->vbos[2], false, drawType, -1, batch->objectidBuffer));
	if (indexed)
		dataBuffers[1]->pushData(bufCount - 1, new RenderData(GL_UNSIGNED_INT, indCount,
			dataBuffers[1]->vbos[bufCount - 1], drawType, batch->indexBuffer));
	dataBuffers[1]->unuse();

	bufCount = 4;
	bufCount = !isFullStatic() ? bufCount + 1 : bufCount;
	bufCount = indexed ? bufCount + 1 : bufCount;
	dataBuffers[2] = new RenderBuffer(bufCount);
	dataBuffers[2]->pushData(0, new RenderData(VERTEX_LOCATION, GL_FLOAT, vertCount, 3, 1,
		dataBuffers[2]->vbos[0], false, drawType, -1, batch->vertexBuffer));
	dataBuffers[2]->pushData(1, new RenderData(NORMAL_LOCATION, GL_FLOAT, vertCount, 3, 1,
		dataBuffers[2]->vbos[1], false, drawType, -1, batch->normalBuffer));
	dataBuffers[2]->pushData(2, new RenderData(TEXCOORD_LOCATION, GL_FLOAT, vertCount, batch->textureChannel, 1,
		dataBuffers[2]->vbos[2], false, drawType, -1, batch->texcoordBuffer));
	dataBuffers[2]->pushData(3, new RenderData(COLOR_LOCATION, GL_UNSIGNED_BYTE, vertCount, 3, 1,
		dataBuffers[2]->vbos[3], false, drawType, -1, batch->colorBuffer));
	if (!isFullStatic())
		dataBuffers[2]->pushData(4, new RenderData(OBJECTID_LOCATION, GL_UNSIGNED_BYTE, vertCount, 1, 1,
			dataBuffers[2]->vbos[4], false, drawType, -1, batch->objectidBuffer));
	if (indexed)
		dataBuffers[2]->pushData(bufCount - 1, new RenderData(GL_UNSIGNED_INT, indCount,
			dataBuffers[2]->vbos[bufCount - 1], drawType, batch->indexBuffer));
	dataBuffers[2]->unuse();
	
	setType(STATIC_DC);
}

StaticDrawcall::~StaticDrawcall() {
	for (int i = 0; i < bufferCount; i++)
		delete dataBuffers[i];
	delete[] dataBuffers;
	uModelMatrix = NULL;
}

void StaticDrawcall::draw(Shader* shader,int pass) {
	if (!(batch->isDynamic() && indexed)) {
		int bufId = 0;
		switch (pass) {
			case 1:
			case 2:
			case 5:
				bufId = 1;
				break;
			case 3:
				bufId = 0;
				break;
			case 4:
				bufId = 2;
				break;
		}
		dataBuffers[bufId]->use();
	}

	if (!indexed)
		glDrawArrays(GL_TRIANGLES, 0, batch->vertexCount);
	else
		glDrawElements(GL_TRIANGLES, batch->indexCount, GL_UNSIGNED_INT, 0);
}

void StaticDrawcall::updateMatrices() {
	objectCount = batch->objectCount;
	if (batch->modelMatrices)
		uModelMatrix = batch->modelMatrices;
}

void StaticDrawcall::updateBuffers(int pass) {
	if (pass == 4) {
		dataBuffers[2]->streamDatas[VERTEX_LOCATION]->updateAttrBuf(batch->vertexCount, (void*)batch->vertexBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[2]->streamDatas[NORMAL_LOCATION]->updateAttrBuf(batch->vertexCount, (void*)batch->normalBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[2]->streamDatas[TEXCOORD_LOCATION]->updateAttrBuf(batch->vertexCount, (void*)batch->texcoordBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[2]->streamDatas[COLOR_LOCATION]->updateAttrBuf(batch->vertexCount, (void*)batch->colorBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[2]->streamDatas[OBJECTID_LOCATION]->updateAttrBuf(batch->vertexCount, (void*)batch->objectidBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[2]->unuseAttr();
		if (indexed) {
			dataBuffers[2]->use();
			dataBuffers[2]->streamDatas[5]->updateIndexBuf(batch->indexCount, (void*)batch->indexBuffer, GL_DYNAMIC_DRAW);
		}
	} else if (pass == 1 || pass == 2) {
		dataBuffers[1]->streamDatas[0]->updateAttrBuf(batch->vertexCount, (void*)batch->vertexBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[1]->streamDatas[1]->updateAttrBuf(batch->vertexCount, (void*)batch->texcoordBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[1]->streamDatas[2]->updateAttrBuf(batch->vertexCount, (void*)batch->objectidBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[1]->unuseAttr();
		if (indexed) {
			dataBuffers[1]->use();
			dataBuffers[1]->streamDatas[3]->updateIndexBuf(batch->indexCount, (void*)batch->indexBuffer, GL_DYNAMIC_DRAW);
		}
	} else if (pass == 3) {
		dataBuffers[0]->streamDatas[0]->updateAttrBuf(batch->vertexCount, (void*)batch->vertexBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[0]->streamDatas[1]->updateAttrBuf(batch->vertexCount, (void*)batch->objectidBuffer, GL_DYNAMIC_DRAW);
		dataBuffers[0]->unuseAttr();
		if (indexed) {
			dataBuffers[0]->use();
			dataBuffers[0]->streamDatas[2]->updateIndexBuf(batch->indexCount, (void*)batch->indexBuffer, GL_DYNAMIC_DRAW);
		}
	}
}

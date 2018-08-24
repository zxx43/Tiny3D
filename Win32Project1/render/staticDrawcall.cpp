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

	int texCnt = batch->textureChannel;
	int bufCount = 4;
	bufCount = !isFullStatic() ? bufCount + 1 : bufCount;
	bufCount = indexed ? bufCount + 1 : bufCount;
	dataBuffer = new RenderBuffer(bufCount);
	dataBuffer->setAttribData(VERTEX_LOCATION, GL_FLOAT, vertCount, 3, 1, false, drawType, -1, batch->vertexBuffer);
	dataBuffer->setAttribData(NORMAL_LOCATION, GL_FLOAT, vertCount, 3, 1, false, drawType, -1, batch->normalBuffer);
	dataBuffer->setAttribData(TEXCOORD_LOCATION, GL_FLOAT, vertCount, texCnt, 1, false, drawType, -1, batch->texcoordBuffer);
	dataBuffer->setAttribData(COLOR_LOCATION, GL_UNSIGNED_BYTE, vertCount, 3, 1, false, drawType, -1, batch->colorBuffer);
	if (!isFullStatic())
		dataBuffer->setAttribData(OBJECTID_LOCATION, GL_UNSIGNED_BYTE, vertCount, 1, 1, false, drawType, -1, batch->objectidBuffer);
	if (indexed)
		dataBuffer->setIndexData(bufCount - 1, GL_UNSIGNED_INT, indCount, drawType, batch->indexBuffer);
	dataBuffer->unuse();
	
	setType(STATIC_DC);
}

StaticDrawcall::~StaticDrawcall() {
	uModelMatrix = NULL;
}

void StaticDrawcall::draw(Shader* shader,int pass) {
	dataBuffer->use();
	if (indexed)
		glDrawElements(GL_TRIANGLES, batch->indexCount, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, batch->vertexCount);
}

void StaticDrawcall::updateMatrices() {
	objectCount = batch->objectCount;
	if (batch->modelMatrices)
		uModelMatrix = batch->modelMatrices;
}

void StaticDrawcall::updateBuffers(int pass) {
	if (pass == 4) {
		dataBuffer->updateAttribData(VERTEX_LOCATION, batch->vertexCount, (void*)batch->vertexBuffer, GL_DYNAMIC_DRAW);
		dataBuffer->updateAttribData(NORMAL_LOCATION, batch->vertexCount, (void*)batch->normalBuffer, GL_DYNAMIC_DRAW);
		dataBuffer->updateAttribData(TEXCOORD_LOCATION, batch->vertexCount, (void*)batch->texcoordBuffer, GL_DYNAMIC_DRAW);
		dataBuffer->updateAttribData(COLOR_LOCATION, batch->vertexCount, (void*)batch->colorBuffer, GL_DYNAMIC_DRAW);
		dataBuffer->updateAttribData(OBJECTID_LOCATION, batch->vertexCount, (void*)batch->objectidBuffer, GL_DYNAMIC_DRAW);
		if (indexed) {
			dataBuffer->use();
			dataBuffer->updateIndexData(5, batch->indexCount, (void*)batch->indexBuffer, GL_DYNAMIC_DRAW);
		}
	} else if (pass == 1 || pass == 2) {
		dataBuffer->updateAttribData(VERTEX_LOCATION, batch->vertexCount, (void*)batch->vertexBuffer, GL_DYNAMIC_DRAW);
		dataBuffer->updateAttribData(TEXCOORD_LOCATION, batch->vertexCount, (void*)batch->texcoordBuffer, GL_DYNAMIC_DRAW);
		dataBuffer->updateAttribData(OBJECTID_LOCATION, batch->vertexCount, (void*)batch->objectidBuffer, GL_DYNAMIC_DRAW);
		if (indexed) {
			dataBuffer->use();
			dataBuffer->updateIndexData(5, batch->indexCount, (void*)batch->indexBuffer, GL_DYNAMIC_DRAW);
		}
	} else if (pass == 3) {
		dataBuffer->updateAttribData(VERTEX_LOCATION, batch->vertexCount, (void*)batch->vertexBuffer, GL_DYNAMIC_DRAW);
		dataBuffer->updateAttribData(OBJECTID_LOCATION, batch->vertexCount, (void*)batch->objectidBuffer, GL_DYNAMIC_DRAW);
		if (indexed) {
			dataBuffer->use();
			dataBuffer->updateIndexData(5, batch->indexCount, (void*)batch->indexBuffer, GL_DYNAMIC_DRAW);
		}
	}
}

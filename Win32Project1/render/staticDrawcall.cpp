#include "staticDrawcall.h"
#include "../batch/batch.h"
#include "render.h"
#include <string.h>
#include <stdlib.h>

const uint VertexSlot = 0;
const uint NormalSlot = 1;
const uint TexcoordSlot = 2;
const uint TexidSlot = 3;
const uint ColorSlot = 4;
const uint TangentSlot = 5;
const uint ObjidSlot = 6;
const uint IndexSlot = 7;

const uint TerrainIndexSlot = 6;

StaticDrawcall::StaticDrawcall(Batch* batch) :Drawcall() {
	batchRef = batch;
	vertexCount = batchRef->vertexCount;
	indexCount = batchRef->indexCount;
	objectCount = batchRef->objectCount;
	indexed = indexCount > 0 ? true : false;
	setFullStatic(batchRef->fullStatic);

	dynDC = batchRef->isDynamic();
	vertCount = dynDC ? MAX_VERTEX_COUNT : vertexCount;
	indCount = dynDC ? MAX_INDEX_COUNT : indexCount;
	drawType = dynDC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	bufCount = 6;
	bufCount = !isFullStatic() ? bufCount + 1 : bufCount;
	bufCount = indexed ? bufCount + 1 : bufCount;

	doubleBuffer = false;
	dataBuffer = createBuffers(batchRef, bufCount, vertCount, indCount, drawType, NULL);
	if (dynDC && doubleBuffer)
		dataBuffer2 = createBuffers(batchRef, bufCount, vertCount, indCount, drawType, NULL);
	else
		dataBuffer2 = NULL;
	dataBufferVisual = NULL;

	bufferToPrepare = dataBuffer;
	if (dataBuffer2)
		bufferToDraw = dataBuffer2;
	else
		bufferToDraw = dataBuffer;

	setType(STATIC_DC);

	vertexCntToPrepare = batchRef->vertexCount;
	indexCntToPrepare = batchRef->indexCount;
	objectCntToPrepare = batchRef->objectCount;
	vertexCntToDraw = vertexCntToPrepare;
	indexCntToDraw = indexCntToPrepare;
	objectCntToDraw = objectCntToPrepare;

	if (!dynDC)
		modelMatricesToPrepare = NULL;
	else {
		modelMatricesToPrepare = (float*)malloc(MAX_OBJECT_COUNT * 12 * sizeof(float));
		memset(modelMatricesToPrepare, 0, MAX_OBJECT_COUNT * 12 * sizeof(float));
	}

	batchRef->releaseBatchData();
}

StaticDrawcall::~StaticDrawcall() {
	uModelMatrix = NULL;
	if (dataBuffer2) delete dataBuffer2;
	if (dataBufferVisual) delete dataBufferVisual;
	if (modelMatricesToPrepare) free(modelMatricesToPrepare);
}

RenderBuffer* StaticDrawcall::createBuffers(Batch* batch, int bufCount, int vertCount, int indCount, GLenum drawType, RenderBuffer* dupBuf) {
	RenderBuffer* buffer = new RenderBuffer(bufCount);
	if (!dupBuf) {
		buffer->setAttribData(VertexSlot, GL_FLOAT, vertCount, 3, 1, false, drawType, -1, batch->vertexBuffer);
		buffer->setAttribData(NormalSlot, GL_FLOAT, vertCount, 3, 1, false, drawType, -1, batch->normalBuffer);
		buffer->setAttribData(TexcoordSlot, GL_FLOAT, vertCount, 4, 1, false, drawType, -1, batch->texcoordBuffer);
		buffer->setAttribData(TexidSlot, GL_FLOAT, vertCount, 4, 1, false, drawType, -1, batch->texidBuffer);
		buffer->setAttribData(ColorSlot, GL_UNSIGNED_BYTE, vertCount, 3, 1, false, drawType, -1, batch->colorBuffer);
		buffer->setAttribData(TangentSlot, GL_FLOAT, vertCount, 3, 1, false, drawType, -1, batch->tangentBuffer);
		if (!isFullStatic())
			buffer->setAttribData(ObjidSlot, GL_UNSIGNED_BYTE, vertCount, 1, 1, false, drawType, -1, batch->objectidBuffer);
	} else {
		buffer->setAttribData(VertexSlot, dupBuf->streamDatas[VertexSlot]);
		buffer->setAttribData(NormalSlot, dupBuf->streamDatas[NormalSlot]);
		buffer->setAttribData(TexcoordSlot, dupBuf->streamDatas[TexcoordSlot]);
		buffer->setAttribData(TexidSlot, dupBuf->streamDatas[TexidSlot]);
		buffer->setAttribData(ColorSlot, dupBuf->streamDatas[ColorSlot]);
		buffer->setAttribData(TangentSlot, dupBuf->streamDatas[TangentSlot]);
		if (!isFullStatic())
			buffer->setAttribData(ObjidSlot, dupBuf->streamDatas[ObjidSlot]);
	}
	GLenum indType = !dupBuf ? drawType : GL_DYNAMIC_DRAW;
	if (indexed)
		buffer->setIndexData(bufCount - 1, GL_UNSIGNED_INT, indCount, indType, batch->indexBuffer);
	buffer->unuse();
	buffer->unuseAttr();
	buffer->unuseElement();
	return buffer;
}

void StaticDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	if (indexCntToDraw <= 0) return;
	if (frame < DELAY_FRAME) frame++;
	else {
		if (state->pass < DEFERRED_PASS && !isFullStatic() && objectCntToDraw > 0 && uModelMatrix)
			shader->setMatrix3x4("modelMatrices", objectCntToDraw, uModelMatrix);

		GLenum type = state->tess ? GL_PATCHES : GL_TRIANGLES;

		bufferToDraw->use();
		if(state->tess) 
			glPatchParameteri(GL_PATCH_VERTICES, 3);
		if (indexed)
			glDrawElements(type, indexCntToDraw, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(type, 0, vertexCntToDraw);
	}

	if (dynDC && doubleBuffer) {
		vertexCntToDraw = vertexCntToPrepare;
		indexCntToDraw = indexCntToPrepare;

		objectCntToDraw = objectCntToPrepare;
		flushMatricesToPrepare();
	}
}

void StaticDrawcall::flushMatricesToPrepare() {
	if (batchRef->matrixDataPtr)
		memcpy(modelMatricesToPrepare, batchRef->matrixDataPtr, objectCntToPrepare * 12 * sizeof(float));
}

void StaticDrawcall::updateMatrices() {
	if (!dynDC || !doubleBuffer) {
		objectCntToDraw = objectCntToPrepare;
		if (dynDC) {
			flushMatricesToPrepare();
			uModelMatrix = modelMatricesToPrepare;
		}
		else if (!dynDC && batchRef->modelMatrices)
			uModelMatrix = batchRef->modelMatrices;
	}
	else {
		uModelMatrix = modelMatricesToPrepare;
	}
}

void StaticDrawcall::updateBuffers(int pass, uint* indices, int indexCount) {
	if (!indices) {
		if (!dynDC || !doubleBuffer) {
			bufferToDraw = dataBuffer;
			indexCntToDraw = indexCntToPrepare;
		} else {
			bufferToDraw = bufferToDraw == dataBuffer ? dataBuffer2 : dataBuffer;
			bufferToPrepare = bufferToPrepare == dataBuffer ? dataBuffer2 : dataBuffer;
		} 
	} else {
		if(!dataBufferVisual) dataBufferVisual = createBuffers(batchRef, bufCount, vertCount, indCount, drawType, dataBuffer);
		bufferToDraw = dataBufferVisual;
		indexCntToDraw = indexCount;
		bufferToDraw->use();
		bufferToDraw->updateIndexData(TerrainIndexSlot, indexCntToDraw, (void*)indices);
	}

	if (!dynDC) return;

	if (!doubleBuffer) {
		vertexCntToDraw = vertexCntToPrepare;
		indexCntToDraw = indexCntToPrepare;
	}

	if (pass == COLOR_PASS) {
		bufferToPrepare->updateAttribData(VertexSlot, vertexCntToPrepare, (void*)batchRef->vertexBuffer);
		bufferToPrepare->updateAttribData(NormalSlot, vertexCntToPrepare, (void*)batchRef->normalBuffer);
		bufferToPrepare->updateAttribData(TexcoordSlot, vertexCntToPrepare, (void*)batchRef->texcoordBuffer);
		bufferToPrepare->updateAttribData(TexidSlot, vertexCntToPrepare, (void*)batchRef->texidBuffer);
		bufferToPrepare->updateAttribData(ColorSlot, vertexCntToPrepare, (void*)batchRef->colorBuffer);
		bufferToPrepare->updateAttribData(TangentSlot, vertexCntToPrepare, (void*)batchRef->tangentBuffer);
		bufferToPrepare->updateAttribData(ObjidSlot, vertexCntToPrepare, (void*)batchRef->objectidBuffer);
		if (indexed) {
			bufferToPrepare->use();
			bufferToPrepare->updateIndexData(IndexSlot, indexCntToPrepare, (void*)batchRef->indexBuffer);
		}
	}
	else if (pass == NEAR_SHADOW_PASS || pass == MID_SHADOW_PASS) {
		bufferToPrepare->updateAttribData(VertexSlot, vertexCntToPrepare, (void*)batchRef->vertexBuffer);
		bufferToPrepare->updateAttribData(TexcoordSlot, vertexCntToPrepare, (void*)batchRef->texcoordBuffer);
		bufferToPrepare->updateAttribData(ObjidSlot, vertexCntToPrepare, (void*)batchRef->objectidBuffer);
		if (indexed) {
			bufferToPrepare->use();
			bufferToPrepare->updateIndexData(IndexSlot, indexCntToPrepare, (void*)batchRef->indexBuffer);
		}
	}
	else if (pass == FAR_SHADOW_PASS) {
		bufferToPrepare->updateAttribData(VertexSlot, vertexCntToPrepare, (void*)batchRef->vertexBuffer);
		bufferToPrepare->updateAttribData(ObjidSlot, vertexCntToPrepare, (void*)batchRef->objectidBuffer);
		if (indexed) {
			bufferToPrepare->use();
			bufferToPrepare->updateIndexData(IndexSlot, indexCntToPrepare, (void*)batchRef->indexBuffer);
		}
	}
}

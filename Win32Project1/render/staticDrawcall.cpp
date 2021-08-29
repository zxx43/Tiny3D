#include "staticDrawcall.h"
#include "../batch/batch.h"
#include "render.h"
#include <string.h>
#include <stdlib.h>

// Attribute slots
const uint VertexSlot = 0;
const uint NormalSlot = 1;
const uint TexcoordSlot = 2;
const uint TexidSlot = 3;
const uint ColorSlot = 4;
const uint TangentSlot = 5;
const uint ObjidSlot = 6;

// VBO index
const uint VertexIndex = 0;
const uint NormalIndex = 1;
const uint TexcoordIndex = 2;
const uint TexidIndex = 3;
const uint ColorIndex = 4;
const uint TangentIndex = 5;
const uint ObjidIndex = 6;
const uint Index = 7;
const uint TerrainIndex = 6;

StaticDrawcall::StaticDrawcall(Batch* batch) :Drawcall() {
	batchRef = batch;
	vertexCount = batchRef->vertexCount;
	indexCount = batchRef->indexCount;
	objectCount = batchRef->objectCount;
	setFullStatic(batchRef->fullStatic);

	dynDC = batchRef->isDynamic();
	vertCount = dynDC ? MAX_VERTEX_COUNT : vertexCount;
	indCount = dynDC ? MAX_INDEX_COUNT : indexCount;
	drawType = dynDC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	bufCount = !isFullStatic() ? 8 : 7;
	dataBuffer = createBuffers(batchRef, bufCount, vertCount, indCount, drawType, NULL);
	dataBufferVisual = NULL;
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
	if (dataBufferVisual) delete dataBufferVisual;
	if (modelMatricesToPrepare) free(modelMatricesToPrepare);
}

RenderBuffer* StaticDrawcall::createBuffers(Batch* batch, int bufCount, int vertCount, int indCount, GLenum drawType, RenderBuffer* dupBuf) {
	RenderBuffer* buffer = new RenderBuffer(bufCount);
	if (!dupBuf) {
		buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex,VertexSlot, GL_FLOAT, vertCount, 3, 1, false, drawType, -1, batch->vertexBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, NormalIndex, NormalSlot, GL_FLOAT, vertCount, 3, 1, false, drawType, -1, batch->normalBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexcoordIndex, TexcoordSlot, GL_FLOAT, vertCount, 4, 1, false, drawType, -1, batch->texcoordBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexidIndex, TexidSlot, GL_FLOAT, vertCount, 4, 1, false, drawType, -1, batch->texidBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, ColorIndex, ColorSlot, GL_UNSIGNED_BYTE, vertCount, 4, 1, false, drawType, -1, batch->colorBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TangentIndex, TangentSlot, GL_FLOAT, vertCount, 3, 1, false, drawType, -1, batch->tangentBuffer);
		if (!isFullStatic())
			buffer->setAttribData(GL_ARRAY_BUFFER, ObjidIndex, ObjidSlot, GL_UNSIGNED_BYTE, vertCount, 1, 1, false, drawType, -1, batch->objectidBuffer);
	} else {
		buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex, dupBuf->streamDatas[VertexIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, NormalIndex, dupBuf->streamDatas[NormalIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexcoordIndex, dupBuf->streamDatas[TexcoordIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexidIndex, dupBuf->streamDatas[TexidIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, ColorIndex, dupBuf->streamDatas[ColorIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, TangentIndex, dupBuf->streamDatas[TangentIndex]);
		if (!isFullStatic())
			buffer->setAttribData(GL_ARRAY_BUFFER, ObjidIndex, dupBuf->streamDatas[ObjidIndex]);
	}
	GLenum indType = !dupBuf ? drawType : GL_DYNAMIC_DRAW;
	buffer->setBufferData(GL_ELEMENT_ARRAY_BUFFER, bufCount - 1, GL_UNSIGNED_INT, indCount, indType, batch->indexBuffer);
	buffer->unuse();
	return buffer;
}

void StaticDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	if (indexCntToDraw <= 0) return;
	if (frame < state->delay) frame++;
	else {
		render->useShader(shader);
		if (state->pass < DEFERRED_PASS && !isFullStatic() && objectCntToDraw > 0 && uModelMatrix)
			shader->setMatrix3x4("modelMatrices", objectCntToDraw, uModelMatrix);

		GLenum type = state->tess ? GL_PATCHES : GL_TRIANGLES;

		bufferToDraw->use();
		if(state->tess) 
			glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(type, indexCntToDraw, GL_UNSIGNED_INT, 0);
	}
}

void StaticDrawcall::flushMatricesToPrepare() {
	if (batchRef->matrixDataPtr)
		memcpy(modelMatricesToPrepare, batchRef->matrixDataPtr, objectCntToPrepare * 12 * sizeof(float));
}

void StaticDrawcall::updateMatrices() {
	objectCntToDraw = objectCntToPrepare;
	if (dynDC) {
		flushMatricesToPrepare();
		uModelMatrix = modelMatricesToPrepare;
	} else {
		if (batchRef->modelMatrices)
			uModelMatrix = batchRef->modelMatrices;
	}

}

void StaticDrawcall::updateBuffers(int pass, uint* indices, int indexCount) {
	if (!indices) {
		if (!dynDC) {
			bufferToDraw = dataBuffer;
			indexCntToDraw = indexCntToPrepare;
		}
	} else {
		if(!dataBufferVisual) dataBufferVisual = createBuffers(batchRef, bufCount, vertCount, indCount, drawType, dataBuffer);
		bufferToDraw = dataBufferVisual;
		indexCntToDraw = indexCount;
		bufferToDraw->use();
		bufferToDraw->updateBufferData(TerrainIndex, indexCntToDraw, (void*)indices);
	}

	if (!dynDC) return;

	vertexCntToDraw = vertexCntToPrepare;
	indexCntToDraw = indexCntToPrepare;

	if (pass == COLOR_PASS) {
		dataBuffer->updateBufferData(VertexIndex, vertexCntToPrepare, (void*)batchRef->vertexBuffer);
		dataBuffer->updateBufferData(NormalIndex, vertexCntToPrepare, (void*)batchRef->normalBuffer);
		dataBuffer->updateBufferData(TexcoordIndex, vertexCntToPrepare, (void*)batchRef->texcoordBuffer);
		dataBuffer->updateBufferData(TexidIndex, vertexCntToPrepare, (void*)batchRef->texidBuffer);
		dataBuffer->updateBufferData(ColorIndex, vertexCntToPrepare, (void*)batchRef->colorBuffer);
		dataBuffer->updateBufferData(TangentIndex, vertexCntToPrepare, (void*)batchRef->tangentBuffer);
		dataBuffer->updateBufferData(ObjidIndex, vertexCntToPrepare, (void*)batchRef->objectidBuffer);

		dataBuffer->use();
		dataBuffer->updateBufferData(Index, indexCntToPrepare, (void*)batchRef->indexBuffer);
	}
	else if (pass == NEAR_SHADOW_PASS || pass == MID_SHADOW_PASS) {
		dataBuffer->updateBufferData(VertexIndex, vertexCntToPrepare, (void*)batchRef->vertexBuffer);
		dataBuffer->updateBufferData(TexcoordIndex, vertexCntToPrepare, (void*)batchRef->texcoordBuffer);
		dataBuffer->updateBufferData(ObjidIndex, vertexCntToPrepare, (void*)batchRef->objectidBuffer);

		dataBuffer->use();
		dataBuffer->updateBufferData(Index, indexCntToPrepare, (void*)batchRef->indexBuffer);
	}
	else if (pass == FAR_SHADOW_PASS) {
		dataBuffer->updateBufferData(VertexIndex, vertexCntToPrepare, (void*)batchRef->vertexBuffer);
		dataBuffer->updateBufferData(ObjidIndex, vertexCntToPrepare, (void*)batchRef->objectidBuffer);

		dataBuffer->use();
		dataBuffer->updateBufferData(Index, indexCntToPrepare, (void*)batchRef->indexBuffer);
	}
}

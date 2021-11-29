#include "waterDrawcall.h"
#include "../render/render.h"

// Attribute slots
const uint VertexSlot = 0;

// VBO index
const uint VertexIndex = 0;
const uint OutIndex = 1;
const uint IndirectIndex = 2;

// SSBO index
const uint ChunkIndex = 0;
const uint InputIndex = 1;

WaterDrawcall::WaterDrawcall(Water* water, Batch* batch) {
	mesh = water;
	data = batch;
	vertexCount = data->vertexCount;
	maxIndexCount = data->indexCount;
	chunkCount = mesh->chunks.size();

	indirectBuffer = (Indirect*)malloc(sizeof(Indirect));
	indirectBuffer->count = 0;
	indirectBuffer->firstIndex = 0;
	indirectBuffer->primCount = 1;
	indirectBuffer->baseVertex = 0;
	indirectBuffer->baseInstance = 0;

	dataBuffer = createBuffers();
	ssBuffer = createSSBuffers();
	data->releaseBatchData();
}

RenderBuffer* WaterDrawcall::createBuffers() {
	RenderBuffer* buffer = new RenderBuffer(3);
	buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex, VertexSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, data->vertexBuffer);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, OutIndex, GL_UNSIGNED_INT, maxIndexCount, GL_STREAM_DRAW, NULL);
	buffer->useAs(OutIndex, GL_ELEMENT_ARRAY_BUFFER);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectIndex, GL_ONE, sizeof(Indirect), GL_STREAM_DRAW, indirectBuffer);
	buffer->useAs(IndirectIndex, GL_DRAW_INDIRECT_BUFFER);
	buffer->unuse();
	return buffer;
}

RenderBuffer* WaterDrawcall::createSSBuffers() {
	ChunkBuffer* chunkBuffer = (ChunkBuffer*)malloc(chunkCount * sizeof(ChunkBuffer));
	memset(chunkBuffer, 0, chunkCount * sizeof(ChunkBuffer));
	uint* indexBuffer = (uint*)malloc(chunkCount * WATER_CHUNK_INDEX_CNT * sizeof(uint));

	for (int i = 0; i < chunkCount; ++i) {
		Chunk* chunk = mesh->chunks[i];
		chunk->genBounding(data->vertexBuffer, WATER_CHUNK_INDEX_CNT);
		chunkBuffer[i].data[0] = chunk->boundCenter.x;
		chunkBuffer[i].data[1] = chunk->boundCenter.y;
		chunkBuffer[i].data[2] = chunk->boundCenter.z;
		chunkBuffer[i].data[4] = chunk->boundSize.x;
		chunkBuffer[i].data[5] = chunk->boundSize.y;
		chunkBuffer[i].data[6] = chunk->boundSize.z;
		for (uint j = 0; j < WATER_CHUNK_INDEX_CNT; ++j)
			indexBuffer[i * WATER_CHUNK_INDEX_CNT + j] = chunk->indices[j];
	}

	RenderBuffer* buffer = new RenderBuffer(2, false);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, ChunkIndex, GL_ONE, chunkCount * sizeof(ChunkBuffer), GL_STATIC_DRAW, chunkBuffer);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, InputIndex, GL_UNSIGNED_INT, chunkCount * WATER_CHUNK_INDEX_CNT, GL_STATIC_DRAW, indexBuffer);

	free(chunkBuffer);
	free(indexBuffer);
	return buffer;
}

WaterDrawcall::~WaterDrawcall() {
	free(indirectBuffer);
	delete ssBuffer;
}

void WaterDrawcall::update(Camera* camera, Render* render, RenderState* state) {
	indirectBuffer->count = 0; // Refresh index count
	dataBuffer->updateBufferMap(GL_DRAW_INDIRECT_BUFFER, IndirectIndex, sizeof(Indirect), indirectBuffer);

	ssBuffer->setShaderBase(ChunkIndex, 1);
	ssBuffer->setShaderBase(InputIndex, 2);
	dataBuffer->setShaderBase(IndirectIndex, 3);
	dataBuffer->setShaderBase(OutIndex, 4);

	render->useShader(state->shaderCompute);
	state->shaderCompute->setMatrix4("viewProjectMatrix", camera->viewProjectMatrix);
	state->shaderCompute->setVector3v("eyePos", camera->position);

	glDispatchCompute(chunkCount, 1, 1); // Update per chunk, check chunk cull
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	UnbindShaderBuffers(1, 4);

	/*
	dataBuffer->readBufferData(GL_DRAW_INDIRECT_BUFFER, IndirectIndex, sizeof(Indirect), indirectBuffer);
	printf("water index cnt: %d\n", indirectBuffer->count);
	//*/
}

void WaterDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	if (frame < state->delay) frame++;
	else {
		dataBuffer->use();
		render->useShader(shader);
		glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);
	}
}
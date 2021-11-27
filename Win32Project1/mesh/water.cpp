#include "water.h"
#include "../constants/constants.h"
#include <stdlib.h>
#include <string.h>

Water::Water(int size, float height) :Mesh() {
	waterSize = size;
	waterHeight = height;
	vertexCount = waterSize * waterSize;
	vertices = new vec4[vertexCount];
	normals = new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords = new vec2[vertexCount];
	indexCount = (waterSize - 1) * (waterSize - 1) * 6;
	indices = (int*)malloc(indexCount*sizeof(int));
	materialids = NULL;

	createChunks();
	initFaces();
	caculateExData();
}

Water::~Water() {
	for (uint i = 0; i < chunks.size(); ++i)
		delete chunks[i];
	chunks.clear();
}

void Water::createChunks() {
	chunks.clear();
	stepCount = waterSize - 1;
	chunkStep = stepCount / WATER_CHUNK_SIZE;

	for (int i = 0; i < chunkStep; ++i) {
		for (int j = 0; j < chunkStep; ++j) {
			Chunk* chunk = new Chunk();
			chunks.push_back(chunk);
		}
	}
}

void Water::initFaces() {
	int currentVertex = 0;

	float x, y, z, u, v;
	for (int i = 0, row = 0; row < stepCount + 1; i++, row++) {
		for (int j = 0, col = 0; col < stepCount + 1; j++, col++) {
			x = j; z = i; y = 0;
			if (j % 2 == 0) y = waterHeight;
			else y = -waterHeight;
			u = col / 4.0; v = row / 4.0;
			vertices[currentVertex] = vec4(x, y, z, 1);
			normals[currentVertex] = vec3(0, 1, 0);
			tangents[currentVertex] = vec3(1, 0, 0);
			texcoords[currentVertex] = vec2(u, v);
			currentVertex++;
		}
	}

	int currentIndex = 0, blockFirstIndex = 0;
	int sideVertexCount = stepCount + 1;
	uint* iArray = (uint*)malloc(6 * sizeof(uint));
	for (int i = 0; i < stepCount; i++) {
		for (int j = 0; j < stepCount; j++) {
			iArray[0] = blockFirstIndex;
			iArray[1] = blockFirstIndex + sideVertexCount;
			iArray[2] = blockFirstIndex + sideVertexCount + 1;
			iArray[3] = blockFirstIndex;
			iArray[4] = blockFirstIndex + sideVertexCount + 1;
			iArray[5] = blockFirstIndex + 1;

			indices[currentIndex++] = iArray[0];
			indices[currentIndex++] = iArray[1];
			indices[currentIndex++] = iArray[2];
			indices[currentIndex++] = iArray[3];
			indices[currentIndex++] = iArray[4];
			indices[currentIndex++] = iArray[5];

			if (j < stepCount - 1)
				blockFirstIndex++;
			else
				blockFirstIndex += 2;

			int chunki = floorf(i * 1.0f / WATER_CHUNK_SIZE), chunkj = floorf(j * 1.0f / WATER_CHUNK_SIZE);
			for (int c = 0; c < 6; ++c)
				chunks[chunki * chunkStep + chunkj]->indices.push_back(iArray[c]);
		}
	}
	free(iArray);
}
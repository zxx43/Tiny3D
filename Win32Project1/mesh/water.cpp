#include "water.h"
#include "../constants/constants.h"
#include <stdlib.h>
#include <string.h>

Water::Water(int size, float height) :Mesh() {
	waterSize = size;
	waterHeight = height;
	vertexCount = waterSize * waterSize;
	vertices = new VECTOR4D[vertexCount];
	normals = new VECTOR3D[vertexCount];
	texcoords = new VECTOR2D[vertexCount];
	indexCount = (waterSize - 1) * (waterSize - 1) * 6;
	indices = (int*)malloc(indexCount*sizeof(int));
	materialids = NULL;
	initFaces();
	caculateExData();
}

Water::~Water() {
}

void Water::initFaces() {
	int currentVertex = 0;
	int stepCount = waterSize - 1;

	float x, y, z, u, v;
	for (int i = 0, row = 0; row < stepCount + 1; i++, row++) {
		for (int j = 0, col = 0; col < stepCount + 1; j++, col++) {
			x = j; z = i; y = 0;
			u = col / 4.0; v = row / 4.0;
			vertices[currentVertex] = VECTOR4D(x, y, z, 1);
			normals[currentVertex] = VECTOR3D(0, 1, 0);
			texcoords[currentVertex] = VECTOR2D(u, v);
			currentVertex++;
		}
	}

	if (waterSize >= 2) {
		vertices[0].y = waterHeight;
		vertices[1].y = -waterHeight;
	}

	int currentIndex = 0, blockFirstIndex = 0;
	int sideVertexCount = stepCount + 1;
	for (int i = 0; i < stepCount; i++) {
		for (int j = 0; j < stepCount; j++) {
			indices[currentIndex] = blockFirstIndex; currentIndex++;
			indices[currentIndex] = blockFirstIndex + sideVertexCount; currentIndex++;
			indices[currentIndex] = blockFirstIndex + sideVertexCount + 1; currentIndex++;
			indices[currentIndex] = blockFirstIndex; currentIndex++;
			indices[currentIndex] = blockFirstIndex + sideVertexCount + 1; currentIndex++;
			indices[currentIndex] = blockFirstIndex + 1; currentIndex++;
			if (j < stepCount - 1)
				blockFirstIndex++;
			else
				blockFirstIndex += 2;
		}
	}
}
#include "board.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include <stdlib.h>
#include <string.h>

Board::Board() :Mesh() {
	vertexCount = 4;
	vertices = new vec4[vertexCount];
	normals = new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords = new vec2[vertexCount];
	materialids = NULL;

	indexCount = 6;
	indices = (int*)malloc(indexCount*sizeof(int));

	baseX = 1.0, baseY = 1.0, baseZ = 1.0;
	biasX = 0.0, biasY = 0.0;
	initFaces();
	caculateExData();
}

Board::Board(float sizex, float sizey, float sizez) :Mesh() {
	vertexCount = 4;
	vertices = new vec4[vertexCount];
	normals = new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords = new vec2[vertexCount];
	materialids = NULL;

	indexCount = 6;
	indices = (int*)malloc(indexCount*sizeof(int));

	baseX = sizex, baseY = sizey, baseZ = sizez;
	biasX = 0.0, biasY = 0.0;
	initFaces();
	caculateExData();
}

Board::Board(float sizex, float sizey, float sizez, float offx, float offy) :Mesh() {
	vertexCount = 4;
	vertices = new vec4[vertexCount];
	normals = new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords = new vec2[vertexCount];
	materialids = NULL;

	indexCount = 6;
	indices = (int*)malloc(indexCount*sizeof(int));

	baseX = sizex, baseY = sizey, baseZ = sizez;
	biasX = offx, biasY = offy;
	initFaces();
	caculateExData();
}

Board::Board(const Board& rhs) {
	vertexCount = rhs.vertexCount;
	vertices = new vec4[vertexCount];
	normals = new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords = new vec2[vertexCount];
	if (rhs.materialids)
		materialids = new int[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		vertices[i] = rhs.vertices[i];
		normals[i] = rhs.normals[i];
		tangents[i] = rhs.tangents[i];
		texcoords[i] = rhs.texcoords[i];
		if (rhs.materialids)
			materialids[i] = rhs.materialids[i];
	}

	indexCount = rhs.indexCount;
	indices = (int*)malloc(indexCount * sizeof(int));
	memcpy(indices, rhs.indices, indexCount * sizeof(int));
	caculateExData();
}

Board::~Board() {
}

void Board::initFaces() {
	vertices[0] = vec4(-0.5, -0.5, 0, 1);
	normals[0] = vec3(0, 0, 1);
	texcoords[0] = vec2(0, 0);

	vertices[1] = vec4(0.5, -0.5, 0, 1);
	normals[1] = vec3(0, 0, 1);
	texcoords[1] = vec2(1, 0);

	vertices[2] = vec4(0.5, 0.5, 0, 1);
	normals[2] = vec3(0, 0, 1);
	texcoords[2] = vec2(1, 1);

	vertices[3] = vec4(-0.5, 0.5, 0, 1);
	normals[3] = vec3(0, 0, 1);
	texcoords[3] = vec2(0, 1);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	vec3 tg0 = CaculateTangent(vertices[0], vertices[1], vertices[2], texcoords[0], texcoords[1], texcoords[2]);
	vec3 tg1 = CaculateTangent(vertices[0], vertices[2], vertices[3], texcoords[0], texcoords[2], texcoords[3]);

	tangents[0] = tg0;
	tangents[1] = tg0;
	tangents[2] = tg1;
	tangents[3] = tg1;

	for (int i = 0; i < 4; i++) {
		vertices[i].x += biasX;
		vertices[i].y += biasY;
		vertices[i].x *= baseX;
		vertices[i].y *= baseY;
		vertices[i].z *= baseZ;
	}
}


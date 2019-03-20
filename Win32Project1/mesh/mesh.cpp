#include "mesh.h"
#include "../constants/constants.h"
#include <stdlib.h>
#include <string.h>
#include "../bounding/aabb.h"

Mesh::Mesh() {
	vertexCount = 0;
	indexCount = 0;
	vertices = NULL;
	vertices3 = NULL;
	normals = NULL;
	normals4 = NULL;
	texcoords = NULL;
	materialids = NULL;
	indices = NULL;
	isBillboard = false;
	bounding = NULL;
}

Mesh::Mesh(const Mesh& rhs) {
	isBillboard = rhs.isBillboard;
}

Mesh::~Mesh() {
	if (vertices) delete[] vertices;
	if (vertices3) delete[] vertices3;
	if (normals) delete[] normals;
	if (normals4) delete[] normals4;
	if (texcoords) delete[] texcoords;
	if (materialids) delete[] materialids;
	if (indices) free(indices);
	if (bounding) free(bounding);
	vertices = NULL;
	vertices3 = NULL;
	normals = NULL;
	normals4 = NULL;
	texcoords = NULL;
	materialids = NULL;
	indices = NULL;
	bounding = NULL;
}

void Mesh::caculateExData() {
	if (!vertices3) vertices3 = new VECTOR3D[vertexCount];
	if (!normals4) normals4 = new VECTOR4D[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		vertices3[i].x = vertices[i].x / vertices[i].w;
		vertices3[i].y = vertices[i].y / vertices[i].w;
		vertices3[i].z = vertices[i].z / vertices[i].w;
		normals4[i] = VECTOR4D(normals[i].x, normals[i].y, normals[i].z, 0.0);
	}

	caculateBounding();
}

void Mesh::caculateBounding() {
	if (vertexCount <= 0) return;
	VECTOR3D first3 = vertices3[0];
	float sx = first3.x, sy = first3.y, sz = first3.z;
	float lx = sx, ly = sy, lz = sz;
	for (int i = 1; i < vertexCount; i++) {
		VECTOR3D local3 = vertices3[i];
		sx = sx > local3.x ? local3.x : sx;
		sy = sy > local3.y ? local3.y : sy;
		sz = sz > local3.z ? local3.z : sz;
		lx = lx < local3.x ? local3.x : lx;
		ly = ly < local3.y ? local3.y : ly;
		lz = lz < local3.z ? local3.z : lz;
	}
	VECTOR3D minVertex(sx, sy, sz), maxVertex(lx, ly, lz);
	AABB bbox(minVertex, maxVertex);

	if (bounding) free(bounding);
	bounding = (float*)malloc(6 * sizeof(float));
	bounding[0] = bbox.position.x;
	bounding[1] = bbox.position.y;
	bounding[2] = bbox.position.z;
	bounding[3] = bbox.sizex * 0.5;
	bounding[4] = bbox.sizey * 0.5;
	bounding[5] = bbox.sizez * 0.5;
}

void Mesh::setIsBillboard(bool billboard) {
	isBillboard = billboard;
}


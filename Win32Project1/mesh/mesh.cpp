#include "mesh.h"
#include "../constants/constants.h"
#include <stdlib.h>

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
	vertices = NULL;
	vertices3 = NULL;
	normals = NULL;
	normals4 = NULL;
	texcoords = NULL;
	materialids = NULL;
	indices = NULL;
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
}

void Mesh::setIsBillboard(bool billboard) {
	isBillboard = billboard;
}


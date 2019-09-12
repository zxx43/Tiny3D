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
	tangents = NULL;
	texcoords = NULL;
	materialids = NULL;
	indices = NULL;
	isBillboard = false;
	bounding = NULL;

	singleFaces.clear();
	normalFaces.clear();
}

Mesh::Mesh(const Mesh& rhs) {
	isBillboard = rhs.isBillboard;

	for (uint i = 0; i < rhs.singleFaces.size(); i++)
		singleFaces.push_back(rhs.singleFaces[i]->copy());
	for (uint i = 0; i < rhs.normalFaces.size(); i++)
		normalFaces.push_back(rhs.normalFaces[i]->copy());
}

Mesh::~Mesh() {
	if (vertices) delete[] vertices;
	if (vertices3) delete[] vertices3;
	if (normals) delete[] normals;
	if (normals4) delete[] normals4;
	if (tangents) delete[] tangents;
	if (texcoords) delete[] texcoords;
	if (materialids) delete[] materialids;
	if (indices) free(indices);
	if (bounding) free(bounding);
	vertices = NULL;
	vertices3 = NULL;
	normals = NULL;
	normals4 = NULL;
	tangents = NULL;
	texcoords = NULL;
	materialids = NULL;
	indices = NULL;
	bounding = NULL;

	for (uint i = 0; i < singleFaces.size(); i++)
		delete singleFaces[i];
	singleFaces.clear();
	for (uint i = 0; i < normalFaces.size(); i++)
		delete normalFaces[i];
	normalFaces.clear();
}

void Mesh::caculateExData() {
	if (!vertices3) vertices3 = new vec3[vertexCount];
	if (!normals4) normals4 = new vec4[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		vertices3[i].x = vertices[i].x / vertices[i].w;
		vertices3[i].y = vertices[i].y / vertices[i].w;
		vertices3[i].z = vertices[i].z / vertices[i].w;
		normals4[i] = vec4(normals[i].x, normals[i].y, normals[i].z, 0.0);
	}

	caculateBounding();
}

void Mesh::caculateBounding() {
	if (vertexCount <= 0) return;
	vec3 first3 = vertices3[0];
	float sx = first3.x, sy = first3.y, sz = first3.z;
	float lx = sx, ly = sy, lz = sz;
	for (int i = 1; i < vertexCount; i++) {
		vec3 local3 = vertices3[i];
		sx = sx > local3.x ? local3.x : sx;
		sy = sy > local3.y ? local3.y : sy;
		sz = sz > local3.z ? local3.z : sz;
		lx = lx < local3.x ? local3.x : lx;
		ly = ly < local3.y ? local3.y : ly;
		lz = lz < local3.z ? local3.z : lz;
	}
	vec3 minVertex(sx, sy, sz), maxVertex(lx, ly, lz);
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
	setSingle(true);
}

void Mesh::setSingle(bool single) {
	if (indexCount > 0) {
		if (single)
			singleFaces.push_back(new FaceBuf(0, indexCount));
		else
			normalFaces.push_back(new FaceBuf(0, indexCount));
	}
}


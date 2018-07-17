#include "sphere.h"
#include "../constants/constants.h"
#include <stdlib.h>
#include <string.h>

Sphere::Sphere(int m,int n):Mesh() {
	longitude=m;
	latitude=n;
	vertexCount = (m - 1) * n + 2;
	vertices=new VECTOR4D[vertexCount];
	normals=new VECTOR3D[vertexCount];
	texcoords=new VECTOR2D[vertexCount];
	indexCount = (m - 2) * n * 6 + 2 * n * 3;
	indices = (int*)malloc(indexCount*sizeof(int));
	materialids = NULL;
	initFaces();
	caculateExData();
}

Sphere::Sphere(const Sphere& rhs) {
	vertexCount=rhs.vertexCount;
	vertices=new VECTOR4D[vertexCount];
	normals=new VECTOR3D[vertexCount];
	texcoords=new VECTOR2D[vertexCount];
	if (rhs.materialids)
		materialids = new int[vertexCount];
	for(int i=0;i<vertexCount;i++) {
		vertices[i]=rhs.vertices[i];
		normals[i]=rhs.normals[i];
		texcoords[i]=rhs.texcoords[i];
		if (rhs.materialids)
			materialids[i] = rhs.materialids[i];
	}
	indexCount = rhs.indexCount;
	indices = (int*)malloc(indexCount*sizeof(int));
	memcpy(indices, rhs.indices, indexCount*sizeof(int));
	caculateExData();
}

Sphere::~Sphere() {
}

void Sphere::initFaces() {
	float stepAngZ=PI/longitude;
	float stepAngXY=PI2/latitude;
	uint current = 0;

	float x0 = 0; float y0 = 0; float z0 = 1;
	float v0 = 0; float u0 = 0;
	vertices[current].x = x0; vertices[current].y = y0; vertices[current].z = z0; vertices[current].w = 1;
	normals[current].x = x0; normals[current].y = y0; normals[current].z = z0;
	texcoords[current].x = u0; texcoords[current].y = v0;
	current++;

	for (int i = 1; i < longitude; i++) {
		for (int j = 0; j < latitude; j++) {
			float x = sin(i * stepAngZ) * cos(j * stepAngXY);
			float y = sin(i * stepAngZ) * sin(j * stepAngXY);
			float z = cos(i * stepAngZ);
			float v = (i * stepAngZ) / PI;
			float u = (j * stepAngXY) / PI2;

			vertices[current].x = x; vertices[current].y = y; vertices[current].z = z; vertices[current].w = 1;
			normals[current].x = x; normals[current].y = y; normals[current].z = z;
			texcoords[current].x = u; texcoords[current].y = v;
			current++;
		}
	}

	z0 = -1;
	v0 = 1; u0 = 0;
	vertices[current].x = x0; vertices[current].y = y0; vertices[current].z = z0; vertices[current].w = 1;
	normals[current].x = x0; normals[current].y = y0; normals[current].z = z0;
	texcoords[current].x = u0; texcoords[current].y = v0;

	uint curIndex = 0, baseVertex = 0;
	for (int i = 0; i < latitude; i++) {
		indices[curIndex] = 0;		curIndex++;
		indices[curIndex] = i + 1;	curIndex++;
		if (i != latitude - 1)
			indices[curIndex] = i + 2;
		else
			indices[curIndex] = 1;
		curIndex++;
	}
	baseVertex = 1;
	for (int i = 1; i < longitude - 1; i++) {
		for (int j = 0; j < latitude; j++) {
			indices[curIndex] = baseVertex + j;						curIndex++;
			indices[curIndex] = baseVertex + latitude + j;			curIndex++;
			if (j != latitude - 1)
				indices[curIndex] = baseVertex + latitude + (j + 1);
			else
				indices[curIndex] = baseVertex + latitude;
			curIndex++;

			if (j != latitude - 1) {
				indices[curIndex] = baseVertex + latitude + (j + 1); curIndex++;
				indices[curIndex] = baseVertex + (j + 1);			 curIndex++;
			} else {
				indices[curIndex] = baseVertex + latitude;			 curIndex++;
				indices[curIndex] = baseVertex;						 curIndex++;
			}
			indices[curIndex] = baseVertex + j;						 curIndex++;
		}
		baseVertex += latitude;
	} 
	for (int i = 0; i < latitude; i++) {
		indices[curIndex] = baseVertex + i;			curIndex++;
		indices[curIndex] = baseVertex + latitude;	curIndex++;
		if (i != latitude - 1)
			indices[curIndex] = baseVertex + i + 1;
		else
			indices[curIndex] = baseVertex;
		curIndex++;
	}
}

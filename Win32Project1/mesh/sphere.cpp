#include "sphere.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include <stdlib.h>
#include <string.h>

Sphere::Sphere(int m, int n) :Mesh() {
	longitude = m;
	latitude = n;
	vertexCount = (m - 1) * n + 2;
	vertices = new vec4[vertexCount];
	normals = new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords = new vec2[vertexCount];
	indexCount = (m - 2) * n * 6 + 2 * n * 3;
	indices = (int*)malloc(indexCount * sizeof(int));
	materialids = NULL;
	initFaces();
	caculateExData();
}

Sphere::Sphere(const Sphere& rhs) :Mesh(rhs) {
	vertexCount=rhs.vertexCount;
	vertices=new vec4[vertexCount];
	normals=new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords=new vec2[vertexCount];
	if (rhs.materialids)
		materialids = new int[vertexCount];
	for(int i=0;i<vertexCount;i++) {
		vertices[i]=rhs.vertices[i];
		normals[i]=rhs.normals[i];
		tangents[i] = rhs.tangents[i];
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

vec3 Sphere::caculateTangent(int j, int i) {
	float stepAngZ = PI / longitude;
	float stepAngXY = PI2 / latitude;

	int lon = i, lat = j;
	float x = sin(lon * stepAngZ) * cos(lat * stepAngXY);
	float y = sin(lon * stepAngZ) * sin(lat * stepAngXY);
	float z = cos(lon * stepAngZ);
	float v = (lon * stepAngZ) / PI;
	float u = (lat * stepAngXY) / PI2;
	vec3 p0(x, y, z); 
	vec2 t0(u, v);

	lon = i, lat = j + 1;
	x = sin(lon * stepAngZ) * cos(lat * stepAngXY);
	y = sin(lon * stepAngZ) * sin(lat * stepAngXY);
	z = cos(lon * stepAngZ);
	v = (lon * stepAngZ) / PI;
	u = (lat * stepAngXY) / PI2;
	vec3 p1(x, y, z);
	vec2 t1(u, v);

	lon = i - 1, lat = j;
	x = sin(lon * stepAngZ) * cos(lat * stepAngXY);
	y = sin(lon * stepAngZ) * sin(lat * stepAngXY);
	z = cos(lon * stepAngZ);
	v = (lon * stepAngZ) / PI;
	u = (lat * stepAngXY) / PI2;
	vec3 p2(x, y, z);
	vec2 t2(u, v);

	vec3 tangent = CaculateTangent(p0, p1, p2, t0, t1, t2);
	return tangent;
}

void Sphere::initFaces() {
	float stepAngZ = PI / longitude;
	float stepAngXY = PI2 / latitude;
	uint current = 0;

	float x0 = 0, y0 = 0, z0 = 1;
	float v0 = 0, u0 = 0;
	vertices[current].x = x0; vertices[current].y = y0; vertices[current].z = z0; vertices[current].w = 1;
	normals[current].x = x0; normals[current].y = y0; normals[current].z = z0;
	texcoords[current].x = u0; texcoords[current].y = v0;
	tangents[current].x = 1.0; tangents[current].y = 0.0; tangents[current].z = 0.0;
	current++;

	for (int i = 1; i < longitude; i++) {
		for (int j = 0; j < latitude; j++) {
			float x = sin(i * stepAngZ) * cos(j * stepAngXY);
			float y = sin(i * stepAngZ) * sin(j * stepAngXY);
			float z = cos(i * stepAngZ);
			float v = (i * stepAngZ) / PI;
			float u = (j * stepAngXY) / PI2;
			vec3 tangent = caculateTangent(j, i);

			vertices[current].x = x; vertices[current].y = y; vertices[current].z = z; vertices[current].w = 1;
			normals[current].x = x; normals[current].y = y; normals[current].z = z;
			texcoords[current].x = u; texcoords[current].y = v;
			tangents[current] = tangent;
			current++;
		}
	}

	z0 = -1;
	v0 = 1, u0 = 0;
	vertices[current].x = x0; vertices[current].y = y0; vertices[current].z = z0; vertices[current].w = 1;
	normals[current].x = x0; normals[current].y = y0; normals[current].z = z0;
	texcoords[current].x = u0; texcoords[current].y = v0;
	tangents[current].x = -1.0; tangents[current].y = 0.0; tangents[current].z = 0.0;

	uint curIndex = 0, baseVertex = 0;
	for (int i = 0; i < latitude; i++) {
		indices[curIndex++] = 0;
		indices[curIndex++] = i + 1;
		if (i != latitude - 1)
			indices[curIndex++] = i + 2;
		else
			indices[curIndex++] = 1;
	}
	baseVertex = 1;
	for (int i = 1; i < longitude - 1; i++) {
		for (int j = 0; j < latitude; j++) {
			indices[curIndex++] = baseVertex + j;
			indices[curIndex++] = baseVertex + latitude + j;
			if (j != latitude - 1)
				indices[curIndex++] = baseVertex + latitude + (j + 1);
			else
				indices[curIndex++] = baseVertex + latitude;

			if (j != latitude - 1) {
				indices[curIndex++] = baseVertex + latitude + (j + 1);
				indices[curIndex++] = baseVertex + (j + 1);
			} else {
				indices[curIndex++] = baseVertex + latitude;
				indices[curIndex++] = baseVertex;
			}
			indices[curIndex++] = baseVertex + j;
		}
		baseVertex += latitude;
	}
	for (int i = 0; i < latitude; i++) {
		indices[curIndex++] = baseVertex + i;
		indices[curIndex++] = baseVertex + latitude;
		if (i != latitude - 1)
			indices[curIndex++] = baseVertex + i + 1;
		else
			indices[curIndex++] = baseVertex;
	}
}

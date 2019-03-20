#include "quad.h"
#include "../constants/constants.h"
#include <stdlib.h>
#include <string.h>

Quad::Quad() :Mesh() {
	vertexCount = 4;
	vertices = new VECTOR4D[vertexCount];
	normals = new VECTOR3D[vertexCount];
	texcoords = new VECTOR2D[vertexCount];
	materialids = NULL;

	indexCount = 6;
	indices = (int*)malloc(indexCount*sizeof(int));

	baseX = 1.0, baseY = 1.0, baseZ = 1.0;
	initFaces();
	caculateExData();
}

Quad::Quad(float sizex, float sizey, float sizez) :Mesh() {
	vertexCount = 4;
	vertices = new VECTOR4D[vertexCount];
	normals = new VECTOR3D[vertexCount];
	texcoords = new VECTOR2D[vertexCount];
	materialids = NULL;

	indexCount = 6;
	indices = (int*)malloc(indexCount*sizeof(int));

	baseX = sizex, baseY = sizey, baseZ = sizez;
	initFaces();
	caculateExData();
}


Quad::Quad(const Quad& rhs) {
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

Quad::~Quad() {

}

void Quad::initFaces() {
	vertices[0]=VECTOR4D(-0.5,0,-0.5,1);
	normals[0]=VECTOR3D(0,1,0);
	texcoords[0]=VECTOR2D(0,1);

	vertices[1]=VECTOR4D(-0.5,0,0.5,1);
	normals[1]=VECTOR3D(0,1,0);
	texcoords[1]=VECTOR2D(0,0);

	vertices[2]=VECTOR4D(0.5,0,0.5,1);
	normals[2]=VECTOR3D(0,1,0);
	texcoords[2]=VECTOR2D(1,0);

	vertices[3]=VECTOR4D(0.5,0,-0.5,1);
	normals[3]=VECTOR3D(0,1,0);
	texcoords[3]=VECTOR2D(1,1);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	for (int i = 0; i < 4; i++) {
		vertices[i].x *= baseX;
		vertices[i].y *= baseY;
		vertices[i].z *= baseZ;
	}
}


#include "box.h"
#include "../constants/constants.h"

Box::Box():Mesh() {
	vertexCount=24;
	vertices=new VECTOR4D[vertexCount];
	normals=new VECTOR3D[vertexCount];
	texcoords=new VECTOR2D[vertexCount];
	indexCount = 36;
	indices = new int[indexCount];
	materialids = NULL;
	initFaces();
}

Box::Box(const Box& rhs) {
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
	indices = new int[indexCount];
	for (int i = 0; i < indexCount; i++)
		indices[i] = rhs.indices[i];
}

Box::~Box() {
}

void Box::initFaces() {
	vertices[0]=VECTOR4D(0.5,-0.5,-0.5,1);
	normals[0]=VECTOR3D(0,0,-1);
	texcoords[0]=VECTOR2D(1,1);

	vertices[1]=VECTOR4D(-0.5,-0.5,-0.5,1);
	normals[1]=VECTOR3D(0,0,-1);
	texcoords[1]=VECTOR2D(0,1);

	vertices[2]=VECTOR4D(-0.5,0.5,-0.5,1);
	normals[2]=VECTOR3D(0,0,-1);
	texcoords[2]=VECTOR2D(0,0);

	vertices[3]=VECTOR4D(0.5,0.5,-0.5,1);
	normals[3]=VECTOR3D(0,0,-1);
	texcoords[3]=VECTOR2D(1,0);

	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 2; indices[4] = 3; indices[5] = 0;


	vertices[4]=VECTOR4D(-0.5,0.5,0.5,1);
	normals[4]=VECTOR3D(0,0,1);
	texcoords[4]=VECTOR2D(0,0);

	vertices[5]=VECTOR4D(-0.5,-0.5,0.5,1);
	normals[5]=VECTOR3D(0,0,1);
	texcoords[5]=VECTOR2D(0,1);

	vertices[6]=VECTOR4D(0.5,-0.5,0.5,1);
	normals[6]=VECTOR3D(0,0,1);
	texcoords[6]=VECTOR2D(1,1);

	vertices[7]=VECTOR4D(0.5,0.5,0.5,1);
	normals[7]=VECTOR3D(0,0,1);
	texcoords[7]=VECTOR2D(1,0);

	indices[6] = 4; indices[7] = 5; indices[8] = 6;
	indices[9] = 6; indices[10] = 7; indices[11] = 4;


	vertices[8]=VECTOR4D(0.5,0.5,0.5,1);
	normals[8]=VECTOR3D(1,0,0);
	texcoords[8]=VECTOR2D(1,0);

	vertices[9]=VECTOR4D(0.5,-0.5,0.5,1);
	normals[9]=VECTOR3D(1,0,0);
	texcoords[9]=VECTOR2D(1,1);

	vertices[10]=VECTOR4D(0.5,-0.5,-0.5,1);
	normals[10]=VECTOR3D(1,0,0);
	texcoords[10]=VECTOR2D(0,1);

	vertices[11]=VECTOR4D(0.5,0.5,-0.5,1);
	normals[11]=VECTOR3D(1,0,0);
	texcoords[11]=VECTOR2D(0,0);

	indices[12] = 8; indices[13] = 9; indices[14] = 10;
	indices[15] = 10; indices[16] = 11; indices[17] = 8;


	vertices[12]=VECTOR4D(-0.5,-0.5,0.5,1);
	normals[12]=VECTOR3D(-1,0,0);
	texcoords[12]=VECTOR2D(0,1);

	vertices[13]=VECTOR4D(-0.5,0.5,0.5,1);
	normals[13]=VECTOR3D(-1,0,0);
	texcoords[13]=VECTOR2D(0,0);

	vertices[14]=VECTOR4D(-0.5,0.5,-0.5,1);
	normals[14]=VECTOR3D(-1,0,0);
	texcoords[14]=VECTOR2D(1,0);

	vertices[15]=VECTOR4D(-0.5,-0.5,-0.5,1);
	normals[15]=VECTOR3D(-1,0,0);
	texcoords[15]=VECTOR2D(1,1);

	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 14; indices[22] = 15; indices[23] = 12;


	vertices[16]=VECTOR4D(-0.5,0.5,-0.5,1);
	normals[16]=VECTOR3D(0,1,0);
	texcoords[16]=VECTOR2D(0,1);

	vertices[17]=VECTOR4D(-0.5,0.5,0.5,1);
	normals[17]=VECTOR3D(0,1,0);
	texcoords[17]=VECTOR2D(0,0);

	vertices[18]=VECTOR4D(0.5,0.5,0.5,1);
	normals[18]=VECTOR3D(0,1,0);
	texcoords[18]=VECTOR2D(1,0);

	vertices[19]=VECTOR4D(0.5,0.5,-0.5,1);
	normals[19]=VECTOR3D(0,1,0);
	texcoords[19]=VECTOR2D(1,1);

	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 18; indices[28] = 19; indices[29] = 16;


	vertices[20]=VECTOR4D(0.5,-0.5,0.5,1);
	normals[20]=VECTOR3D(0,-1,0);
	texcoords[20]=VECTOR2D(1,0);

	vertices[21]=VECTOR4D(-0.5,-0.5,0.5,1);
	normals[21]=VECTOR3D(0,-1,0);
	texcoords[21]=VECTOR2D(0,0);

	vertices[22]=VECTOR4D(-0.5,-0.5,-0.5,1);
	normals[22]=VECTOR3D(0,-1,0);
	texcoords[22]=VECTOR2D(0,1);

	vertices[23]=VECTOR4D(0.5,-0.5,-0.5,1);
	normals[23]=VECTOR3D(0,-1,0);
	texcoords[23]=VECTOR2D(1,1);

	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 22; indices[34] = 23; indices[35] = 20;
}


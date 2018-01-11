#include "quad.h"
#include "../constants/constants.h"

Quad::Quad():Mesh() {
	vertexCount=6;
	vertices=new VECTOR4D[vertexCount];
	normals=new VECTOR3D[vertexCount];
	texcoords=new VECTOR2D[vertexCount];
	materialids = NULL;
	initFaces();
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

	vertices[3]=VECTOR4D(0.5,0,0.5,1);
	normals[3]=VECTOR3D(0,1,0);
	texcoords[3]=VECTOR2D(1,0);

	vertices[4]=VECTOR4D(0.5,0,-0.5,1);
	normals[4]=VECTOR3D(0,1,0);
	texcoords[4]=VECTOR2D(1,1);

	vertices[5]=VECTOR4D(-0.5,0,-0.5,1);
	normals[5]=VECTOR3D(0,1,0);
	texcoords[5]=VECTOR2D(0,1);
}


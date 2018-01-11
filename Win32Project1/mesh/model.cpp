#include "model.h"
#include "../constants/constants.h"

Model::Model():Mesh() {
	vertexCount=0;
	indexCount=0;
	vertices=NULL;
	normals=NULL;
	texcoords=NULL;
	materialids = NULL;
	indices=NULL;
}

Model::Model(const char* obj, const char* mtl, int vt) :Mesh() {
	vertexCount = 0;
	indexCount = 0;
	vertices = NULL;
	normals = NULL;
	texcoords = NULL;
	materialids = NULL;
	indices = NULL;
	loadModelSimple(obj, mtl, vt);
}

Model::Model(const Model& rhs) {
	if(rhs.vertexCount>0) {
		vertexCount=rhs.vertexCount;
		vertices=new VECTOR4D[vertexCount];
		normals=new VECTOR3D[vertexCount];
		texcoords=new VECTOR2D[vertexCount];
		materialids = new int[vertexCount];
		for(int i=0;i<vertexCount;i++) {
			vertices[i]=rhs.vertices[i];
			normals[i]=rhs.normals[i];
			texcoords[i]=rhs.texcoords[i];
			materialids[i] = rhs.materialids[i];
		}
	}

	if(rhs.indexCount>0) {
		indexCount=rhs.indexCount;
		indices=new int[indexCount];
		for(int i=0;i<indexCount;i++)
			indices[i]=rhs.indices[i];
	}
}

Model::~Model() {

}

void Model::loadModel(const char* obj,const char* mtl,int vt) {
	loader=new ObjLoader(obj,mtl,vt);
	initFaces();
	delete loader;
	loader=NULL;
}

void Model::loadModelSimple(const char* obj,const char* mtl,int vt) {
	loader=new ObjLoader(obj,mtl,vt);
	initFacesWidthIndices();
	delete loader;
	loader=NULL;
}

void Model::initFaces() {
	vertexCount=loader->faceCount*3;
	vertices=new VECTOR4D[vertexCount];
	normals=new VECTOR3D[vertexCount];
	texcoords=new VECTOR2D[vertexCount];
	materialids = new int[vertexCount];

	for (int i=0;i<loader->faceCount;i++) {
		VECTOR3D p1(loader->vArr[loader->fvArr[i][0]-1][0],
				loader->vArr[loader->fvArr[i][0]-1][1],
				loader->vArr[loader->fvArr[i][0]-1][2]);
		VECTOR3D p2(loader->vArr[loader->fvArr[i][1]-1][0],
				loader->vArr[loader->fvArr[i][1]-1][1],
				loader->vArr[loader->fvArr[i][1]-1][2]);
		VECTOR3D p3(loader->vArr[loader->fvArr[i][2]-1][0],
				loader->vArr[loader->fvArr[i][2]-1][1],
				loader->vArr[loader->fvArr[i][2]-1][2]);

		VECTOR3D n1(loader->vnArr[loader->fnArr[i][0]-1][0],
				loader->vnArr[loader->fnArr[i][0]-1][1],
				loader->vnArr[loader->fnArr[i][0]-1][2]);
		VECTOR3D n2(loader->vnArr[loader->fnArr[i][1]-1][0],
				loader->vnArr[loader->fnArr[i][1]-1][1],
				loader->vnArr[loader->fnArr[i][1]-1][2]);
		VECTOR3D n3(loader->vnArr[loader->fnArr[i][2]-1][0],
				loader->vnArr[loader->fnArr[i][2]-1][1],
				loader->vnArr[loader->fnArr[i][2]-1][2]);

		VECTOR2D c1(loader->vtArr[loader->ftArr[i][0]-1][0],
				loader->vtArr[loader->ftArr[i][0]-1][1]);
		VECTOR2D c2(loader->vtArr[loader->ftArr[i][1]-1][0],
				loader->vtArr[loader->ftArr[i][1]-1][1]);
		VECTOR2D c3(loader->vtArr[loader->ftArr[i][2]-1][0],
				loader->vtArr[loader->ftArr[i][2]-1][1]);

		vertices[i*3].x=p1.x; vertices[i*3].y=p1.y; vertices[i*3].z=p1.z; vertices[i*3].w=1;
		vertices[i*3+1].x=p2.x; vertices[i*3+1].y=p2.y; vertices[i*3+1].z=p2.z; vertices[i*3+1].w=1;
		vertices[i*3+2].x=p3.x; vertices[i*3+2].y=p3.y; vertices[i*3+2].z=p3.z; vertices[i*3+2].w=1;

		normals[i*3].x=n1.x; normals[i*3].y=n1.y; normals[i*3].z=n1.z;
		normals[i*3+1].x=n2.x; normals[i*3+1].y=n2.y; normals[i*3+1].z=n2.z;
		normals[i*3+2].x=n3.x; normals[i*3+2].y=n3.y; normals[i*3+2].z=n3.z;

		texcoords[i*3].x=c1.x; texcoords[i*3].y=c1.y;
		texcoords[i*3+1].x=c2.x; texcoords[i*3+1].y=c2.y;
		texcoords[i*3+2].x=c3.x; texcoords[i*3+2].y=c3.y;

		int mid = loader->mtlLoader->objMtls[loader->mtArr[i]];
		materialids[i * 3] = mid;
		materialids[i * 3 + 1] = mid;
		materialids[i * 3 + 2] = mid;
	}
}

void Model::initFacesWidthIndices() {
	vertexCount=loader->vCount;
	vertices=new VECTOR4D[vertexCount];
	for(int i=0;i<vertexCount;i++) {
		vertices[i].x=loader->vArr[i][0];
		vertices[i].y=loader->vArr[i][1];
		vertices[i].z=loader->vArr[i][2];
		vertices[i].w=1.0;
	}

	normals=new VECTOR3D[vertexCount];
	texcoords=new VECTOR2D[vertexCount];
	materialids = new int[vertexCount];
	indexCount=loader->faceCount*3;
	indices=new int[indexCount];

	for (int i=0;i<loader->faceCount;i++) {
		int index1=loader->fvArr[i][0]-1;
		int index2=loader->fvArr[i][1]-1;
		int index3=loader->fvArr[i][2]-1;

		VECTOR3D n1(loader->vnArr[loader->fnArr[i][0]-1][0],
				loader->vnArr[loader->fnArr[i][0]-1][1],
				loader->vnArr[loader->fnArr[i][0]-1][2]);
		VECTOR3D n2(loader->vnArr[loader->fnArr[i][1]-1][0],
				loader->vnArr[loader->fnArr[i][1]-1][1],
				loader->vnArr[loader->fnArr[i][1]-1][2]);
		VECTOR3D n3(loader->vnArr[loader->fnArr[i][2]-1][0],
				loader->vnArr[loader->fnArr[i][2]-1][1],
				loader->vnArr[loader->fnArr[i][2]-1][2]);

		VECTOR2D c1(loader->vtArr[loader->ftArr[i][0]-1][0],
				loader->vtArr[loader->ftArr[i][0]-1][1]);
		VECTOR2D c2(loader->vtArr[loader->ftArr[i][1]-1][0],
				loader->vtArr[loader->ftArr[i][1]-1][1]);
		VECTOR2D c3(loader->vtArr[loader->ftArr[i][2]-1][0],
				loader->vtArr[loader->ftArr[i][2]-1][1]);

		normals[index1].x=n1.x; normals[index1].y=n1.y; normals[index1].z=n1.z;
		normals[index2].x=n2.x; normals[index2].y=n2.y; normals[index2].z=n2.z;
		normals[index3].x=n3.x; normals[index3].y=n3.y; normals[index3].z=n3.z;

		texcoords[index1].x=c1.x; texcoords[index1].y=c1.y;
		texcoords[index2].x=c2.x; texcoords[index2].y=c2.y;
		texcoords[index3].x=c3.x; texcoords[index3].y=c3.y;

		indices[i * 3] = index1;
		indices[i * 3 + 1] = index2;
		indices[i * 3 + 2] = index3;

		int mid = loader->mtlLoader->objMtls[loader->mtArr[i]];
		materialids[index1] = mid;
		materialids[index2] = mid;
		materialids[index3] = mid;
	}
}

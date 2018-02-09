#include "instance.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"

Instance::Instance(Mesh* mesh) {
	instanceMesh=mesh;
	vertexCount=0;
	indexCount=0;
	vertexBuffer=NULL;
	normalBuffer=NULL;
	texcoordBuffer=NULL;
	colorBuffer = NULL;
	indexBuffer=NULL;

	instanceCount=0;
	modelMatrices=NULL;
	normalMatrices=NULL;
	modelMatrixList.clear();
	normalMatrixList.clear();
}

Instance::~Instance() {
	if (vertexBuffer) delete[] vertexBuffer; vertexBuffer = NULL;
	if (normalBuffer) delete[] normalBuffer; normalBuffer = NULL;
	if (texcoordBuffer) delete[] texcoordBuffer; texcoordBuffer = NULL;
	if (colorBuffer) delete[] colorBuffer; colorBuffer = NULL;
	if (indexBuffer) delete[] indexBuffer; indexBuffer = NULL;
	if (modelMatrices) delete[] modelMatrices; modelMatrices = NULL;
	if (normalMatrices) delete[] normalMatrices; normalMatrices = NULL;
	
	modelMatrixList.clear();
	normalMatrixList.clear();
}

void Instance::initInstanceBuffers(int mid,int vertices,int indices) {
	vertexCount=vertices;
	vertexBuffer=new float[vertexCount*3];
	normalBuffer=new float[vertexCount*3];
	texcoordBuffer=new float[vertexCount*4];
	colorBuffer = new byte[vertexCount * 3];

	indexCount=indices;
	if(indexCount>0)
		indexBuffer=new ushort[indexCount];

	for(int i=0;i<vertexCount;i++) {
		VECTOR4D vertex=instanceMesh->vertices[i];
		VECTOR3D normal=instanceMesh->normals[i];
		VECTOR2D texcoord=instanceMesh->texcoords[i];

		Material* mat = NULL;
		if (!instanceMesh->materialids && mid >= 0)
			mat = materials->find(mid);
		else if (instanceMesh->materialids)
			mat = materials->find(instanceMesh->materialids[i]);
		if (!mat) mat = materials->find(0);
		VECTOR4D textures = mat->texture;
		VECTOR3D ambient = mat->ambient;
		VECTOR3D diffuse = mat->diffuse;
		VECTOR3D specular = mat->specular;

		vertexBuffer[i*3]=vertex.x;
		vertexBuffer[i*3+1]=vertex.y;
		vertexBuffer[i*3+2]=vertex.z;

		normalBuffer[i*3]=normal.x;
		normalBuffer[i*3+1]=normal.y;
		normalBuffer[i*3+2]=normal.z;

		textureChannel = textures.y >= 0 ? 4 : 3;
		texcoordBuffer[i * textureChannel] = texcoord.x;
		texcoordBuffer[i * textureChannel + 1] = texcoord.y;
		texcoordBuffer[i * textureChannel + 2] = textures.x;
		if (textureChannel == 4)
			texcoordBuffer[i * textureChannel + 3] = textures.y;

		colorBuffer[i * 3] = (byte)(ambient.x * 255);
		colorBuffer[i * 3 + 1] = (byte)(diffuse.x * 255);
		colorBuffer[i * 3 + 2] = (byte)(specular.x * 255);
	}

	if(instanceMesh->indices) {
		for(int i=0;i<indexCount;i++) {
			int index=instanceMesh->indices[i];
			indexBuffer[i]=(ushort)index;
		}
	}
}

void Instance::pushObjectToInstances(const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix) {
	instanceCount++;
	modelMatrixList.push_back(transformMatrix);
	normalMatrixList.push_back(normalMatrix);
}

void Instance::updateMatricesBuffer(int baseInstance, const MATRIX4X4& transformMatrix, const MATRIX4X4* normalMatrix) {
	MATRIX4X4 transform = transformMatrix;
	transform.Transpose();
	for (int m = 0; m < 12; m++)
		modelMatrices[baseInstance * 12 + m] = transform.entries[m];
	if (normalMatrix) {
		for (int n = 0; n < 3; n++)
			normalMatrices[baseInstance * 9 + n] = normalMatrix->entries[n];
		for (int n = 3; n < 6; n++)
			normalMatrices[baseInstance * 9 + n] = normalMatrix->entries[n + 1];
		for (int n = 6; n < 9; n++)
			normalMatrices[baseInstance * 9 + n] = normalMatrix->entries[n + 2];
	}
}

void Instance::initMatrices() {
	modelMatrices=new float[instanceCount*12];
	normalMatrices=new float[instanceCount*9];
	for(int i=0;i<instanceCount;i++) {
		MATRIX4X4 transform = modelMatrixList[i];
		transform.Transpose();
		for (int m = 0; m < 12; m++) 
			modelMatrices[i * 12 + m] = transform.entries[m];
		for (int n = 0; n < 3; n++)
			normalMatrices[i * 9 + n] = normalMatrixList[i].entries[n];
		for (int n = 3; n < 6; n++)
			normalMatrices[i * 9 + n] = normalMatrixList[i].entries[n + 1];
		for (int n = 6; n < 9; n++)
			normalMatrices[i * 9 + n] = normalMatrixList[i].entries[n + 2];
	}
}

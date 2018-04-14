#include "instance.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"
#include <stdlib.h>
#include <string.h>

std::map<Mesh*, int> Instance::instanceTable;

Instance::Instance(Mesh* mesh) {
	instanceMesh=mesh;
	vertexCount=0;
	indexCount=0;
	vertexBuffer=NULL;
	normalBuffer=NULL;
	texcoordBuffer=NULL;
	colorBuffer = NULL;
	indexBuffer=NULL;

	instanceCount = 0;
	drawcall = NULL;
	singleSide = false;
}

Instance::~Instance() {
	if (vertexBuffer) delete[] vertexBuffer; vertexBuffer = NULL;
	if (normalBuffer) delete[] normalBuffer; normalBuffer = NULL;
	if (texcoordBuffer) delete[] texcoordBuffer; texcoordBuffer = NULL;
	if (colorBuffer) delete[] colorBuffer; colorBuffer = NULL;
	if (indexBuffer) delete[] indexBuffer; indexBuffer = NULL;

	if (modelMatrices) free(modelMatrices); modelMatrices = NULL;
	if (drawcall) delete drawcall;
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
			mat = MaterialManager::materials->find(mid);
		else if (instanceMesh->materialids)
			mat = MaterialManager::materials->find(instanceMesh->materialids[i]);
		if (!mat) mat = MaterialManager::materials->find(0);
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

	initMatrices();
}

void Instance::setInstanceCount(int count) {
	instanceCount = count;
	if (drawcall) drawcall->objectToDraw = instanceCount;
}

void Instance::updateMatricesBuffer(int i, const MATRIX4X4& transformMatrix) {
	memcpy(modelMatrices + (i * 12), transformMatrix.GetTranspose().entries, 12 * sizeof(float));
}

void Instance::initMatrices() {
	modelMatrices = (float*)malloc(MAX_INSTANCE_COUNT * 12 * sizeof(float));
	memset(modelMatrices, 0, MAX_INSTANCE_COUNT * 12 * sizeof(float));
}

void Instance::createDrawcall(bool simple) {
	drawcall = new InstanceDrawcall(this, simple);
	drawcall->setSide(singleSide);
}

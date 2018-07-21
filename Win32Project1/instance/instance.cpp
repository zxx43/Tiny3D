#include "instance.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"

std::map<Mesh*, int> Instance::instanceTable;

Instance::Instance(Mesh* mesh) {
	instanceMesh = mesh;
	vertexCount = 0;
	indexCount = 0;
	vertexBuffer = NULL;
	normalBuffer = NULL;
	texcoordBuffer = NULL;
	colorBuffer = NULL;
	indexBuffer = NULL;

	instanceCount = 0;
	drawcall = NULL;
	singleSide = false;
	isBillboard = instanceMesh->isBillboard;

	modelMatrices = NULL;
	positions = NULL;
	billboards = NULL;
}

Instance::~Instance() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	if (modelMatrices) free(modelMatrices); modelMatrices = NULL;
	if (positions) free(positions); positions = NULL;
	if (billboards) free(billboards); billboards = NULL;
	if (drawcall) delete drawcall;
}

void Instance::initInstanceBuffers(Object* object,int vertices,int indices) {
	vertexCount = vertices;
	vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	normalBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));

	indexCount=indices;
	if (indexCount > 0)
		indexBuffer = (ushort*)malloc(indexCount*sizeof(ushort));

	int mid = object->material;
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
		if (!isBillboard) {
			texcoordBuffer[i * textureChannel + 2] = textures.x;
			if (textureChannel == 4)
				texcoordBuffer[i * textureChannel + 3] = textures.y;
		} else
			texcoordBuffer[i * textureChannel + 2] = (float)object->billboard->texid;

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

	if (isBillboard)
		initBillboards();
	else
		initMatrices();
}


void Instance::initMatrices() {
	modelMatrices = (float*)malloc(MAX_INSTANCE_COUNT * 12 * sizeof(float));
	memset(modelMatrices, 0, MAX_INSTANCE_COUNT * 12 * sizeof(float));
}

void Instance::initBillboards() {
	positions = (float*)malloc(MAX_INSTANCE_COUNT * 3 * sizeof(float));
	memset(positions, 0, MAX_INSTANCE_COUNT * 3 * sizeof(float));
	billboards = (float*)malloc(MAX_INSTANCE_COUNT * 2 * sizeof(float));
	memset(billboards, 0, MAX_INSTANCE_COUNT * 2 * sizeof(float));
}

void Instance::setRenderData(int count, float* matrices, float* billboards, float* positions) {
	instanceCount = count;
	if (drawcall) drawcall->objectToDraw = instanceCount;

	if (matrices)
		memcpy(modelMatrices, matrices, instanceCount * 12 * sizeof(float));
	else {
		memcpy(this->billboards, billboards, instanceCount * 2 * sizeof(float));
		memcpy(this->positions, positions, instanceCount * 3 * sizeof(float));
	}
}

void Instance::createDrawcall() {
	drawcall = new InstanceDrawcall(this);
	drawcall->setSide(singleSide);
}

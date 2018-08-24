#include "batch.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"
#include <string.h>
#include <stdlib.h>

Batch::Batch() {
	vertexCount=0;
	indexCount=0;
	vertexBuffer=NULL;
	normalBuffer=NULL;
	texcoordBuffer=NULL;
	colorBuffer = NULL;
	objectidBuffer = NULL;
	indexBuffer=NULL;

	fullStatic = false;
	textureChannel = 3;
	type = BATCH_TYPE_DYNAMIC;
	objectCount = 0;
	modelMatrices = NULL;
	normalMatrices = NULL;
	drawcall = NULL;
}

Batch::~Batch() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (objectidBuffer) free(objectidBuffer); objectidBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	if (modelMatrices) free(modelMatrices); modelMatrices = NULL;
	if (normalMatrices) free(normalMatrices); normalMatrices = NULL;

	if (drawcall) delete drawcall; drawcall = NULL;
}

void Batch::flushBatchBuffers() {
	vertexCount = 0;
	indexCount = 0;
	objectCount = 0;
}

void Batch::initBatchBuffers(int vertCount, int indCount) {
	vertexCount = vertCount, indexCount = indCount;
	if (!vertexBuffer) vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	if (!normalBuffer) normalBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	if (!texcoordBuffer) texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	if (!colorBuffer) colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));
	if (!objectidBuffer) objectidBuffer = (byte*)malloc(vertexCount * sizeof(byte));
	if (!indexBuffer) indexBuffer = (uint*)malloc(indexCount * sizeof(uint));

	if (!modelMatrices) modelMatrices = (float*)malloc(MAX_OBJECT_COUNT * 12 * sizeof(float));
	if (!normalMatrices) normalMatrices = (float*)malloc(MAX_OBJECT_COUNT * 9 * sizeof(float));

	flushBatchBuffers();
}

void Batch::pushMeshToBuffers(Mesh* mesh,int mid,bool fullStatic,const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix) {
	this->fullStatic = fullStatic;
	int baseVertex = vertexCount;
	int currentObject = objectCount++;

	Material* mat = NULL;
	if (mid >= 0)
		mat = MaterialManager::materials->find(mid);
	if (!mat) mat = MaterialManager::materials->find(0);

	for (int i = 0; i < mesh->vertexCount; i++) {
		VECTOR4D normal = mesh->normals4[i];
		VECTOR2D texcoord = mesh->texcoords[i];

		if (mesh->materialids)
			mat = MaterialManager::materials->find(mesh->materialids[i]);

		if (!fullStatic) {
			VECTOR3D vertex3 = mesh->vertices3[i];
			vertexBuffer[vertexCount * 3] = vertex3.x;
			vertexBuffer[vertexCount * 3 + 1] = vertex3.y;
			vertexBuffer[vertexCount * 3 + 2] = vertex3.z;

			normalBuffer[vertexCount * 3] = normal.x;
			normalBuffer[vertexCount * 3 + 1] = normal.y;
			normalBuffer[vertexCount * 3 + 2] = normal.z;
		} else {
			VECTOR4D vertex = transformMatrix * mesh->vertices[i];
			float invW = 1.0 / vertex.w;
			vertexBuffer[vertexCount * 3] = vertex.x * invW;
			vertexBuffer[vertexCount * 3 + 1] = vertex.y * invW;
			vertexBuffer[vertexCount * 3 + 2] = vertex.z * invW;

			normal = normalMatrix * normal;
			normalBuffer[vertexCount * 3] = normal.x;
			normalBuffer[vertexCount * 3 + 1] = normal.y;
			normalBuffer[vertexCount * 3 + 2] = normal.z;
		}

		textureChannel = mat->texture.y >= 0 ? 4 : 3;
		texcoordBuffer[vertexCount * textureChannel] = texcoord.x;
		texcoordBuffer[vertexCount * textureChannel + 1] = texcoord.y;
		texcoordBuffer[vertexCount * textureChannel + 2] = mat->texture.x;
		if (textureChannel == 4)
			texcoordBuffer[vertexCount * textureChannel + 3] = mat->texture.y;

		colorBuffer[vertexCount * 3] = (byte)(mat->ambient.x * 255);
		colorBuffer[vertexCount * 3 + 1] = (byte)(mat->diffuse.x * 255);
		colorBuffer[vertexCount * 3 + 2] = (byte)(mat->specular.x * 255);

		objectidBuffer[vertexCount++] = currentObject;
	}

	for (int i = 0; i < mesh->indexCount; i++) 
		indexBuffer[indexCount++] = (uint)(baseVertex + mesh->indices[i]);

	if (!fullStatic && type == BATCH_TYPE_STATIC)
		initMatrix(currentObject, transformMatrix, normalMatrix);
}

void Batch::updateMatrices(unsigned short objectId, const MATRIX4X4& transformMatrix, const MATRIX4X4* normalMatrix) {
	memcpy(modelMatrices + (objectId * 12), transformMatrix.GetTranspose().entries, 12 * sizeof(float));
	if (normalMatrix) {
		memcpy(normalMatrices + (objectId * 9), normalMatrix->entries, 3 * sizeof(float));
		memcpy(normalMatrices + (objectId * 9 + 3), normalMatrix->entries + 4, 3 * sizeof(float));
		memcpy(normalMatrices + (objectId * 9 + 6), normalMatrix->entries + 8, 3 * sizeof(float));
	}
}

void Batch::initMatrix(unsigned short currentObject, const MATRIX4X4& transformMatrix, const MATRIX4X4& normalMatrix) {
	memcpy(modelMatrices + (currentObject * 12), transformMatrix.GetTranspose().entries, 12 * sizeof(float));
	memcpy(normalMatrices + (currentObject * 9), normalMatrix.entries, 3 * sizeof(float));
	memcpy(normalMatrices + (currentObject * 9 + 3), normalMatrix.entries + 4, 3 * sizeof(float));
	memcpy(normalMatrices + (currentObject * 9 + 6), normalMatrix.entries + 8, 3 * sizeof(float));
}

void Batch::setRenderData(int pass, int vertCnt, int indCnt, int objCnt,
	float* vertices, float* normals, float* texcoords,
	byte* colors, byte* objectids, uint* indices, float* matrices) {
		vertexCount = vertCnt;
		indexCount = indCnt;
		objectCount = objCnt;

		memcpy(vertexBuffer, vertices, vertexCount * 3 * sizeof(float));
		if (pass == 1 || pass == 2 || pass == 4) {
			memcpy(texcoordBuffer, texcoords, vertexCount * 3 * sizeof(float));
			if (pass == 4) {
				memcpy(normalBuffer, normals, vertexCount * 3 * sizeof(float));
				memcpy(colorBuffer, colors, vertexCount * 3 * sizeof(byte));
			}
		}
		memcpy(objectidBuffer, objectids, vertexCount * sizeof(byte));
		memcpy(indexBuffer, indices, indexCount * sizeof(uint));
		memcpy(modelMatrices, matrices, objectCount * 12 * sizeof(float));
}

void Batch::createDrawcall() {
	drawcall = new StaticDrawcall(this);
}

bool Batch::isDynamic() {
	return type == BATCH_TYPE_DYNAMIC;
}

void Batch::setDynamic(bool dynamic) {
	type = dynamic ? BATCH_TYPE_DYNAMIC : BATCH_TYPE_STATIC;
}

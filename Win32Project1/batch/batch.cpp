#include "batch.h"
#include "../material/materialManager.h"
#include <string.h>
#include <stdlib.h>

Batch::Batch() {
	vertexCount=0;
	indexCount=0;
	vertexBuffer=NULL;
	normalBuffer=NULL;
	texcoordBuffer=NULL;
	texOfsBuffer = NULL;
	colorBuffer = NULL;
	objectidBuffer = NULL;
	indexBuffer=NULL;

	fullStatic = false;
	textureCount = 1;
	type = BATCH_TYPE_DYNAMIC;
	objectCount = 0;
	modelMatrices = NULL;
	matrixDataPtr = NULL;
	normalMatrices = NULL;
	drawcall = NULL;
}

Batch::~Batch() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (texOfsBuffer) free(texOfsBuffer); texOfsBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (objectidBuffer) free(objectidBuffer); objectidBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	matrixDataPtr = NULL;
	if (modelMatrices) free(modelMatrices); modelMatrices = NULL;
	if (normalMatrices) free(normalMatrices); normalMatrices = NULL;

	if (drawcall) delete drawcall; drawcall = NULL;
}

void Batch::releaseBatchData() {
	if (fullStatic || !isDynamic()) {
		if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
		if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
		if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
		if (texOfsBuffer) free(texOfsBuffer); texOfsBuffer = NULL;
		if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
		if (objectidBuffer) free(objectidBuffer); objectidBuffer = NULL;
		if (indexBuffer) free(indexBuffer); indexBuffer = NULL;
	}
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
	if (!texOfsBuffer) texOfsBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
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
			for (int v = 0; v < 3; v++) {
				vertexBuffer[vertexCount * 3 + v] = GetVec3(&vertex3, v);
				normalBuffer[vertexCount * 3 + v] = GetVec4(&normal, v);
			}
		} else {
			VECTOR4D vertex = transformMatrix * mesh->vertices[i];
			float invW = 1.0 / vertex.w;
			normal = normalMatrix * normal;
			for (int v = 0; v < 3; v++) {
				vertexBuffer[vertexCount * 3 + v] = GetVec4(&vertex, v) * invW;
				normalBuffer[vertexCount * 3 + v] = GetVec4(&normal, v);
			}
		}

		textureCount = mat->texOfs1.z >= 0 ? 3 : 1;
		texcoordBuffer[vertexCount * 4] = texcoord.x;
		texcoordBuffer[vertexCount * 4 + 1] = texcoord.y;
		texcoordBuffer[vertexCount * 4 + 2] = mat->texOfs1.x;
		texcoordBuffer[vertexCount * 4 + 3] = mat->texOfs1.y;

		if (textureCount > 1) {
			texOfsBuffer[vertexCount * 4] = (float)(mat->texOfs1.z);
			texOfsBuffer[vertexCount * 4 + 1] = (float)(mat->texOfs1.w);
			texOfsBuffer[vertexCount * 4 + 2] = (float)(mat->texOfs2.x);
			texOfsBuffer[vertexCount * 4 + 3] = (float)(mat->texOfs2.y);
		}

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

void Batch::setRenderData(int pass, BatchData* data) {
	vertexCount = data->vertexCount;
	indexCount = data->indexCount;
	objectCount = data->objectCount;
	if (drawcall) {
		drawcall->vertexCntToPrepare = vertexCount;
		drawcall->indexCntToPrepare = indexCount;
		drawcall->objectCntToPrepare = objectCount;
	}

	memcpy(vertexBuffer, data->vertices, vertexCount * 3 * sizeof(float));
	if (pass == NEAR_SHADOW_PASS || pass == MID_SHADOW_PASS || pass == COLOR_PASS) {
		memcpy(texcoordBuffer, data->texcoords, vertexCount * 4 * sizeof(float));
		if (pass == COLOR_PASS) {
			memcpy(normalBuffer, data->normals, vertexCount * 3 * sizeof(float));
			memcpy(colorBuffer, data->colors, vertexCount * 3 * sizeof(byte));
		}
	}
	memcpy(objectidBuffer, data->objectids, vertexCount * sizeof(byte));
	memcpy(indexBuffer, data->indices, indexCount * sizeof(uint));
	//memcpy(modelMatrices, data->matrices, objectCount * 12 * sizeof(float));
	matrixDataPtr = data->matrices;
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

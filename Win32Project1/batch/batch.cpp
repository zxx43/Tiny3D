#include "batch.h"
#include "../material/materialManager.h"
#include "../mesh/terrain.h"
#include <string.h>
#include <stdlib.h>

Batch::Batch() {
	vertexCount = 0;
	indexCount = 0;
	vertexBuffer = NULL;
	normalBuffer = NULL;
	tangentBuffer = NULL;
	texcoordBuffer = NULL;
	texidBuffer = NULL;
	colorBuffer = NULL;
	objectidBuffer = NULL;
	indexBuffer = NULL;

	fullStatic = false;
	hasTerrain = false;
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
	if (tangentBuffer) free(tangentBuffer); tangentBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (texidBuffer) free(texidBuffer); texidBuffer = NULL;
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
		if (tangentBuffer) free(tangentBuffer); tangentBuffer = NULL;
		if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
		if (texidBuffer) free(texidBuffer); texidBuffer = NULL;
		if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
		if (objectidBuffer) free(objectidBuffer); objectidBuffer = NULL;
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
	if (!tangentBuffer) tangentBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	if (!texcoordBuffer) texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	if (!texidBuffer) texidBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	if (!colorBuffer) colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));
	if (!objectidBuffer) objectidBuffer = (byte*)malloc(vertexCount * sizeof(byte));
	if (!indexBuffer) indexBuffer = (uint*)malloc(indexCount * sizeof(uint));

	if (!modelMatrices) modelMatrices = (float*)malloc(MAX_OBJECT_COUNT * 12 * sizeof(float));
	if (!normalMatrices) normalMatrices = (float*)malloc(MAX_OBJECT_COUNT * 9 * sizeof(float));

	flushBatchBuffers();
}

void Batch::pushMeshToBuffers(Mesh* mesh,int mid,bool fullStatic,const mat4& transformMatrix,const mat4& normalMatrix) {
	if (dynamic_cast<Terrain*>(mesh)) this->hasTerrain = true;
	this->fullStatic = fullStatic;
	int baseVertex = vertexCount;
	int currentObject = objectCount++;

	Material* mat = NULL;
	if (mid >= 0)
		mat = MaterialManager::materials->find(mid);
	if (!mat) mat = MaterialManager::materials->find(0);

	for (int i = 0; i < mesh->vertexCount; i++) {
		vec4 normal = mesh->normals4[i];
		vec3 tangent = mesh->tangents[i];
		vec4 tangent4 = vec4(tangent, 0.0);
		vec2 texcoord = mesh->texcoords[i];

		if (mesh->materialids)
			mat = MaterialManager::materials->find(mesh->materialids[i]);

		if (!fullStatic) {
			vec3 vertex3 = mesh->vertices3[i];
			for (int v = 0; v < 3; v++) {
				vertexBuffer[vertexCount * 3 + v] = GetVec3(&vertex3, v);
				normalBuffer[vertexCount * 3 + v] = GetVec4(&normal, v);
				tangentBuffer[vertexCount * 3 + v] = GetVec3(&tangent, v);
			}
		} else {
			vec4 vertex = transformMatrix * mesh->vertices[i];
			float invW = 1.0 / vertex.w;
			normal = normalMatrix * normal;
			tangent4 = normalMatrix * tangent4;
			for (int v = 0; v < 3; v++) {
				vertexBuffer[vertexCount * 3 + v] = GetVec4(&vertex, v) * invW;
				normalBuffer[vertexCount * 3 + v] = GetVec4(&normal, v);
				tangentBuffer[vertexCount * 3 + v] = GetVec4(&tangent4, v);
			}
		}

		texcoordBuffer[vertexCount * 4 + 0] = texcoord.x;
		texcoordBuffer[vertexCount * 4 + 1] = texcoord.y;
		texcoordBuffer[vertexCount * 4 + 2] = mat->exTexids.x;
		texcoordBuffer[vertexCount * 4 + 3] = mat->exTexids.y;

		texidBuffer[vertexCount * 4 + 0] = mat->texids.x;
		texidBuffer[vertexCount * 4 + 1] = mat->texids.y;
		texidBuffer[vertexCount * 4 + 2] = mat->texids.z;
		texidBuffer[vertexCount * 4 + 3] = mat->texids.w;

		colorBuffer[vertexCount * 3 + 0] = (byte)(mat->ambient.x * 255);
		colorBuffer[vertexCount * 3 + 1] = (byte)(mat->diffuse.x * 255);
		colorBuffer[vertexCount * 3 + 2] = (byte)(mat->specular.x * 255);

		objectidBuffer[vertexCount++] = currentObject;
	}

	for (int i = 0; i < mesh->indexCount; i++) 
		indexBuffer[indexCount++] = (uint)(baseVertex + mesh->indices[i]);

	if (!fullStatic && type == BATCH_TYPE_STATIC)
		initMatrix(currentObject, transformMatrix, normalMatrix);
}

void Batch::updateMatrices(unsigned short objectId, const mat4& transformMatrix, const mat4* normalMatrix) {
	memcpy(modelMatrices + (objectId * 12), transformMatrix.GetTranspose().entries, 12 * sizeof(float));
	if (normalMatrix) {
		memcpy(normalMatrices + (objectId * 9), normalMatrix->entries, 3 * sizeof(float));
		memcpy(normalMatrices + (objectId * 9 + 3), normalMatrix->entries + 4, 3 * sizeof(float));
		memcpy(normalMatrices + (objectId * 9 + 6), normalMatrix->entries + 8, 3 * sizeof(float));
	}
}

void Batch::initMatrix(unsigned short currentObject, const mat4& transformMatrix, const mat4& normalMatrix) {
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
			memcpy(texidBuffer, data->texids, vertexCount * 4 * sizeof(float));
			memcpy(normalBuffer, data->normals, vertexCount * 3 * sizeof(float));
			memcpy(tangentBuffer, data->tangents, vertexCount * 3 * sizeof(float));
			memcpy(colorBuffer, data->colors, vertexCount * 3 * sizeof(byte));
		}
	}
	memcpy(objectidBuffer, data->objectids, vertexCount * sizeof(byte));
	memcpy(indexBuffer, data->indices, indexCount * sizeof(uint));
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

#include "meshData.h"
#include "../mesh/mesh.h"
#include "../animation/animation.h"

void MeshData::createMeshBuffers() {
	isAnim = false;
	vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	normalBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	tangentBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	texidBuffer = (float*)malloc(vertexCount * 2 * sizeof(float));
	colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));
	indexBuffer = (ushort*)malloc(indexCount * sizeof(ushort));
	boneids = NULL, weights = NULL;
}

void MeshData::createAnimBuffers() {
	isAnim = true;
	boneids = (byte*)malloc(vertexCount * 4 * sizeof(byte));
	weights = (half*)malloc(vertexCount * 4 * sizeof(half));
}

MeshData::MeshData(Mesh* mesh) {
	vertexCount = mesh->vertexCount;
	indexCount = mesh->indexCount;
	createMeshBuffers();

	for (int i = 0; i < vertexCount; i++) {
		vec4 vertex = mesh->vertices[i];
		vec3 normal = mesh->normals[i];
		vec3 tangent = mesh->tangents[i];
		vec2 texcoord = mesh->texcoords[i];

		int mid = mesh->materialids ? mesh->materialids[i] : 0;

		for (int v = 0; v < 3; v++) {
			vertexBuffer[i * 3 + v] = GetVec4(&vertex, v);
			normalBuffer[i * 3 + v] = Float2Half(GetVec3(&normal, v));
			tangentBuffer[i * 3 + v] = Float2Half(GetVec3(&tangent, v));
		}

		texcoordBuffer[i * 4 + 0] = (texcoord.x);
		texcoordBuffer[i * 4 + 1] = (texcoord.y);
		texcoordBuffer[i * 4 + 2] = mid;
	}
	for (int i = 0; i < indexCount; i++) 
		indexBuffer[i] = (ushort)(mesh->indices[i]);

	meshCount = 1;
}

MeshData::MeshData(Animation* anim) {
	vertexCount = anim->aVertices.size();
	indexCount = anim->aIndices.size();
	createMeshBuffers();
	createAnimBuffers();

	for (int i = 0; i < vertexCount; i++) {
		SetVec3(anim->aVertices[i], vertexBuffer, i);
		for (int v = 0; v < 3; v++) {
			normalBuffer[i * 3 + v] = Float2Half(GetVec3(&anim->aNormals[i], v));
			if (anim->aTangents.size() > 0)
				tangentBuffer[i * 3 + v] = Float2Half(GetVec3(&anim->aTangents[i], v));
		}

		texcoordBuffer[i * 4 + 0] = anim->aTexcoords[i].x;
		texcoordBuffer[i * 4 + 1] = anim->aTexcoords[i].y;
		texcoordBuffer[i * 4 + 2] = anim->aMids[i];

		SetUVec4(anim->aBoneids[i], boneids, i);
		for (int v = 0; v < 4; v++)
			weights[i * 4 + v] = Float2Half(GetVec4(&anim->aWeights[i], v));
	}
	for (int i = 0; i < indexCount; i++)
		indexBuffer[i] = (ushort)(anim->aIndices[i]);

	meshCount = 1;
}

MeshData::MeshData() {
	isAnim = false;
	vertexBuffer = NULL;
	normalBuffer = NULL;
	tangentBuffer = NULL;
	texcoordBuffer = NULL;
	texidBuffer = NULL;
	colorBuffer = NULL;
	indexBuffer = NULL;
	boneids = NULL;
	weights = NULL;
	vertexCount = 0;
	indexCount = 0;
	meshCount = 0;
}

MeshData::MeshData(int vSize, int iSize, bool isA) {
	isAnim = isA;
	vertexBuffer = (float*)malloc(vSize * 3 * sizeof(float));
	normalBuffer = (half*)malloc(vSize * 3 * sizeof(half));
	tangentBuffer = (half*)malloc(vSize * 3 * sizeof(half));
	texcoordBuffer = (float*)malloc(vSize * 4 * sizeof(float));
	texidBuffer = (float*)malloc(vSize * 2 * sizeof(float));
	colorBuffer = (byte*)malloc(vSize * 3 * sizeof(byte));
	indexBuffer = (ushort*)malloc(iSize * sizeof(ushort));
	boneids = NULL, weights = NULL;
	if (isAnim) {
		boneids = (byte*)malloc(vSize * 4 * sizeof(byte));
		weights = (half*)malloc(vSize * 4 * sizeof(half));
	}
	vertexCount = 0, indexCount = 0;
	meshCount = 0;
}

void MeshData::append(MeshData* data) {
	isAnim = isAnim || data->isAnim;

	int newVertexCount = vertexCount + data->vertexCount;
	int newIndexCount = indexCount + data->indexCount;
	
	float* vertexTmp = (float*)malloc(newVertexCount * 3 * sizeof(float));
	half* normalTmp = (half*)malloc(newVertexCount * 3 * sizeof(half));
	half* tangentTmp = (half*)malloc(newVertexCount * 3 * sizeof(half));
	float* texcoordTmp = (float*)malloc(newVertexCount * 4 * sizeof(float));
	float* texidTmp = (float*)malloc(newVertexCount * 2 * sizeof(float));
	byte* colorTmp = (byte*)malloc(newVertexCount * 3 * sizeof(byte));
	ushort* indexTmp = (ushort*)malloc(newIndexCount * sizeof(ushort));
	byte* boneidTmp = isAnim ? (byte*)malloc(newVertexCount * 4 * sizeof(byte)) : NULL;
	half* weightTmp = isAnim ? (half*)malloc(newVertexCount * 4 * sizeof(half)) : NULL;

	if (vertexCount > 0) {
		memcpy(vertexTmp, vertexBuffer, vertexCount * 3 * sizeof(float));
		memcpy(normalTmp, normalBuffer, vertexCount * 3 * sizeof(half));
		memcpy(tangentTmp, tangentBuffer, vertexCount * 3 * sizeof(half));
		memcpy(texcoordTmp, texcoordBuffer, vertexCount * 4 * sizeof(float));
		memcpy(texidTmp, texidBuffer, vertexCount * 2 * sizeof(float));
		memcpy(colorTmp, colorBuffer, vertexCount * 3 * sizeof(byte));
		if (isAnim) {
			memcpy(boneidTmp, boneids, vertexCount * 4 * sizeof(byte));
			memcpy(weightTmp, weights, vertexCount * 4 * sizeof(half));
		}
	}
	if (indexCount > 0) memcpy(indexTmp, indexBuffer, indexCount * sizeof(ushort));

	memcpy(vertexTmp + vertexCount * 3, data->vertexBuffer, data->vertexCount * 3 * sizeof(float));
	memcpy(normalTmp + vertexCount * 3, data->normalBuffer, data->vertexCount * 3 * sizeof(half));
	memcpy(tangentTmp + vertexCount * 3, data->tangentBuffer, data->vertexCount * 3 * sizeof(half));
	memcpy(texcoordTmp + vertexCount * 4, data->texcoordBuffer, data->vertexCount * 4 * sizeof(float));
	memcpy(texidTmp + vertexCount * 2, data->texidBuffer, data->vertexCount * 2 * sizeof(float));
	memcpy(colorTmp + vertexCount * 3, data->colorBuffer, data->vertexCount * 3 * sizeof(byte));
	memcpy(indexTmp + indexCount, data->indexBuffer, data->indexCount * sizeof(ushort));
	if (isAnim) {
		memcpy(boneidTmp + vertexCount * 4, data->boneids, data->vertexCount * 4 * sizeof(byte));
		memcpy(weightTmp + vertexCount * 4, data->weights, data->vertexCount * 4 * sizeof(half));
	}

	releaseBuffers();

	vertexCount = newVertexCount;
	indexCount = newIndexCount;

	vertexBuffer = vertexTmp;
	normalBuffer = normalTmp;
	tangentBuffer = tangentTmp;
	texcoordBuffer = texcoordTmp;
	texidBuffer = texidTmp;
	colorBuffer = colorTmp;
	indexBuffer = indexTmp;
	boneids = boneidTmp;
	weights = weightTmp;

	meshCount += data->meshCount;
}

void MeshData::releaseBuffers() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (tangentBuffer) free(tangentBuffer); tangentBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (texidBuffer) free(texidBuffer); texidBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;
	if (boneids) free(boneids); boneids = NULL;
	if (weights) free(weights); weights = NULL;
}

MeshData::~MeshData() {
	releaseBuffers();
}
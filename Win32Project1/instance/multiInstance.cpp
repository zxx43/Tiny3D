#include "multiInstance.h"

const int MaxInstance = 4096;

MultiInstance::MultiInstance() {
	vertexBuffer = NULL;
	normalBuffer = NULL;
	tangentBuffer = NULL;
	texcoordBuffer = NULL;
	texidBuffer = NULL;
	colorBuffer = NULL;
	indexBuffer = NULL;
	transforms = NULL;

	insDatas.clear();
	indirects = NULL;
	indirectCount = 0;

	normals.clear(), singles.clear(), bills.clear();
	indirectsNormal = NULL, indirectsSingle = NULL, indirectsBill = NULL;
	normalCount = 0, singleCount = 0, billCount = 0;
	normalBases = NULL, singleBases = NULL, billBases = NULL;

	vertexCount = 0;
	indexCount = 0;
	instanceCount = 0;
	maxInstance = 0;

	bufferInited = false;
	drawcall = NULL;
}

void MultiInstance::releaseInstanceData() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (tangentBuffer) free(tangentBuffer); tangentBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (texidBuffer) free(texidBuffer); texidBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	for (uint i = 0; i < normals.size(); i++)
		free(normals[i]);
	normals.clear();
	for (uint i = 0; i < singles.size(); i++)
		free(singles[i]);
	singles.clear();
	for (uint i = 0; i < bills.size(); i++)
		free(bills[i]);
	bills.clear();
	if (indirects) free(indirects); indirects = NULL;

	if (indirectsNormal) free(indirectsNormal); indirectsNormal = NULL;
	if (indirectsSingle) free(indirectsSingle); indirectsSingle = NULL;
	if (indirectsBill) free(indirectsBill); indirectsBill = NULL;
}

MultiInstance::~MultiInstance() {
	releaseInstanceData();
	if (transforms) free(transforms);

	insDatas.clear();

	if (normalBases) free(normalBases);
	if (singleBases) free(singleBases);
	if (billBases) free(billBases);
	
	if (drawcall) delete drawcall;
}

void MultiInstance::add(Instance* instance) {
	insDatas.push_back(instance);
}

void MultiInstance::initBuffers() {
	indirectCount = insDatas.size();
	indirects = (Indirect*)malloc(indirectCount * sizeof(Indirect));

	vertexCount = 0, indexCount = 0, maxInstance = 0;
	for (uint i = 0; i < indirectCount; ++i) {
		Instance* ins = insDatas[i];
		indirects[i].baseVertex = vertexCount;
		indirects[i].count = ins->indexCount;
		indirects[i].firstIndex = indexCount;
		indirects[i].primCount = 0;
		indirects[i].baseInstance = 0;

		if (ins->isBillboard) {
			Indirect* idBill = (Indirect*)malloc(sizeof(Indirect));
			memcpy(idBill, indirects + i, sizeof(Indirect));

			bills.push_back(idBill);
			ins->insBillId = bills.size() - 1;
		} else {
			if (ins->instanceMesh->normalFaces.size() > 0) {
				Indirect* idNorm = (Indirect*)malloc(sizeof(Indirect));
				memcpy(idNorm, indirects + i, sizeof(Indirect));

				FaceBuf* buf = ins->instanceMesh->normalFaces[0];
				idNorm->count = buf->count;
				idNorm->firstIndex = indexCount + buf->start;
				normals.push_back(idNorm);
				ins->insId = normals.size() - 1;
			}
			if (ins->instanceMesh->singleFaces.size() > 0) {
				Indirect* idSing = (Indirect*)malloc(sizeof(Indirect));
				memcpy(idSing, indirects + i, sizeof(Indirect));

				FaceBuf* buf = ins->instanceMesh->singleFaces[0];
				idSing->count = buf->count;
				idSing->firstIndex = indexCount + buf->start;
				singles.push_back(idSing);
				ins->insSingleId = singles.size() - 1;
			}
		}

		vertexCount += ins->vertexCount;
		indexCount += ins->indexCount;
		maxInstance += ins->maxInstanceCount > MaxInstance ? MaxInstance : ins->maxInstanceCount;
	}
	normalCount = normals.size(), singleCount = singles.size(), billCount = bills.size();
	indirectsNormal = (Indirect*)malloc(normalCount * sizeof(Indirect));
	indirectsSingle = (Indirect*)malloc(singleCount * sizeof(Indirect));
	indirectsBill = (Indirect*)malloc(billCount * sizeof(Indirect));
	for (uint i = 0; i < normalCount; i++)
		memcpy(indirectsNormal + i, normals[i], sizeof(Indirect));
	for (uint i = 0; i < singleCount; i++)
		memcpy(indirectsSingle + i, singles[i], sizeof(Indirect));
	for (uint i = 0; i < billCount; i++)
		memcpy(indirectsBill + i, bills[i], sizeof(Indirect));

	normalBases = (uint*)malloc(normalCount * sizeof(uint));
	singleBases = (uint*)malloc(singleCount * sizeof(uint));
	billBases = (uint*)malloc(billCount * sizeof(uint));

	vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	normalBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	tangentBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	texidBuffer = (float*)malloc(vertexCount * 2 * sizeof(float));
	colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));
	indexBuffer = (ushort*)malloc(indexCount * sizeof(ushort));
	transforms = (buff*)malloc(maxInstance * 16 * sizeof(buff));

	uint curVertex = 0, curIndex = 0;
	for (uint i = 0; i < indirectCount; ++i) {
		Instance* ins = insDatas[i];
		memcpy(vertexBuffer + curVertex * 3, ins->vertexBuffer, ins->vertexCount * 3 * sizeof(float));
		memcpy(normalBuffer + curVertex * 3, ins->normalBuffer, ins->vertexCount * 3 * sizeof(half));
		memcpy(tangentBuffer + curVertex * 3, ins->tangentBuffer, ins->vertexCount * 3 * sizeof(half));
		memcpy(texcoordBuffer + curVertex * 4, ins->texcoordBuffer, ins->vertexCount * 4 * sizeof(float));
		memcpy(texidBuffer + curVertex * 2, ins->texidBuffer, ins->vertexCount * 2 * sizeof(float));
		memcpy(colorBuffer + curVertex * 3, ins->colorBuffer, ins->vertexCount * 3 * sizeof(byte));
		memcpy(indexBuffer + curIndex, ins->indexBuffer, ins->indexCount * sizeof(ushort));
		curVertex += ins->vertexCount;
		curIndex += ins->indexCount;
	}
	bufferInited = true;
}

void MultiInstance::updateTransform() {
	instanceCount = 0;
	int curNorm = 0, curSing = 0, curBill = 0;
	for (uint i = 0; i < indirectCount; i++) {
		Instance* ins = insDatas[i];
		if (ins->isBillboard) 
			billBases[curBill++] = instanceCount;
		else {
			if (ins->instanceMesh->normalFaces.size() > 0) 
				normalBases[curNorm++] = instanceCount;
			if (ins->instanceMesh->singleFaces.size() > 0) 
				singleBases[curSing++] = instanceCount;
		}

		if (ins->instanceCount > 0) {
			memcpy(transforms + instanceCount * 16, ins->modelTransform, ins->instanceCount * 16 * sizeof(buff));
			instanceCount += ins->instanceCount;
		}
	}
}
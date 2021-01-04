#include "multiInstance.h"

const int MaxInstance = 4096;

MultiInstance::MultiInstance() {
	vertexBuffer = NULL, normalBuffer = NULL, tangentBuffer = NULL;
	texcoordBuffer = NULL, texidBuffer = NULL, colorBuffer = NULL;
	boneidBuffer = NULL, weightBuffer = NULL;
	indexBuffer = NULL;
	transforms = NULL;

	insDatas.clear();
	animDatas.clear();

	normalIns.clear();
	mixedIns.clear();
	singleIns.clear();
	billIns.clear();

	indirects = NULL;
	indirectCount = 0;

	normals.clear(), singles.clear(), bills.clear(), anims.clear();
	indirectsNormal = NULL, indirectsSingle = NULL, indirectsBill = NULL, indirectsAnim = NULL;
	normalCount = 0, singleCount = 0, billCount = 0, animCount = 0;
	meshCount = 0, bases = NULL;

	vertexCount = 0, indexCount = 0, maxInstance = 0, instanceCount = 0;

	hasAnim = false;
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
	if (boneidBuffer) free(boneidBuffer); boneidBuffer = NULL;
	if (weightBuffer) free(weightBuffer); weightBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	for (uint i = 0; i < normals.size(); i++) free(normals[i]);
	for (uint i = 0; i < singles.size(); i++) free(singles[i]);
	for (uint i = 0; i < bills.size(); i++) free(bills[i]);
	for (uint i = 0; i < anims.size(); i++) free(anims[i]);
	normals.clear(), singles.clear(), bills.clear(), anims.clear();

	if (indirects) free(indirects); indirects = NULL;
	if (indirectsNormal) free(indirectsNormal); indirectsNormal = NULL;
	if (indirectsSingle) free(indirectsSingle); indirectsSingle = NULL;
	if (indirectsBill) free(indirectsBill); indirectsBill = NULL;
	if (indirectsAnim) free(indirectsAnim); indirectsAnim = NULL;
}

MultiInstance::~MultiInstance() {
	releaseInstanceData();
	if (transforms) free(transforms);

	insDatas.clear();
	animDatas.clear();

	normalIns.clear();
	mixedIns.clear();
	singleIns.clear();
	billIns.clear();

	if (bases) free(bases);
	if (drawcall) delete drawcall;
}

void MultiInstance::add(Instance* instance) {
	if (instance->hasNormal && !instance->hasSingle) normalIns.push_back(instance);
	else if (instance->hasNormal && instance->hasSingle) mixedIns.push_back(instance);
	else if (instance->hasSingle) singleIns.push_back(instance);
	else if (instance->isBillboard) billIns.push_back(instance);
}

void MultiInstance::add(AnimationData* animData) {
	animDatas.push_back(animData);
	hasAnim = true;
}

void MultiInstance::initBuffers(int pass) {
	if (!hasAnim) {
		uint n = 0, m = 0, s = 0, b = 0;
		for (; n < normalIns.size() && (pass == ALL_PASS || pass == NORMAL_PASS); ++n)
			insDatas.push_back(normalIns[n]);
		for (; m < mixedIns.size() && (pass == ALL_PASS || pass == NORMAL_PASS || pass == SINGLE_PASS); ++m)
			insDatas.push_back(mixedIns[m]);
		for (; s < singleIns.size() && (pass == ALL_PASS || pass == SINGLE_PASS); ++s)
			insDatas.push_back(singleIns[s]);
		for (; b < billIns.size() && (pass == ALL_PASS || pass == BILL_PASS); ++b)
			insDatas.push_back(billIns[b]);
		printf("normal: %d, mixed: %d, single: %d, bill: %d\n", n, m, s, b);
		normalIns.clear(), mixedIns.clear(), singleIns.clear(), billIns.clear();
	}

	bufferPass = pass;
	indirectCount = hasAnim ? animDatas.size() : insDatas.size();
	indirects = (Indirect*)malloc(indirectCount * sizeof(Indirect));

	vertexCount = 0, indexCount = 0, maxInstance = 0;
	for (uint i = 0; i < indirectCount; ++i) {
		if (!hasAnim) {
			Instance* ins = insDatas[i];
			indirects[i].baseVertex = vertexCount;
			indirects[i].count = ins->indexCount;
			indirects[i].firstIndex = indexCount;
			indirects[i].primCount = 0;
			indirects[i].baseInstance = 0;

			bool pushed = false;
			if (ins->isBillboard && (bufferPass == ALL_PASS || bufferPass == BILL_PASS)) {
				Indirect* idBill = (Indirect*)malloc(sizeof(Indirect));
				memcpy(idBill, indirects + i, sizeof(Indirect));

				bills.push_back(idBill);
				ins->insBillId = bills.size() - 1;
				pushed = true;
			} else {
				if (ins->hasNormal && (bufferPass == ALL_PASS || bufferPass == NORMAL_PASS)) {
					Indirect* idNorm = (Indirect*)malloc(sizeof(Indirect));
					memcpy(idNorm, indirects + i, sizeof(Indirect));

					FaceBuf* buf = ins->instanceMesh->normalFaces[0];
					idNorm->count = buf->count;
					idNorm->firstIndex = indexCount + buf->start;
					normals.push_back(idNorm);
					ins->insId = normals.size() - 1;
					pushed = true;
				}
				if (ins->hasSingle && (bufferPass == ALL_PASS || bufferPass == SINGLE_PASS)) {
					Indirect* idSing = (Indirect*)malloc(sizeof(Indirect));
					memcpy(idSing, indirects + i, sizeof(Indirect));

					FaceBuf* buf = ins->instanceMesh->singleFaces[0];
					idSing->count = buf->count;
					idSing->firstIndex = indexCount + buf->start;
					singles.push_back(idSing);
					ins->insSingleId = singles.size() - 1;
					pushed = true;
				}
			}

			if (pushed) {
				vertexCount += ins->vertexCount, indexCount += ins->indexCount;
				maxInstance += ins->maxInstanceCount > MaxInstance ? MaxInstance : ins->maxInstanceCount;
			}
		} else {
			AnimationData* anim = animDatas[i];
			indirects[i].baseVertex = vertexCount;
			indirects[i].count = anim->indexCount;
			indirects[i].firstIndex = indexCount;
			indirects[i].primCount = 0;
			indirects[i].baseInstance = 0;

			Indirect* idAnim = (Indirect*)malloc(sizeof(Indirect));
			memcpy(idAnim, indirects + i, sizeof(Indirect));
			anims.push_back(idAnim);
			anim->animId = anims.size() - 1;

			vertexCount += anim->vertexCount, indexCount += anim->indexCount;
			maxInstance += anim->maxAnim > MaxInstance ? MaxInstance : anim->maxAnim;
		}
	}

	normalCount = normals.size(), singleCount = singles.size(), billCount = bills.size(), animCount = anims.size();
	meshCount = normalCount > singleCount ? normalCount : singleCount;
	meshCount = meshCount > billCount ? meshCount : billCount;
	meshCount = meshCount > animCount ? meshCount : animCount;

	if (!hasAnim) {
		indirectsNormal = (Indirect*)malloc(normalCount * sizeof(Indirect));
		indirectsSingle = (Indirect*)malloc(singleCount * sizeof(Indirect));
		indirectsBill = (Indirect*)malloc(billCount * sizeof(Indirect));
		for (uint k = 0; k < normalCount; k++)
			memcpy(indirectsNormal + k, normals[k], sizeof(Indirect));
		for (uint k = 0; k < singleCount; k++)
			memcpy(indirectsSingle + k, singles[k], sizeof(Indirect));
		for (uint k = 0; k < billCount; k++)
			memcpy(indirectsBill + k, bills[k], sizeof(Indirect));
	} else {
		indirectsAnim = (Indirect*)malloc(animCount * sizeof(Indirect));
		for (uint k = 0; k < animCount; k++)
			memcpy(indirectsAnim + k, anims[k], sizeof(Indirect));
	}
	bases = (uint*)malloc(meshCount * 4 * sizeof(uint));
	memset(bases, 0, meshCount * 4 * sizeof(uint));

	vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	normalBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	tangentBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	texidBuffer = (float*)malloc(vertexCount * 2 * sizeof(float));
	colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));
	if (hasAnim) {
		boneidBuffer = (byte*)malloc(vertexCount * 4 * sizeof(byte));
		weightBuffer = (half*)malloc(vertexCount * 4 * sizeof(half));
	}
	indexBuffer = (ushort*)malloc(indexCount * sizeof(ushort));
	transforms = (buff*)malloc(maxInstance * 16 * sizeof(buff));

	uint curVertex = 0, curIndex = 0;
	for (uint i = 0; i < indirectCount; ++i) {
		if (!hasAnim) {
			Instance* ins = insDatas[i];
			memcpy(vertexBuffer + curVertex * 3, ins->vertexBuffer, ins->vertexCount * 3 * sizeof(float));
			memcpy(normalBuffer + curVertex * 3, ins->normalBuffer, ins->vertexCount * 3 * sizeof(half));
			memcpy(tangentBuffer + curVertex * 3, ins->tangentBuffer, ins->vertexCount * 3 * sizeof(half));
			memcpy(texcoordBuffer + curVertex * 4, ins->texcoordBuffer, ins->vertexCount * 4 * sizeof(float));
			memcpy(texidBuffer + curVertex * 2, ins->texidBuffer, ins->vertexCount * 2 * sizeof(float));
			memcpy(colorBuffer + curVertex * 3, ins->colorBuffer, ins->vertexCount * 3 * sizeof(byte));
			memcpy(indexBuffer + curIndex, ins->indexBuffer, ins->indexCount * sizeof(ushort));
			curVertex += ins->vertexCount, curIndex += ins->indexCount;
		} else {
			AnimationData* anim = animDatas[i];
			memcpy(vertexBuffer + curVertex * 3, anim->vertices, anim->vertexCount * 3 * sizeof(float));
			memcpy(normalBuffer + curVertex * 3, anim->normals, anim->vertexCount * 3 * sizeof(half));
			memcpy(tangentBuffer + curVertex * 3, anim->tangents, anim->vertexCount * 3 * sizeof(half));
			memcpy(texcoordBuffer + curVertex * 4, anim->texcoords, anim->vertexCount * 4 * sizeof(float));
			memcpy(texidBuffer + curVertex * 2, anim->texids, anim->vertexCount * 2 * sizeof(float));
			memcpy(colorBuffer + curVertex * 3, anim->colors, anim->vertexCount * 3 * sizeof(byte));
			memcpy(boneidBuffer + curVertex * 4, anim->boneids, anim->vertexCount * 4 * sizeof(byte));
			memcpy(weightBuffer + curVertex * 4, anim->weights, anim->vertexCount * 4 * sizeof(half));
			memcpy(indexBuffer + curIndex, anim->indices, anim->indexCount * sizeof(ushort));
			curVertex += anim->vertexCount, curIndex += anim->indexCount;
		}
	}
	bufferInited = true;
}

int MultiInstance::updateTransform(buff* targetBuffer) {
	instanceCount = 0;
	buff* target = targetBuffer ? targetBuffer : transforms;
	for (uint i = 0; i < indirectCount; ++i) {
		if (!hasAnim) {
			Instance* ins = insDatas[i];
			bool pushed = false;
			if (ins->hasNormal && (bufferPass == ALL_PASS || bufferPass == NORMAL_PASS)) {
				bases[ins->insId * 4 + 0] = instanceCount;
				pushed = true;
			}
			if (ins->hasSingle && (bufferPass == ALL_PASS || bufferPass == SINGLE_PASS)) {
				bases[ins->insSingleId * 4 + 1] = instanceCount;
				pushed = true;
			}
			if (ins->isBillboard && (bufferPass == ALL_PASS || bufferPass == BILL_PASS)) {
				bases[ins->insBillId * 4 + 2] = instanceCount;
				pushed = true;
			}

			if (ins->insData->count > 0 && pushed) {
				memcpy(target + instanceCount * 16, ins->insData->transformsFull, ins->insData->count * 16 * sizeof(buff));
				instanceCount += ins->insData->count;
			}
		} else {
			AnimationData* anim = animDatas[i];
			bases[anim->animId * 4 + 3] = instanceCount;
			if (anim->animCount > 0) {
				memcpy(target + instanceCount * 16, anim->transformsFull, anim->animCount * 16 * sizeof(buff));
				instanceCount += anim->animCount;
			}
		}
	}
	return instanceCount;
}
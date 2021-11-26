#include "animationData.h"

AnimationData::AnimationData(Animation* anim, int maxCount) : DataBuffer(ANIMATE_BUFFER) {
	animId = -1;
	indexCount = anim->aIndices.size();
	vertexCount = anim->aVertices.size();
	vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	normalBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	tangentBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	texidBuffer = (float*)malloc(vertexCount * 2 * sizeof(float));
	colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));
	boneids = (byte*)malloc(vertexCount * 4 * sizeof(byte));
	weights = (half*)malloc(vertexCount * 4 * sizeof(half));
	indexBuffer = (ushort*)malloc(indexCount * sizeof(ushort));

	for (uint i = 0; i < (uint)vertexCount; i++) {
		SetVec3(anim->aVertices[i], vertexBuffer, i);
		for (int v = 0; v < 3; v++) {
			normalBuffer[i * 3 + v] = Float2Half(GetVec3(&anim->aNormals[i], v));
			if (anim->aTangents.size() > 0)
				tangentBuffer[i * 3 + v] = Float2Half(GetVec3(&anim->aTangents[i], v));
		}

		texcoordBuffer[i * 4 + 0] = anim->aTexcoords[i].x;
		texcoordBuffer[i * 4 + 1] = anim->aTexcoords[i].y;
		texcoordBuffer[i * 4 + 2] = anim->aMids[i];
		//texcoordBuffer[i * 4 + 2] = anim->aTextures[i]->texids.x;
		//texcoordBuffer[i * 4 + 3] = anim->aTextures[i]->texids.y;
		//texidBuffer[i * 2 + 0] = anim->aTextures[i]->texids.z;
		//texidBuffer[i * 2 + 1] = anim->aTextures[i]->texids.w;

		//colorBuffer[i * 3 + 0] = (byte)(anim->aAmbients[i].x * 255);
		//colorBuffer[i * 3 + 1] = (byte)(anim->aDiffuses[i].x * 255);
		//colorBuffer[i * 3 + 2] = (byte)(anim->aSpeculars[i].x * 255);

		SetUVec4(anim->aBoneids[i], boneids, i);
		for (uint v = 0; v < 4; v++)
			weights[i * 4 + v] = Float2Half(GetVec4(&anim->aWeights[i], v));
	}
	for (uint i = 0; i < (uint)indexCount; i++)
		indexBuffer[i] = (ushort)(anim->aIndices[i]);

	this->maxCount = maxCount;
	transformsFull = (buff*)malloc(this->maxCount * 16 * sizeof(buff));
	memset(transformsFull, 0, this->maxCount * 16 * sizeof(buff));
	animCount = 0;
}

AnimationData::~AnimationData() {
	releaseDatas();
	free(transformsFull);
	transformsFull = NULL;
}

void AnimationData::releaseDatas() {
	DataBuffer::releaseDatas();
	if (boneids) free(boneids); boneids = NULL;
	if (weights) free(weights); weights = NULL;
}

void AnimationData::addAnimObject(Object* object, bool uniformScale) {
	if (transformsFull) {
		memcpy(transformsFull + (animCount * 16), object->transformsFull, 12 * sizeof(buff));

		AnimationObject* animObj = (AnimationObject*)object;
		transformsFull[animCount * 16 + 12] = animObj->fid + 0.1;
		transformsFull[animCount * 16 + 13] = animObj->getCurFrame();
		transformsFull[animCount * 16 + 14] = animId + 0.1;
		transformsFull[animCount * 16 + 15] = object->material;

		vec4 quat(transformsFull[animCount * 16 + 4], transformsFull[animCount * 16 + 5], transformsFull[animCount * 16 + 6], transformsFull[animCount * 16 + 7]);
		vec3 quat3 = EncodeQuat(quat, true);
		transformsFull[animCount * 16 + 4] = quat3.x;
		transformsFull[animCount * 16 + 5] = quat3.y;
		transformsFull[animCount * 16 + 6] = quat3.z;

		if (uniformScale) transformsFull[animCount * 16 + 7] = 1.0;
		else {
			vec3 scale(object->scaleMat[0], object->scaleMat[5], object->scaleMat[10]);
			float pScale = PackVec2Float(scale);
			transformsFull[animCount * 16 + 3] = pScale;
			transformsFull[animCount * 16 + 7] = -1.0;
		}

		animCount++;
	}
}
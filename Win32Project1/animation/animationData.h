#ifndef ANIMATION_DATA_H_
#define ANIMATION_DATA_H_

#include "../animation/animation.h"
#include "../object/animationObject.h"
#include "../constants/constants.h"

class AnimationDrawcall;

struct AnimationData {
	int animId;
	float* vertices;
	half* normals;
	half* tangents;
	float* texcoords;
	float* texids;
	byte* colors;
	byte* boneids;
	half* weights;
	ushort* indices;
	int indexCount, vertexCount, animCount, maxAnim;
	buff* transformsFull;

	AnimationData(Animation* anim, int maxCount) {
		animId = -1;
		indexCount = anim->aIndices.size();
		vertexCount = anim->aVertices.size();
		vertices = (float*)malloc(vertexCount * 3 * sizeof(float));
		normals = (half*)malloc(vertexCount * 3 * sizeof(half));
		tangents = (half*)malloc(vertexCount * 3 * sizeof(half));
		texcoords = (float*)malloc(vertexCount * 4 * sizeof(float));
		texids = (float*)malloc(vertexCount * 2 * sizeof(float));
		colors = (byte*)malloc(vertexCount * 3 * sizeof(byte));
		boneids = (byte*)malloc(vertexCount * 4 * sizeof(byte));
		weights = (half*)malloc(vertexCount * 4 * sizeof(half));
		indices = (ushort*)malloc(indexCount * sizeof(ushort));

		for (uint i = 0; i < (uint)vertexCount; i++) {
			SetVec3(anim->aVertices[i], vertices, i);
			for (int v = 0; v < 3; v++) {
				normals[i * 3 + v] = Float2Half(GetVec3(&anim->aNormals[i], v));
				if (anim->aTangents.size() > 0) 
					tangents[i * 3 + v] = Float2Half(GetVec3(&anim->aTangents[i], v));
			}

			texcoords[i * 4 + 0] = anim->aTexcoords[i].x;
			texcoords[i * 4 + 1] = anim->aTexcoords[i].y;
			texcoords[i * 4 + 2] = anim->aTextures[i]->texids.x;
			texcoords[i * 4 + 3] = anim->aTextures[i]->texids.y;
			texids[i * 2 + 0] = anim->aTextures[i]->texids.z;
			texids[i * 2 + 1] = anim->aTextures[i]->texids.w;

			colors[i * 3 + 0] = (byte)(anim->aAmbients[i].x * 255);
			colors[i * 3 + 1] = (byte)(anim->aDiffuses[i].x * 255);
			colors[i * 3 + 2] = (byte)(anim->aSpeculars[i].x * 255);

			SetUVec4(anim->aBoneids[i], boneids, i);
			for (uint v = 0; v < 4; v++)
				weights[i * 4 + v] = Float2Half(GetVec4(&anim->aWeights[i], v));
		}
		for (uint i = 0; i < (uint)indexCount; i++)
			indices[i] = (ushort)(anim->aIndices[i]);

		maxAnim = maxCount;
		transformsFull = (buff*)malloc(maxAnim * 16 * sizeof(buff));
		memset(transformsFull, 0, maxAnim * 16 * sizeof(buff));
		animCount = 0;
	}
	~AnimationData() {
		releaseAnimData();
		free(transformsFull);
		transformsFull = NULL;
	}
	void releaseAnimData() {
		if (vertices) free(vertices); vertices = NULL;
		if (normals) free(normals); normals = NULL;
		if (tangents) free(tangents); tangents = NULL;
		if (texcoords) free(texcoords); texcoords = NULL;
		if (texids) free(texids); texids = NULL;
		if (colors) free(colors); colors = NULL;
		if (boneids) free(boneids); boneids = NULL;
		if (weights) free(weights); weights = NULL;
		if (indices) free(indices); indices = NULL;
	}
	void resetAnims() {
		animCount = 0;
	}
	void addAnimObject(Object* object) {
		if (transformsFull) {
			memcpy(transformsFull + (animCount * 16), object->transformsFull, 12 * sizeof(buff));

			AnimationObject* animObj = (AnimationObject*)object;
			transformsFull[animCount * 16 + 12] = animObj->fid + 0.1;
			transformsFull[animCount * 16 + 13] = animObj->getCurFrame();
			transformsFull[animCount * 16 + 14] = 0.0;
			transformsFull[animCount * 16 + 15] = animId + 0.1;
			animCount++;
		}
	}
};

#endif

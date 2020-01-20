#ifndef ANIMATION_DATA_H_
#define ANIMATION_DATA_H_

#include "../animation/animation.h"
#include "../constants/constants.h"

struct AnimationData {
	float* vertices;
	float* normals;
	float* tangents;
	float* texcoords;
	float* texids;
	byte* colors;
	byte* boneids;
	float* weights;
	ushort* indices;
	int indexCount, vertexCount, boneCount;
	Animation* animation;

	AnimationData(Animation* anim) {
		indexCount = anim->aIndices.size();
		vertexCount = anim->aVertices.size();
		boneCount = anim->boneCount;
		vertices = (float*)malloc(vertexCount * 3 * sizeof(float));
		normals = (float*)malloc(vertexCount * 3 * sizeof(float));
		tangents = (float*)malloc(vertexCount * 3 * sizeof(float));
		texcoords = (float*)malloc(vertexCount * 4 * sizeof(float));
		texids = (float*)malloc(vertexCount * 2 * sizeof(float));
		colors = (byte*)malloc(vertexCount * 3 * sizeof(byte));
		boneids = (byte*)malloc(vertexCount * 4 * sizeof(byte));
		weights = (float*)malloc(vertexCount * 4 * sizeof(float));
		indices = (ushort*)malloc(indexCount * sizeof(ushort));

		for (uint i = 0; i < (uint)vertexCount; i++) {
			SetVec3(anim->aVertices[i], vertices, i);
			SetVec3(anim->aNormals[i], normals, i);
			if (anim->aTangents.size() > 0) 
				SetVec3(anim->aTangents[i], tangents, i);

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
			SetVec4(anim->aWeights[i], weights, i);
		}
		for (uint i = 0; i < (uint)indexCount; i++)
			indices[i] = (ushort)(anim->aIndices[i]);

		animation = anim;
	}
	~AnimationData() {
		releaseAnimData();
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
};

#endif

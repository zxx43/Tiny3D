#ifndef ANIMATION_DATA_H_
#define ANIMATION_DATA_H_

#include "../animation/animation.h"
#include "../constants/constants.h"

struct AnimationData {
	float* vertices;
	float* normals;
	float* texcoords;
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
		texcoords = (float*)malloc(vertexCount * 4 * sizeof(float));
		colors = (byte*)malloc(vertexCount * 3 * sizeof(byte));
		boneids = (byte*)malloc(vertexCount * 4 * sizeof(byte));
		weights = (float*)malloc(vertexCount * 4 * sizeof(float));
		indices = (ushort*)malloc(indexCount * sizeof(ushort));

		for (uint i = 0; i < (uint)vertexCount; i++) {
			vertices[i * 3 + 0] = anim->aVertices[i].x;
			vertices[i * 3 + 1] = anim->aVertices[i].y;
			vertices[i * 3 + 2] = anim->aVertices[i].z;

			normals[i * 3 + 0] = anim->aNormals[i].x;
			normals[i * 3 + 1] = anim->aNormals[i].y;
			normals[i * 3 + 2] = anim->aNormals[i].z;

			texcoords[i * 4 + 0] = anim->aTexcoords[i].x;
			texcoords[i * 4 + 1] = anim->aTexcoords[i].y;
			texcoords[i * 4 + 2] = anim->aTextures[i]->texOfs1.x;
			texcoords[i * 4 + 3] = anim->aTextures[i]->texOfs1.y;

			colors[i * 3 + 0] = (byte)(anim->aAmbients[i].x * 255);
			colors[i * 3 + 1] = (byte)(anim->aDiffuses[i].x * 255);
			colors[i * 3 + 2] = (byte)(anim->aSpeculars[i].x * 255);

			boneids[i * 4 + 0] = (byte)(anim->aBoneids[i].x);
			boneids[i * 4 + 1] = (byte)(anim->aBoneids[i].y);
			boneids[i * 4 + 2] = (byte)(anim->aBoneids[i].z);
			boneids[i * 4 + 3] = (byte)(anim->aBoneids[i].w);
			
			weights[i * 4 + 0] = anim->aWeights[i].x;
			weights[i * 4 + 1] = anim->aWeights[i].y;
			weights[i * 4 + 2] = anim->aWeights[i].z;
			weights[i * 4 + 3] = anim->aWeights[i].w;
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
		if (texcoords) free(texcoords); texcoords = NULL;
		if (colors) free(colors); colors = NULL;
		if (boneids) free(boneids); boneids = NULL;
		if (weights) free(weights); weights = NULL;
		if (indices) free(indices); indices = NULL;
	}
};

#endif

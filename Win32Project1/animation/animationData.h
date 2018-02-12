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
	int textureChannel;
	int indexCount, vertexCount, boneCount;
	Animation* animation;

	AnimationData(Animation* anim) {
		indexCount = anim->aIndices.size();
		vertexCount = anim->aVertices.size();
		boneCount = anim->boneCount;
		vertices = new float[vertexCount * 3];
		normals = new float[vertexCount * 3];
		texcoords = new float[vertexCount * 4];
		colors = new byte[vertexCount * 3];
		boneids = new byte[vertexCount * 4];
		weights = new float[vertexCount * 4];
		indices = new ushort[indexCount];

		for (uint i = 0; i < vertexCount; i++) {
			vertices[i * 3] = anim->aVertices[i].x;
			vertices[i * 3 + 1] = anim->aVertices[i].y;
			vertices[i * 3 + 2] = anim->aVertices[i].z;
		}
		for (uint i = 0; i<vertexCount; i++) {
			normals[i * 3] = anim->aNormals[i].x;
			normals[i * 3 + 1] = anim->aNormals[i].y;
			normals[i * 3 + 2] = anim->aNormals[i].z;
		}
		for (uint i = 0; i<vertexCount; i++) {
			VECTOR2D texcoord = anim->aTexcoords[i];
			VECTOR4D texids = anim->aTextures[i];
			textureChannel = texids.y >= 0 ? 4 : 3;
			texcoords[i * textureChannel] = texcoord.x;
			texcoords[i * textureChannel + 1] = texcoord.y;
			texcoords[i * textureChannel + 2] = texids.x;
			if (textureChannel == 4)
				texcoords[i * textureChannel + 3] = texids.y;
		}
		for (uint i = 0; i < vertexCount; i++) {
			colors[i * 3] = (byte)(anim->aAmbients[i].x * 255);
			colors[i * 3 + 1] = (byte)(anim->aDiffuses[i].x * 255);
			colors[i * 3 + 2] = (byte)(anim->aSpeculars[i].x * 255);
		}
		for (uint i = 0; i < vertexCount; i++) {
			boneids[i * 4] = (byte)(anim->aBoneids[i].x);
			boneids[i * 4 + 1] = (byte)(anim->aBoneids[i].y);
			boneids[i * 4 + 2] = (byte)(anim->aBoneids[i].z);
			boneids[i * 4 + 3] = (byte)(anim->aBoneids[i].w);
		}
		for (uint i = 0; i < vertexCount; i++) {
			weights[i * 4] = anim->aWeights[i].x;
			weights[i * 4 + 1] = anim->aWeights[i].y;
			weights[i * 4 + 2] = anim->aWeights[i].z;
			weights[i * 4 + 3] = anim->aWeights[i].w;
		}
		for (uint i = 0; i < indexCount; i++)
			indices[i] = (ushort)(anim->aIndices[i]);

		animation = anim;
	}
	~AnimationData() {
		delete[] vertices; vertices = NULL;
		delete[] normals; normals = NULL;
		delete[] texcoords; texcoords = NULL;
		delete[] colors; colors = NULL;
		delete[] boneids; boneids = NULL;
		delete[] weights; weights = NULL;
		delete[] indices; indices = NULL;
	}
};

#endif
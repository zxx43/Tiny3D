#include "animationDrawcall.h"
#include "../constants/constants.h"

AnimationDrawcall::AnimationDrawcall() {}

AnimationDrawcall::AnimationDrawcall(Animation* anim) :Drawcall() {
	objectCount = 1;

	animation = anim;
	vertices = new float[anim->aVertices.size() * 3];
	normals = new float[anim->aNormals.size() * 3];
	texcoords = new float[anim->aTexcoords.size() * 4];
	colors = new byte[anim->aAmbients.size() * 3];
	boneids = new byte[anim->aBoneids.size() * 4];
	weights = new float[anim->aWeights.size() * 4];
	indices = new ushort[anim->aIndices.size()];
	indexCount = anim->aIndices.size();
	boneCount = anim->boneCount;

	for (uint i = 0; i < anim->aVertices.size(); i++) {
		vertices[i * 3] = anim->aVertices[i].x;
		vertices[i * 3 + 1] = anim->aVertices[i].y;
		vertices[i * 3 + 2] = anim->aVertices[i].z;
	}
	for (uint i = 0; i<anim->aNormals.size(); i++) {
		normals[i * 3] = anim->aNormals[i].x;
		normals[i * 3 + 1] = anim->aNormals[i].y;
		normals[i * 3 + 2] = anim->aNormals[i].z;
	}
	for (uint i = 0; i<anim->aTexcoords.size(); i++) {
		VECTOR2D texcoord = anim->aTexcoords[i];
		VECTOR4D texids = anim->aTextures[i];
		textureChannel = texids.y >= 0 ? 4 : 3;
		texcoords[i * textureChannel] = texcoord.x;
		texcoords[i * textureChannel + 1] = texcoord.y;
		texcoords[i * textureChannel + 2] = texids.x;
		if (textureChannel == 4)
			texcoords[i * textureChannel + 3] = texids.y;
	}
	for (uint i = 0; i < anim->aAmbients.size(); i++) {
		colors[i * 3] = (byte)(anim->aAmbients[i].x * 255);
		colors[i * 3 + 1] = (byte)(anim->aDiffuses[i].x * 255);
		colors[i * 3 + 2] = (byte)(anim->aSpeculars[i].x * 255);
	}
	for (uint i = 0; i < anim->aBoneids.size(); i++) {
		boneids[i * 4] = (byte)(anim->aBoneids[i].x);
		boneids[i * 4 + 1] = (byte)(anim->aBoneids[i].y);
		boneids[i * 4 + 2] = (byte)(anim->aBoneids[i].z);
		boneids[i * 4 + 3] = (byte)(anim->aBoneids[i].w);
	}
	for (uint i = 0; i < anim->aWeights.size(); i++) {
		weights[i * 4] = anim->aWeights[i].x;
		weights[i * 4 + 1] = anim->aWeights[i].y;
		weights[i * 4 + 2] = anim->aWeights[i].z;
		weights[i * 4 + 3] = anim->aWeights[i].w;
	}
	for (uint i = 0; i < anim->aIndices.size(); i++)
		indices[i] = (ushort)(anim->aIndices[i]);

	dataBuffer = new RenderBuffer(7);
	dataBuffer->pushData(0, new RenderData(VERTEX_LOCATION, GL_FLOAT, anim->aVertices.size(), 3, 1,
		dataBuffer->vbos[0], false, GL_STATIC_DRAW, -1, vertices));
	dataBuffer->pushData(1, new RenderData(NORMAL_LOCATION, GL_FLOAT, anim->aNormals.size(), 3, 1,
		dataBuffer->vbos[1], false, GL_STATIC_DRAW, -1, normals));
	dataBuffer->pushData(2, new RenderData(TEXCOORD_LOCATION, GL_FLOAT, anim->aTexcoords.size(), textureChannel, 1,
		dataBuffer->vbos[2], false, GL_STATIC_DRAW, -1, texcoords));
	dataBuffer->pushData(3, new RenderData(COLOR_LOCATION, GL_UNSIGNED_BYTE, anim->aAmbients.size(), 3, 1,
		dataBuffer->vbos[3], false, GL_STATIC_DRAW, -1, colors));
	dataBuffer->pushData(4, new RenderData(BONEIDS_LOCATION, GL_UNSIGNED_BYTE, anim->aBoneids.size(), 4, 1,
		dataBuffer->vbos[4], false, GL_STATIC_DRAW, -1, boneids));
	dataBuffer->pushData(5, new RenderData(WEIGHTS_LOCATION, GL_FLOAT, anim->aWeights.size(), 4, 1,
		dataBuffer->vbos[5], false, GL_STATIC_DRAW, -1, weights));
	dataBuffer->pushData(6, new RenderData(GL_UNSIGNED_SHORT, anim->aIndices.size(),
		dataBuffer->vbos[6], GL_STATIC_DRAW, indices));

	createSimple();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setType(ANIMATE_DC);
}

AnimationDrawcall::~AnimationDrawcall() {
	releaseSimple();
	delete[] vertices; vertices=NULL;
	delete[] normals; normals=NULL;
	delete[] texcoords; texcoords=NULL;
	delete[] colors; colors = NULL;
	delete[] boneids; boneids=NULL;
	delete[] weights; weights=NULL;
	delete[] indices; indices=NULL;
	delete dataBuffer;
}

void AnimationDrawcall::createSimple() {
	simpleBuffer = new RenderBuffer(4);
	simpleBuffer->pushData(0, new RenderData(0, GL_FLOAT, animation->aVertices.size(), 3, 1,
		simpleBuffer->vbos[0], false, GL_STATIC_DRAW, -1, vertices));
	simpleBuffer->pushData(1, new RenderData(1, GL_UNSIGNED_BYTE, animation->aBoneids.size(), 4, 1,
		simpleBuffer->vbos[1], false, GL_STATIC_DRAW, -1, boneids));
	simpleBuffer->pushData(2, new RenderData(2, GL_FLOAT, animation->aWeights.size(), 4, 1,
		simpleBuffer->vbos[2], false, GL_STATIC_DRAW, -1, weights));
	simpleBuffer->pushData(3, new RenderData(GL_UNSIGNED_SHORT, animation->aIndices.size(),
		simpleBuffer->vbos[3], GL_STATIC_DRAW, indices));
}

void AnimationDrawcall::releaseSimple() {
	delete simpleBuffer;
}

void AnimationDrawcall::draw(Shader* shader,bool simple) {
	if (uModelMatrix)
		shader->setMatrix4("uModelMatrix", uModelMatrix);
	if (uNormalMatrix && !simple) 
		shader->setMatrix3("uNormalMatrix", uNormalMatrix);
	if (animation->boneTransformMats)
		shader->setMatrix3x4("boneMats", boneCount, animation->boneTransformMats);
	if (!simple)
		dataBuffer->use();
	else
		simpleBuffer->use();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);
}

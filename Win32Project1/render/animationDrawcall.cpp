#include "animationDrawcall.h"
#include "../constants/constants.h"

AnimationDrawcall::AnimationDrawcall() {}

AnimationDrawcall::AnimationDrawcall(Animation* anim) :Drawcall() {
	vbos = new GLuint[8];

	animation = anim;
	vertices = new float[anim->aVertices.size() * 3];
	normals = new float[anim->aNormals.size() * 3];
	texcoords = new byte[anim->aTexcoords.size() * 2];
	textureids = new short[anim->aTextures.size() * 4];
	colors = new byte[anim->aAmbients.size() * 3];
	boneids = new unsigned short[anim->aBoneids.size() * 4];
	weights = new float[anim->aWeights.size() * 4];
	indices = new uint[anim->aIndices.size()];
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
		if (texcoord.x>1) texcoord.x = texcoord.x - (int)texcoord.x;
		if (texcoord.y>1) texcoord.y = texcoord.y - (int)texcoord.y;
		texcoords[i * 2] = (byte)(texcoord.x * 255);
		texcoords[i * 2 + 1] = (byte)(texcoord.y * 255);
	}
	for (uint i = 0; i < anim->aTextures.size(); i++) {
		textureChannel = anim->aTextures[i].y >= 0 ? 4 : 1;
		textureids[i * textureChannel] = (short)anim->aTextures[i].x;
		if (textureChannel == 4) {
			textureids[i * 4 + 1] = (short)anim->aTextures[i].y;
			textureids[i * 4 + 2] = (short)anim->aTextures[i].z;
			textureids[i * 4 + 3] = (short)anim->aTextures[i].w;
		}
	}
	for (uint i = 0; i < anim->aAmbients.size(); i++) {
		colors[i * 3] = (byte)(anim->aAmbients[i].x * 255);
		colors[i * 3 + 1] = (byte)(anim->aDiffuses[i].x * 255);
		colors[i * 3 + 2] = (byte)(anim->aSpeculars[i].x * 255);
	}
	for (uint i = 0; i < anim->aBoneids.size(); i++) {
		boneids[i * 4] = (unsigned short)(anim->aBoneids[i].x);
		boneids[i * 4 + 1] = (unsigned short)(anim->aBoneids[i].y);
		boneids[i * 4 + 2] = (unsigned short)(anim->aBoneids[i].z);
		boneids[i * 4 + 3] = (unsigned short)(anim->aBoneids[i].w);
	}
	for (uint i = 0; i < anim->aWeights.size(); i++) {
		weights[i * 4] = anim->aWeights[i].x;
		weights[i * 4 + 1] = anim->aWeights[i].y;
		weights[i * 4 + 2] = anim->aWeights[i].z;
		weights[i * 4 + 3] = anim->aWeights[i].w;
	}
	for (uint i = 0; i < anim->aIndices.size(); i++)
		indices[i] = anim->aIndices[i];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(8, vbos);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[VERTEX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aVertices.size() * 3 * sizeof(float),
		vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aNormals.size() * 3 * sizeof(float),
		normals, GL_STATIC_DRAW);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(NORMAL_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[TEXCOORD_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aTexcoords.size() * 2 * sizeof(byte),
		texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(TEXCOORD_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[TEXTUREID_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aTextures.size()*textureChannel*sizeof(short),
		textureids, GL_STATIC_DRAW);
	glVertexAttribPointer(TEXTURE_LOCATION, textureChannel, GL_SHORT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(TEXTURE_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[COLOR_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aAmbients.size() * 3 * sizeof(byte),
		colors, GL_STATIC_DRAW);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(COLOR_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[BONEID_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aBoneids.size() * 4 * sizeof(unsigned short),
		boneids, GL_STATIC_DRAW);
	glVertexAttribPointer(BONEIDS_LOCATION, 4, GL_UNSIGNED_SHORT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(BONEIDS_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[WEIGHT_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aWeights.size() * 4 * sizeof(float),
		weights, GL_STATIC_DRAW);
	glVertexAttribPointer(WEIGHTS_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(WEIGHTS_LOCATION);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[INDEX_VBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, anim->aIndices.size()*sizeof(uint),
		indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

AnimationDrawcall::~AnimationDrawcall() {
	delete[] vertices; vertices=NULL;
	delete[] normals; normals=NULL;
	delete[] texcoords; texcoords=NULL;
	delete[] textureids; textureids = NULL;
	delete[] colors; colors = NULL;
	delete[] boneids; boneids=NULL;
	delete[] weights; weights=NULL;
	delete[] indices; indices=NULL;
	glDeleteBuffers(8,vbos);
	delete[] vbos;
	vbos=NULL;
	glDeleteVertexArrays(1,&vao);
}

void AnimationDrawcall::draw(Shader* shader) {
	if (uModelMatrix)
		shader->setMatrix4("uModelMatrix", uModelMatrix);
	if (uNormalMatrix) 
		shader->setMatrix3("uNormalMatrix", uNormalMatrix);
	if (animation->boneTransformMats)
		shader->setMatrix3x4("boneMats", boneCount, animation->boneTransformMats);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

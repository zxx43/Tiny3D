#include "animationDrawcall.h"
#include "../constants/constants.h"

AnimationDrawcall::AnimationDrawcall() {}

AnimationDrawcall::AnimationDrawcall(Animation* anim) :Drawcall() {
	vbos = new GLuint[7];

	animation = anim;
	vertices = new float[anim->aVertices.size() * 3];
	normals = new float[anim->aNormals.size() * 3];
	texcoords = new float[anim->aTexcoords.size() * 4];
	colors = new byte[anim->aAmbients.size() * 3];
	boneids = new ushort[anim->aBoneids.size() * 4];
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
		VECTOR4D texids = anim->aTextures[i];
		texcoords[i * 4] = texcoord.x;
		texcoords[i * 4 + 1] = texcoord.y;
		texcoords[i * 4 + 2] = texids.x;
		texcoords[i * 4 + 3] = texids.y;
		textureChannel = texids.y >= 0 ? 2 : 1;
	}
	for (uint i = 0; i < anim->aAmbients.size(); i++) {
		colors[i * 3] = (byte)(anim->aAmbients[i].x * 255);
		colors[i * 3 + 1] = (byte)(anim->aDiffuses[i].x * 255);
		colors[i * 3 + 2] = (byte)(anim->aSpeculars[i].x * 255);
	}
	for (uint i = 0; i < anim->aBoneids.size(); i++) {
		boneids[i * 4] = (ushort)(anim->aBoneids[i].x);
		boneids[i * 4 + 1] = (ushort)(anim->aBoneids[i].y);
		boneids[i * 4 + 2] = (ushort)(anim->aBoneids[i].z);
		boneids[i * 4 + 3] = (ushort)(anim->aBoneids[i].w);
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
	glGenBuffers(7, vbos);

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
	glBufferData(GL_ARRAY_BUFFER, anim->aTexcoords.size() * 4 * sizeof(float),
		texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(TEXCOORD_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(TEXCOORD_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[COLOR_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aAmbients.size() * 3 * sizeof(byte),
		colors, GL_STATIC_DRAW);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(COLOR_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[BONEID_VBO]);
	glBufferData(GL_ARRAY_BUFFER, anim->aBoneids.size() * 4 * sizeof(ushort),
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

	createSimple();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	glDeleteBuffers(7,vbos);
	delete[] vbos; vbos=NULL;
	glDeleteVertexArrays(1,&vao);
}

void AnimationDrawcall::createSimple() {
	vboSimple = new GLuint[4];
	glGenVertexArrays(1, &vaoSimple);
	glBindVertexArray(vaoSimple);
	glGenBuffers(4, vboSimple);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[0]);
	glBufferData(GL_ARRAY_BUFFER, animation->aVertices.size() * 3 * sizeof(float),
		vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[1]);
	glBufferData(GL_ARRAY_BUFFER, animation->aBoneids.size() * 4 * sizeof(uint),
		boneids, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_SHORT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[2]);
	glBufferData(GL_ARRAY_BUFFER, animation->aWeights.size() * 4 * sizeof(float),
		weights, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSimple[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, animation->aIndices.size()*sizeof(uint),
		indices, GL_STATIC_DRAW);
}

void AnimationDrawcall::releaseSimple() {
	glDeleteBuffers(4, vboSimple);
	delete[] vboSimple; vboSimple = NULL;
	glDeleteVertexArrays(1, &vaoSimple);
}

void AnimationDrawcall::draw(Shader* shader,bool simple) {
	if (uModelMatrix)
		shader->setMatrix4("uModelMatrix", uModelMatrix);
	if (uNormalMatrix && !simple) 
		shader->setMatrix3("uNormalMatrix", uNormalMatrix);
	if (animation->boneTransformMats)
		shader->setMatrix3x4("boneMats", boneCount, animation->boneTransformMats);
	if (!simple)
		glBindVertexArray(vao);
	else
		glBindVertexArray(vaoSimple);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

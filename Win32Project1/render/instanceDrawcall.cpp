#include "instanceDrawcall.h"

InstanceDrawcall::InstanceDrawcall(Instance* instance) :Drawcall() {
	vertexCount = instance->vertexCount;
	indexCount = instance->indexCount;
	indexed = indexCount > 0 ? true : false;
	instanceCount = instance->instanceCount;
	this->instance = instance;

	vbos = new GLuint[8];
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(8, vbos);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[VERTEX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat),
		instance->vertexBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(VERTEX_LOCATION, 0);
	glEnableVertexAttribArray(VERTEX_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat),
		instance->normalBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(NORMAL_LOCATION, 0);
	glEnableVertexAttribArray(NORMAL_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[TEXCOORD_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(GLfloat),
		instance->texcoordBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(TEXCOORD_LOCATION, 0);
	glEnableVertexAttribArray(TEXCOORD_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[COLOR_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLubyte),
		instance->colorBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
	glVertexAttribDivisor(COLOR_LOCATION, 0);
	glEnableVertexAttribArray(COLOR_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[TEXTUREID_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * instance->textureChannel * sizeof(short),
		instance->textureIdBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(TEXTURE_LOCATION, instance->textureChannel, GL_SHORT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(TEXTURE_LOCATION, 0);
	glEnableVertexAttribArray(TEXTURE_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[MODEL_MATRIX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 12 * sizeof(GLfloat),
		instance->modelMatrices, GL_DYNAMIC_DRAW);
	for (int i = 0; i < 3; i++) {
		glVertexAttribPointer(MODEL_MATRIX_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 12,
			(void*)(sizeof(float)*i * 4));
		glVertexAttribDivisor(MODEL_MATRIX_LOCATION + i, 1);
		glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + i);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_MATRIX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 9 * sizeof(GLfloat),
		instance->normalMatrices, GL_DYNAMIC_DRAW);
	for (int i = 0; i < 3; i++) {
		glVertexAttribPointer(NORMAL_MATRIX_LOCATION + i, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 9,
			(void*)(sizeof(float)*i * 3));
		glVertexAttribDivisor(NORMAL_MATRIX_LOCATION + i, 1);
		glEnableVertexAttribArray(NORMAL_MATRIX_LOCATION + i);
	}

	if (indexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[INDEX_VBO]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(GLuint),
			instance->indexBuffer, GL_STATIC_DRAW);
	}

	createSimple();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

InstanceDrawcall::~InstanceDrawcall() {
	releaseSimple();
	glDeleteBuffers(8,vbos);
	delete[] vbos; vbos=NULL;
	glDeleteVertexArrays(1,&vao);
}

void InstanceDrawcall::createSimple() {
	vboSimple = new GLuint[5];
	glGenVertexArrays(1, &vaoSimple);
	glBindVertexArray(vaoSimple);
	glGenBuffers(5, vboSimple);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat),
		instance->vertexBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(GLfloat),
		instance->texcoordBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(1, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[2]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * instance->textureChannel * sizeof(short),
		instance->textureIdBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(2, instance->textureChannel, GL_SHORT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(2, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[3]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 12 * sizeof(GLfloat),
		instance->modelMatrices, GL_DYNAMIC_DRAW);
	for (int i = 0; i < 3; i++) {
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 12,
			(void*)(sizeof(float)*i * 4));
		glVertexAttribDivisor(3 + i, 1);
		glEnableVertexAttribArray(3 + i);
	}

	if (indexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSimple[4]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(GLuint),
			instance->indexBuffer, GL_STATIC_DRAW);
	}
}

void InstanceDrawcall::releaseSimple() {
	glDeleteBuffers(5, vboSimple);
	delete[] vboSimple; vboSimple = NULL;
	glDeleteVertexArrays(1, &vaoSimple);
}

void InstanceDrawcall::draw(Shader* shader,bool simple) {
	if (!simple)
		glBindVertexArray(vao);
	else
		glBindVertexArray(vaoSimple);
	if(!indexed)
		glDrawArraysInstanced(GL_TRIANGLES,0,vertexCount,instanceCount);
	else
		glDrawElementsInstanced(GL_TRIANGLES,indexCount,GL_UNSIGNED_INT,0,instanceCount);
}

void InstanceDrawcall::updateMatrices(Instance* instance, bool updateNormals) {
	glBindBuffer(GL_ARRAY_BUFFER, vbos[MODEL_MATRIX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 12 * sizeof(GLfloat),
		instance->modelMatrices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[3]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 12 * sizeof(GLfloat),
		instance->modelMatrices, GL_DYNAMIC_DRAW);

	if (updateNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_MATRIX_VBO]);
		glBufferData(GL_ARRAY_BUFFER, instanceCount * 9 * sizeof(GLfloat),
			instance->normalMatrices, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

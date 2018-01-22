#include "instanceDrawcall.h"

InstanceDrawcall::InstanceDrawcall(Instance* instance) :Drawcall() {
	vertexCount = instance->vertexCount;
	indexCount = instance->indexCount;
	indexed = indexCount > 0 ? true : false;
	instanceCount = instance->instanceCount;
	this->instance = instance;

	vbos = new GLuint[7];
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(7, vbos);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[VERTEX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float),
		instance->vertexBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(VERTEX_LOCATION, 0);
	glEnableVertexAttribArray(VERTEX_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float),
		instance->normalBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(NORMAL_LOCATION, 0);
	glEnableVertexAttribArray(NORMAL_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[TEXCOORD_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 4 * sizeof(float),
		instance->texcoordBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(TEXCOORD_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(TEXCOORD_LOCATION, 0);
	glEnableVertexAttribArray(TEXCOORD_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[COLOR_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(byte),
		instance->colorBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
	glVertexAttribDivisor(COLOR_LOCATION, 0);
	glEnableVertexAttribArray(COLOR_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[MODEL_MATRIX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 12 * sizeof(float),
		instance->modelMatrices, GL_DYNAMIC_DRAW);
	for (int i = 0; i < 3; i++) {
		glVertexAttribPointer(MODEL_MATRIX_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 12,
			(void*)(sizeof(float)*i * 4));
		glVertexAttribDivisor(MODEL_MATRIX_LOCATION + i, 1);
		glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + i);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_MATRIX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 9 * sizeof(float),
		instance->normalMatrices, GL_DYNAMIC_DRAW);
	for (int i = 0; i < 3; i++) {
		glVertexAttribPointer(NORMAL_MATRIX_LOCATION + i, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 9,
			(void*)(sizeof(float)*i * 3));
		glVertexAttribDivisor(NORMAL_MATRIX_LOCATION + i, 1);
		glEnableVertexAttribArray(NORMAL_MATRIX_LOCATION + i);
	}

	if (indexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[INDEX_VBO]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(uint),
			instance->indexBuffer, GL_STATIC_DRAW);
	}

	createSimple();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

InstanceDrawcall::~InstanceDrawcall() {
	releaseSimple();
	glDeleteBuffers(7,vbos);
	delete[] vbos; vbos=NULL;
	glDeleteVertexArrays(1,&vao);
}

void InstanceDrawcall::createSimple() {
	vboSimple = new GLuint[4];
	glGenVertexArrays(1, &vaoSimple);
	glBindVertexArray(vaoSimple);
	glGenBuffers(4, vboSimple);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float),
		instance->vertexBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 4 * sizeof(float),
		instance->texcoordBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(1, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[2]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 12 * sizeof(float),
		instance->modelMatrices, GL_DYNAMIC_DRAW);
	for (int i = 0; i < 3; i++) {
		glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 12,
			(void*)(sizeof(float)*i * 4));
		glVertexAttribDivisor(2 + i, 1);
		glEnableVertexAttribArray(2 + i);
	}

	if (indexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSimple[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(uint),
			instance->indexBuffer, GL_STATIC_DRAW);
	}
}

void InstanceDrawcall::releaseSimple() {
	glDeleteBuffers(4, vboSimple);
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
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 12 * sizeof(float),
		instance->modelMatrices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[2]);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * 12 * sizeof(float),
		instance->modelMatrices, GL_DYNAMIC_DRAW);

	if (updateNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_MATRIX_VBO]);
		glBufferData(GL_ARRAY_BUFFER, instanceCount * 9 * sizeof(float),
			instance->normalMatrices, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#include "staticDrawcall.h"

StaticDrawcall::StaticDrawcall() {}

StaticDrawcall::StaticDrawcall(Batch* batch) :Drawcall() {
	vbos = new GLuint[7];
	vertexCount = batch->vertexCount;
	indexCount = batch->indexCount;
	indexed = indexCount > 0 ? true : false;
	this->batch = batch;
	setFullStatic(batch->fullStatic);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(7, vbos);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[VERTEX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float),
		batch->vertexBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float),
		batch->normalBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(NORMAL_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[TEXCOORD_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * batch->textureChannel * sizeof(float),
		batch->texcoordBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(TEXCOORD_LOCATION, batch->textureChannel, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(TEXCOORD_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[COLOR_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(byte),
		batch->colorBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(COLOR_LOCATION);

	int indexVBO = INDEX_VBO;
	if (isFullStatic())
		indexVBO -= 2;
	else {
		glBindBuffer(GL_ARRAY_BUFFER, vbos[MODEL_MATRIX_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * 12 * sizeof(float),
			batch->modelMatrices, GL_DYNAMIC_DRAW);
		for (int i = 0; i < 3; i++) {
			glVertexAttribPointer(MODEL_MATRIX_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 12,
				(void*)(sizeof(float)*i * 4));
			glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + i);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_MATRIX_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * 9 * sizeof(float),
			batch->normalMatrices, GL_DYNAMIC_DRAW);
		for (int i = 0; i < 3; i++) {
			glVertexAttribPointer(NORMAL_MATRIX_LOCATION + i, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 9,
				(void*)(sizeof(float)*i * 3));
			glEnableVertexAttribArray(NORMAL_MATRIX_LOCATION + i);
		}
	}

	if (indexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[indexVBO]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(uint),
			batch->indexBuffer, GL_STATIC_DRAW);
	}

	createSimple();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

StaticDrawcall::~StaticDrawcall() {
	releaseSimple();
	glDeleteBuffers(7,vbos);
	delete[] vbos; vbos=NULL;
	glDeleteVertexArrays(1,&vao);
}

void StaticDrawcall::createSimple() {
	vboSimple = new GLuint[4];

	glGenVertexArrays(1, &vaoSimple);
	glBindVertexArray(vaoSimple);
	glGenBuffers(4, vboSimple);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float),
		batch->vertexBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * batch->textureChannel * sizeof(float),
		batch->texcoordBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(1, batch->textureChannel, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	int indexVBO = 3;
	if (isFullStatic())
		indexVBO = 2;
	else {
		glBindBuffer(GL_ARRAY_BUFFER, vboSimple[2]);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * 12 * sizeof(float),
			batch->modelMatrices, GL_DYNAMIC_DRAW);
		for (int i = 0; i < 3; i++) {
			glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 12,
				(void*)(sizeof(float)*i * 4));
			glEnableVertexAttribArray(2 + i);
		}
	}

	if (indexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSimple[indexVBO]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(uint),
			batch->indexBuffer, GL_STATIC_DRAW);
	}
}

void StaticDrawcall::releaseSimple() {
	glDeleteBuffers(4, vboSimple);
	delete[] vboSimple; vboSimple = NULL;
	glDeleteVertexArrays(1, &vaoSimple);
}

void StaticDrawcall::draw(Shader* shader,bool simple) {
	if (!simple)
		glBindVertexArray(vao);
	else
		glBindVertexArray(vaoSimple);
	if(!indexed)
		glDrawArrays(GL_TRIANGLES,0,vertexCount);
	else
		glDrawElements(GL_TRIANGLES,indexCount,GL_UNSIGNED_INT,0);
}

void StaticDrawcall::updateMatrices(Batch* batch, bool updateNormals) {
	glBindBuffer(GL_ARRAY_BUFFER, vbos[MODEL_MATRIX_VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 12 * sizeof(float),
		batch->modelMatrices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboSimple[2]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 12 * sizeof(float),
		batch->modelMatrices, GL_DYNAMIC_DRAW);

	if (updateNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos[NORMAL_MATRIX_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * 9 * sizeof(float),
			batch->normalMatrices, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

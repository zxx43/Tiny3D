#ifndef RENDER_BUFFER_H_
#define RENDER_BUFFER_H_

#include "glheader.h"
#include "../constants/constants.h"

struct RenderData {
	uint bitSize;
	uint dataSize, channelCount, rowCount;
	GLuint bufferid;
	GLenum drawType;
	void* streamData;
	RenderData(uint loc, GLenum type, uint count, uint channel, uint row, GLuint vbo, bool normalize, GLenum draw, int divisor, void* data) {
		switch (type) {
		case GL_FLOAT:
			bitSize = sizeof(float);
			break;
		case GL_INT:
			bitSize = sizeof(int);
			break;
		case GL_UNSIGNED_INT:
			bitSize = sizeof(uint);
			break;
		case GL_UNSIGNED_SHORT:
			bitSize = sizeof(ushort);
			break;
		case GL_UNSIGNED_BYTE:
			bitSize = sizeof(byte);
			break;
		}
		channelCount = channel;
		rowCount = row;
		dataSize = count * channelCount * rowCount;
		bufferid = vbo;
		drawType = draw;
		streamData = data;

		glBindBuffer(GL_ARRAY_BUFFER, bufferid);
		glBufferData(GL_ARRAY_BUFFER, dataSize * bitSize, streamData, drawType);
		for (int i = 0; i < row; i++) {
			uint attrloc = loc + i;
			glVertexAttribPointer(attrloc, channel, type, normalize, bitSize * row * channel,
				(void*)(bitSize * i * channel));
			if (divisor >= 0)
				glVertexAttribDivisor(attrloc, divisor);
			glEnableVertexAttribArray(attrloc);
		}
	}
	RenderData(GLenum type, uint size, GLuint vbo, GLenum draw, void* data) {
		switch (type) {
		case GL_UNSIGNED_INT:
			bitSize = sizeof(uint);
			break;
		case GL_UNSIGNED_SHORT:
			bitSize = sizeof(ushort);
			break;
		case GL_UNSIGNED_BYTE:
			bitSize = sizeof(byte);
			break;
		}
		dataSize = size;
		bufferid = vbo;
		drawType = draw;
		streamData = data;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferid);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize * bitSize, streamData, drawType);
	}
	void updateAttrBuf(uint count, void* data, GLenum draw) {
		dataSize = count * channelCount * rowCount;
		drawType = draw;
		streamData = data;
		glBindBuffer(GL_ARRAY_BUFFER, bufferid);
		glBufferData(GL_ARRAY_BUFFER, dataSize * bitSize, streamData, drawType);
	}
};

struct RenderBuffer {
	GLuint vao;
	GLuint* vbos;
	RenderData** streamDatas;
	uint bufferSize;
	RenderBuffer(uint size) {
		bufferSize = size;
		vbos = new GLuint[bufferSize];
		streamDatas = new RenderData*[bufferSize];
		for (uint i = 0; i < bufferSize; i++)
			pushData(i, NULL);
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(bufferSize, vbos);
	}
	~RenderBuffer() {
		glDeleteBuffers(bufferSize, vbos);
		glDeleteVertexArrays(1, &vao);
		for (uint i = 0; i < bufferSize; i++) {
			if (streamDatas[i])
				delete streamDatas[i];
		}
		delete[] streamDatas;
		delete[] vbos;
	}
	void pushData(uint i, RenderData* data) {
		streamDatas[i] = data;
	}
	void use() {
		glBindVertexArray(vao);
	}
};

#endif
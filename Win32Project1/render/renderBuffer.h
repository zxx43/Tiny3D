#ifndef RENDER_BUFFER_H_
#define RENDER_BUFFER_H_

#include "glheader.h"
#include "../constants/constants.h"

const std::map<GLenum, uint> TypeSize = {
	std::map<GLenum, uint>::value_type(GL_FLOAT, sizeof(GLfloat)),
	std::map<GLenum, uint>::value_type(GL_INT, sizeof(GLint)),
	std::map<GLenum, uint>::value_type(GL_UNSIGNED_INT, sizeof(GLuint)),
	std::map<GLenum, uint>::value_type(GL_UNSIGNED_SHORT, sizeof(GLushort)),
	std::map<GLenum, uint>::value_type(GL_UNSIGNED_BYTE, sizeof(GLubyte)),
	std::map<GLenum, uint>::value_type(GL_DOUBLE, sizeof(GLdouble))
};

struct RenderData {
	uint bitSize;
	uint dataSize, channelCount, rowCount;
	GLuint bufferid;
	GLenum drawType;
	void* streamData;
	void* gpuPtr;

	uint locid;
	bool norm;
	int div;
	GLenum dataType;

	void createAttribute() {
		int stride = rowCount > 1 ? bitSize * rowCount * channelCount : 0;
		for (uint i = 0; i < rowCount; i++) {
			uint attrloc = locid + i;
			glVertexAttribPointer(attrloc, channelCount, dataType, norm, stride, (void*)(bitSize * i * channelCount));
			if (div >= 0) glVertexAttribDivisor(attrloc, div);
			glEnableVertexAttribArray(attrloc);
		}
	}

	RenderData(uint loc, GLenum type, uint count, uint channel, uint row, GLuint vbo, bool normalize, GLenum draw, int divisor, void* data) {
		bitSize = TypeSize.find(type)->second;
		channelCount = channel;
		rowCount = row;
		dataSize = count * channelCount * rowCount;
		bufferid = vbo;
		drawType = draw;
		streamData = data;

		locid = loc;
		norm = normalize;
		div = divisor;
		dataType = type;
		/*
		glBindBuffer(GL_ARRAY_BUFFER, bufferid);
		glBufferStorage(GL_ARRAY_BUFFER, dataSize * bitSize, streamData, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		if (drawType != GL_STATIC_DRAW)
			gpuPtr = glMapNamedBufferRange(bufferid, 0, dataSize * bitSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		//*/
		///*
		glBindBuffer(GL_ARRAY_BUFFER, bufferid);
		glBufferData(GL_ARRAY_BUFFER, dataSize * bitSize, streamData, drawType);
		//glBufferStorage(GL_ARRAY_BUFFER, dataSize * bitSize, streamData, GL_MAP_WRITE_BIT);
		//*/
		createAttribute();
	}
	RenderData(GLenum type, uint size, GLuint vbo, GLenum draw, void* data) {
		bitSize = TypeSize.find(type)->second;
		dataSize = size;
		bufferid = vbo;
		drawType = draw;
		streamData = data;
		/*
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferid);
		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, dataSize * bitSize, streamData, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		if (drawType != GL_STATIC_DRAW)
			gpuPtr = glMapNamedBufferRange(bufferid, 0, dataSize * bitSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		//*/
		///*
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferid);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize * bitSize, streamData, drawType);
		//glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, dataSize * bitSize, streamData, GL_MAP_WRITE_BIT);
		//*/
	}
	void updateAttrBuf(uint count, void* data) {
		dataSize = count * channelCount * rowCount;
		streamData = data;
		glNamedBufferSubData(bufferid, 0, dataSize * bitSize, streamData);
		//gpuPtr = glMapNamedBufferRange(bufferid, 0, dataSize * bitSize, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		//memcpy(gpuPtr, streamData, dataSize * bitSize);
		//glUnmapNamedBuffer(bufferid);
	}
	void updateIndexBuf(uint count, void* data) {
		dataSize = count;
		streamData = data;
		glNamedBufferSubData(bufferid, 0, dataSize * bitSize, streamData);
		//gpuPtr = glMapNamedBufferRange(bufferid, 0, dataSize * bitSize, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		//memcpy(gpuPtr, streamData, dataSize * bitSize);
		//glUnmapNamedBuffer(bufferid);
	}
};

struct RenderBuffer {
	GLuint vao;
	GLuint* vbos;
	RenderData** streamDatas;
	bool* relies;
	uint bufferSize;
	RenderBuffer(uint size) {
		bufferSize = size;
		vbos = new GLuint[bufferSize];
		streamDatas = new RenderData*[bufferSize];
		relies = (bool*)malloc(bufferSize * sizeof(bool));
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
			if (streamDatas[i] && !relies[i])
				delete streamDatas[i];
		}
		free(relies);
		delete[] streamDatas;
		delete[] vbos;
	}
	void pushData(uint i, RenderData* data) {
		streamDatas[i] = data;
		relies[i] = false;
	}
	void setAttribData(uint loc, GLenum type, uint count, uint channel, uint row, bool normalize, GLenum draw, int divisor, void* data) {
		if (streamDatas[loc]) delete streamDatas[loc];
		streamDatas[loc] = new RenderData(loc, type, count, channel, row, vbos[loc], normalize, draw, divisor, data);
	}
	void setAttribData(uint loc, RenderData* data) {
		streamDatas[loc] = data;
		relies[loc] = true;
		glBindBuffer(GL_ARRAY_BUFFER, data->bufferid);
		data->createAttribute();
	}
	void setIndexData(uint ind, GLenum type, uint size, GLenum draw, void* data) {
		if (streamDatas[ind]) delete streamDatas[ind];
		streamDatas[ind] = new RenderData(type, size, vbos[ind], draw, data);
	}
	void setIndexData(uint ind, RenderData* data) {
		streamDatas[ind] = data;
		relies[ind] = true;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->bufferid);
	}
	void updateAttribData(uint loc, uint count, void* data) {
		streamDatas[loc]->updateAttrBuf(count, data);
	}
	void updateIndexData(uint loc, uint count, void* data) {
		streamDatas[loc]->updateIndexBuf(count, data);
	}
	void unuseAttr() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void use() {
		glBindVertexArray(vao);
	}
	void unuse() {
		glBindVertexArray(0);
	}
};

#endif
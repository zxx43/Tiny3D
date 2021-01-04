#ifndef RENDER_BUFFER_H_
#define RENDER_BUFFER_H_

#include "glheader.h"
#include "../constants/constants.h"

const std::map<GLenum, uint> TypeSize = {
	std::map<GLenum, uint>::value_type(GL_FLOAT, sizeof(GLfloat)),
	std::map<GLenum, uint>::value_type(GL_HALF_FLOAT, sizeof(GLhalf)),
	std::map<GLenum, uint>::value_type(GL_INT, sizeof(GLint)),
	std::map<GLenum, uint>::value_type(GL_UNSIGNED_INT, sizeof(GLuint)),
	std::map<GLenum, uint>::value_type(GL_UNSIGNED_SHORT, sizeof(GLushort)),
	std::map<GLenum, uint>::value_type(GL_UNSIGNED_BYTE, sizeof(GLubyte)),
	std::map<GLenum, uint>::value_type(GL_DOUBLE, sizeof(GLdouble)),
	std::map<GLenum, uint>::value_type(GL_BYTE, sizeof(GLbyte)),
	std::map<GLenum, uint>::value_type(GL_ONE, 1)
};

struct RenderData {
	uint bitSize;
	uint dataSize, channelCount, rowCount;
	GLuint bufferid;
	GLenum drawType;
	void* streamData;

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

	RenderData(GLenum target, uint loc, GLenum type, uint count, uint channel, uint row, GLuint vbo, bool normalize, GLenum draw, int divisor, void* data) {
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

		glBindBuffer(target, bufferid);
		glBufferData(target, dataSize * bitSize, streamData, drawType);

		if (target == GL_ARRAY_BUFFER) createAttribute();
	}
	RenderData(GLenum target, GLenum type, uint size, GLuint vbo, GLenum draw, void* data) {
		bitSize = TypeSize.find(type)->second;
		channelCount = 1;
		rowCount = 1;
		dataSize = size;
		bufferid = vbo;
		drawType = draw;
		streamData = data;

		glBindBuffer(target, bufferid);
		glBufferData(target, dataSize * bitSize, streamData, drawType);
	}
	RenderData(GLenum target, GLenum type, uint size, uint channel, GLuint vbo, GLenum draw, void* data) {
		bitSize = TypeSize.find(type)->second;
		channelCount = channel;
		rowCount = 1;
		dataSize = size * channelCount;
		bufferid = vbo;
		drawType = draw;
		streamData = data;

		glBindBuffer(target, bufferid);
		glBufferData(target, dataSize * bitSize, streamData, drawType);
	}
	void useAs(GLenum target) {
		glBindBuffer(target, bufferid);
	}
	void setShaderBase(int base) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, base, bufferid);
	}
	void unbindShaderBase(int base) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, base, 0);
	}
	void updateBuffer(uint count, void* data) {
		dataSize = count * channelCount * rowCount;
		streamData = data;
		glNamedBufferSubData(bufferid, 0, dataSize * bitSize, streamData);
	}
	void updateBufferMap(GLenum target, uint count, void* data) {
		int mapSize = count * channelCount * rowCount;
		glBindBuffer(target, bufferid);
		void* ptr = glMapBufferRange(target, 0, mapSize * bitSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		if (ptr) {
			memcpy(ptr, data, mapSize * bitSize);
			glUnmapBuffer(target);
		}
	}
	void* getMapBuffer(uint count) {
		int mapSize = count * channelCount * rowCount;
		return glMapNamedBufferRange(bufferid, 0, mapSize * bitSize, GL_MAP_WRITE_BIT);
	}
	void endMapBuffer() {
		glUnmapNamedBuffer(bufferid);
	}
	void readBufferData(GLenum target, uint count, void* ret) {
		int mapSize = count * channelCount * rowCount;
		glBindBuffer(target, bufferid);
		void* ptr = glMapBufferRange(target, 0, mapSize * bitSize, GL_MAP_READ_BIT);
		if (ptr) {
			memcpy(ret, ptr, mapSize * bitSize);
			glUnmapBuffer(target);
		}
	}
};

struct RenderBuffer {
	bool useVao;
	GLuint vao;
	GLuint* vbos;
	RenderData** streamDatas;
	bool* relies;
	uint bufferSize;
	RenderBuffer(uint size, bool uVao = true) {
		bufferSize = size;
		vbos = new GLuint[bufferSize];
		streamDatas = new RenderData*[bufferSize];
		relies = (bool*)malloc(bufferSize * sizeof(bool));
		for (uint i = 0; i < bufferSize; i++)
			pushData(i, NULL);

		useVao = uVao;
		if (useVao) {
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
		}
		glGenBuffers(bufferSize, vbos);
	}
	~RenderBuffer() {
		glDeleteBuffers(bufferSize, vbos);
		if(useVao) glDeleteVertexArrays(1, &vao);
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
	void setAttribData(GLenum target, uint loc, uint attrid, GLenum type, uint count, uint channel, uint row, bool normalize, GLenum draw, int divisor, void* data) {
		if (streamDatas[loc]) delete streamDatas[loc];
		streamDatas[loc] = new RenderData(target, attrid, type, count, channel, row, vbos[loc], normalize, draw, divisor, data);
	}
	void setAttribData(GLenum target, uint loc, RenderData* data) {
		streamDatas[loc] = data;
		relies[loc] = true;
		glBindBuffer(target, data->bufferid);
		if(target == GL_ARRAY_BUFFER)
			data->createAttribute();
	}
	void setBufferData(GLenum target, uint ind, GLenum type, uint size, GLenum draw, void* data) {
		if (streamDatas[ind]) delete streamDatas[ind];
		streamDatas[ind] = new RenderData(target, type, size, vbos[ind], draw, data);
	}
	void setBufferData(GLenum target, uint ind, GLenum type, uint size, uint channel, GLenum draw, void* data) {
		if (streamDatas[ind]) delete streamDatas[ind];
		streamDatas[ind] = new RenderData(target, type, size, channel, vbos[ind], draw, data);
	}
	void setBufferData(GLenum target, uint ind, RenderData* data) {
		streamDatas[ind] = data;
		relies[ind] = true;
		glBindBuffer(target, data->bufferid);
	}
	void useAs(uint ind, GLenum target) {
		streamDatas[ind]->useAs(target);
	}
	void unuseAs(GLenum target) {
		glBindBuffer(target, 0);
	}
	void setAttrib(uint ind) {
		streamDatas[ind]->createAttribute();
	}
	void setShaderBase(uint ind, int base) {
		streamDatas[ind]->setShaderBase(base);
	}
	void unbindShaderBase(uint ind, int base) {
		streamDatas[ind]->unbindShaderBase(base);
	}
	void updateBufferData(uint loc, uint count, void* data) {
		streamDatas[loc]->updateBuffer(count, data);
	}
	void updateBufferMap(GLenum target, uint loc, uint count, void* data) {
		streamDatas[loc]->updateBufferMap(target, count, data);
	}
	void* getBufferMap(uint count, uint loc) {
		return streamDatas[loc]->getMapBuffer(count);
	}
	void endBufferMap(uint loc) {
		streamDatas[loc]->endMapBuffer();
	}
	void readBufferData(GLenum target, uint loc, uint count, void* ret) {
		streamDatas[loc]->readBufferData(target, count, ret);
	}
	void use() {
		if(useVao) glBindVertexArray(vao);
	}
	void unuse() {
		glBindVertexArray(0);
	}
};

inline void UnbindShaderBuffers(GLuint first, GLsizei count​) {
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, first, count​, 0);
}

#endif
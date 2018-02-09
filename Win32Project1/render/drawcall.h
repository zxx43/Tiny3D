/*
 * drawcall.h
 *
 *  Created on: 2017-6-22
 *      Author: a
 */

#ifndef DRAWCALL_H_
#define DRAWCALL_H_

#define VERTEX_VBO 0
#define NORMAL_VBO 1
#define TEXCOORD_VBO 2
#define COLOR_VBO 3
#define OBJECTID_VBO 4
#define BONEID_VBO 4
#define WEIGHT_VBO 5
#define MODEL_MATRIX_VBO 4

#define ATTRIB_VERTEX "vertex"
#define ATTRIB_NORMAL "normal"
#define ATTRIB_TEXCOORD "texcoord"
#define ATTRIB_AMBIENT "ambient"
#define ATTRIB_DIFFUSE "diffuse"
#define ATTRIB_SPECULAR "specular"
#define ATTRIB_BONEIDS "boneids"
#define ATTRIB_WEIGHTS "weights"
#define ATTRIB_MODEL_MATRIX "modelMatrix"

#define VERTEX_LOCATION 0
#define NORMAL_LOCATION 1
#define TEXCOORD_LOCATION 2
#define COLOR_LOCATION 3
#define OBJECTID_LOCATION 4
#define BONEIDS_LOCATION 4
#define WEIGHTS_LOCATION 5
#define MODEL_MATRIX_LOCATION 4

#define NULL_DC 0
#define STATIC_DC 1
#define INSTANCE_DC 2
#define ANIMATE_DC 3

#include "../shader/shader.h"
#include "../maths/Maths.h"
#include "../shadow/shadow.h"

struct RenderState {
	bool enableCull;
	int cullMode;
	bool drawLine;
	bool enableDepthTest;
	int depthTestMode;
	bool enableAlphaTest;
	int alphaTestMode;
	float alphaThreshold;
	bool lightEffect; 
	bool shadowPass;
	Shadow* shadow;
	VECTOR3D light;
	Shader* shader;
	Shader* shaderIns;
	RenderState() {
		reset();
	}
	void reset() {
		enableCull = true;
		cullMode = CULL_BACK;
		drawLine = false;
		enableDepthTest = true;
		depthTestMode = LEQUAL;
		enableAlphaTest = false;
		alphaTestMode = GREATER;
		alphaThreshold = 0;
		lightEffect = true;
		shadowPass = false;
		shadow = NULL;
		light = VECTOR3D(0, 0, 0);
		shader = NULL;
		shaderIns = NULL;
	}
	void copyFrom(RenderState* src) {
		enableCull = src->enableCull;
		cullMode = src->cullMode;
		drawLine = src->drawLine;
		enableDepthTest = src->enableDepthTest;
		depthTestMode = src->depthTestMode;
		enableAlphaTest = src->enableAlphaTest;
		alphaTestMode = src->alphaTestMode;
		alphaThreshold = src->alphaThreshold;
		lightEffect = src->lightEffect;
		shadowPass = src->shadowPass;
		shadow = src->shadow;
		light = src->light;
		shader = src->shader;
		shaderIns = src->shaderIns;
	}
};

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

class Drawcall {
private:
	bool singleSide;
	int type;
	bool fullStatic;
public:
	float* uModelMatrix;
	float* uNormalMatrix;
	int objectCount;
	RenderBuffer* dataBuffer;
	RenderBuffer* simpleBuffer;

	Drawcall();
	virtual ~Drawcall();

	virtual void createSimple() = 0;
	virtual void releaseSimple() = 0;

	virtual void draw(Shader* shader,bool simple)=0;
	void setSide(bool single);
	bool isSingleSide();
	void setType(int typ);
	int getType();
	void setFullStatic(bool stat);
	bool isFullStatic();
};

#endif /* DRAWCALL_H_ */

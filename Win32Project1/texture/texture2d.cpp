#include "texture2d.h"
#include "../constants/constants.h"
#include <string.h>
#include <stdlib.h>

Texture2D::Texture2D(uint w,uint h,int t,int p,int c,int filter,bool clampBorder,void* initData) {
	width = w, height = h;
	type = t;
	precision = p;
	channel = c;
	if (type == TEXTURE_TYPE_DEPTH) channel = 1;
	else if (type == TEXTURE_TYPE_ANIME) channel = 4;

	glGenTextures(1,&id);
	glBindTexture(GL_TEXTURE_2D,id);

	GLint filterParam = filter == LINEAR ? GL_LINEAR : GL_NEAREST;
	if (type == TEXTURE_TYPE_ANIME) filterParam = GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
	
	if (clampBorder) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4] = { 1, 1, 1, 1 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	} 
	preDepth = precision >= HIGH_PRE ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16;
	if (precision == FLOAT_PRE) preDepth = GL_DEPTH_COMPONENT32F;
	format = GL_RGBA;
	
	preColor = precision >= HIGH_PRE ? GL_RGBA16 : GL_RGBA8;
	if (precision == FLOAT_PRE) preColor = GL_RGBA32F;

	if (channel == 1) {
		format = GL_RED;
		preColor = precision >= HIGH_PRE ? GL_R16 : GL_R8;
		if (precision == FLOAT_PRE) preColor = GL_R32F;
	} else if (channel == 2) {
		format = GL_RG;
		preColor = precision >= HIGH_PRE ? GL_RG16 : GL_RG8;
		if (precision == FLOAT_PRE) preColor = GL_RG32F;
	} else if (channel == 3) {
		format = GL_RGB;
		preColor = precision >= HIGH_PRE ? GL_RGB16 : GL_RGB8;
		if (precision == FLOAT_PRE) preColor = GL_RGB32F;
	}
	if (type == TEXTURE_TYPE_DEPTH) format = GL_DEPTH_COMPONENT;

	void* texData = NULL;
	texType = GL_UNSIGNED_BYTE;
	if (precision < FLOAT_PRE) {
		texData = malloc((width*height*channel)*sizeof(byte));
		memset(texData, 255, (width*height*channel)*sizeof(byte));
		texType = GL_UNSIGNED_BYTE;
	} else {
		texData = malloc((width*height*channel)*sizeof(float));
		memset(texData, 0, (width*height*channel)*sizeof(float));
		texType = GL_FLOAT;
	}
	depthType = precision > HIGH_PRE ? GL_FLOAT : GL_UNSIGNED_BYTE;

	GLenum dataType;
	if (type == TEXTURE_TYPE_COLOR || type == TEXTURE_TYPE_ANIME) dataType = texType;
	else if (type == TEXTURE_TYPE_DEPTH) dataType = depthType;
	if (dataType == GL_FLOAT) buffSize = width * height * channel * sizeof(GL_FLOAT);
	else if (dataType == GL_UNSIGNED_BYTE) buffSize = width * height * channel * sizeof(GL_UNSIGNED_BYTE);

	void* data = initData ? initData : texData;
	switch(type) {
		case TEXTURE_TYPE_COLOR:
		case TEXTURE_TYPE_ANIME:
			glTexImage2D(GL_TEXTURE_2D, 0, preColor, width, height, 0, format, texType, data);
			break;
		case TEXTURE_TYPE_DEPTH:
			glTexImage2D(GL_TEXTURE_2D, 0, preDepth, width, height, 0, format, depthType, 0);
			break;
	}

	if (texData) free(texData);
	glBindTexture(GL_TEXTURE_2D,0);

	hnd = genBindless();
}

Texture2D::~Texture2D() {
	releaseBindless(hnd);
	glDeleteTextures(1, &id);
}

void Texture2D::copyDataFrom(Texture2D* src) {
	glCopyImageSubData(src->id, GL_TEXTURE_2D, 0, 0, 0, 0,
		id, GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1);
}

void Texture2D::readData(int bitSize, void* ret) {
	GLenum readType;
	switch (type) {
		case TEXTURE_TYPE_COLOR:
		case TEXTURE_TYPE_ANIME:
			readType = texType;
			break;
		case TEXTURE_TYPE_DEPTH:
			readType = depthType;
			break;
	}
	int bufSize = width * height * channel * bitSize;
	glGetTextureSubImage(id, 0, 0, 0, 0, width, height, 1, format, readType, bufSize, ret);
}

u64 Texture2D::genBindless() {
	u64 texHnd = glGetTextureHandleARB(id);
	glMakeTextureHandleResidentARB(texHnd);
	return texHnd;
}

void Texture2D::releaseBindless(u64 texHnd) {
	glMakeTextureHandleNonResidentARB(texHnd);
}
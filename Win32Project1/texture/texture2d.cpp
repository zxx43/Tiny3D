#include "texture2d.h"
#include "../constants/constants.h"
#include <string.h>
#include <stdlib.h>

Texture2D::Texture2D(float w,float h,int t,int p,int c,bool clampBorder) {
	width = w, height = h;
	type = t;
	precision = p;
	channel = c;
	if (type == TEXTURE_TYPE_DEPTH) channel = 1;

	glGenTextures(1,&id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,id);
	GLint filterParam = precision >= HIGH_PRE ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
	if (clampBorder) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4] = { 1, 1, 1, 1 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	} else {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	preDepth = precision >= HIGH_PRE ? GL_DEPTH_COMPONENT32 : GL_DEPTH_COMPONENT24;
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
		texData = malloc((width*height*channel)*sizeof(unsigned char));
		memset(texData, 255, (width*height*channel)*sizeof(unsigned char));
		texType = GL_UNSIGNED_BYTE;
	} else {
		texData = malloc((width*height*channel)*sizeof(float));
		memset(texData, 0, (width*height*channel)*sizeof(float));
		texType = GL_FLOAT;
	}
	depthType = precision >= HIGH_PRE ? GL_FLOAT : GL_UNSIGNED_BYTE;

	GLenum dataType;
	if (type == TEXTURE_TYPE_COLOR) dataType = texType;
	else if (type == TEXTURE_TYPE_DEPTH) dataType = depthType;
	if (dataType == GL_FLOAT) buffSize = width * height * channel * sizeof(GL_FLOAT);
	else if (dataType == GL_UNSIGNED_BYTE) buffSize = width * height * channel * sizeof(GL_UNSIGNED_BYTE);

	switch(type) {
		case TEXTURE_TYPE_COLOR:
			glTexImage2D(GL_TEXTURE_2D, 0, preColor, width, height, 0, format, texType, texData);
			break;
		case TEXTURE_TYPE_DEPTH:
			glTexImage2D(GL_TEXTURE_2D, 0, preDepth, width, height, 0, format, depthType, 0);
			break;
	}

	if (texData) free(texData);
	glBindTexture(GL_TEXTURE_2D,0);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &id);
}

void Texture2D::copyDataFrom(Texture2D* src) {
	glCopyImageSubData(src->id, GL_TEXTURE_2D, 0, 0, 0, 0,
		id, GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1);
}

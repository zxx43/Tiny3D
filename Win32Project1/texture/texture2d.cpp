#include "texture2d.h"
#include "../util/util.h"
#include <string.h>
#include <stdlib.h>

Texture2D::Texture2D(uint w, uint h, bool useMip, int t, int p, int c, int filter, int wrapMode, bool clearWhite, void* initData): Texture(w, h, p, c) {
	type = t;

	if (type == TEXTURE_TYPE_DEPTH) channel = 1;
	if (type == TEXTURE_TYPE_ANIME) channel = 4;
	if (type == TEXTURE_TYPE_COLOR) channel = c;

	if (type == TEXTURE_TYPE_DEPTH && precision == HALF_PRE) precision = FLOAT_PRE;

	glBindTexture(GL_TEXTURE_2D, id);

	GLint filterParam = filter == LINEAR ? GL_LINEAR : GL_NEAREST;
	if (type == TEXTURE_TYPE_ANIME) filterParam = GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	if (wrapMode == WRAP_CLAMP_TO_BORDER) {
		const float White[4] = { 1, 1, 1, 1 };
		const float Black[4] = { 0, 0, 0, 0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clearWhite ? White : Black);
	}
	
	GLenum color1, color2, color3, color4, depth1;

	switch (precision) {
		case LOW_PRE:
			depth1 = GL_DEPTH_COMPONENT16;
			color1 = GL_R8;
			color2 = GL_RG8;
			color3 = GL_RGB8;
			color4 = GL_RGBA8;
			break;
		case HIGH_PRE:
			depth1 = GL_DEPTH_COMPONENT24;
			color1 = GL_R16;
			color2 = GL_RG16;
			color3 = GL_RGB16;
			color4 = GL_RGBA16;
			break;
		case HALF_PRE:
			color1 = GL_R16F;
			color2 = GL_RG16F;
			color3 = GL_RGB16F;
			color4 = GL_RGBA16F;
			break;
		case FLOAT_PRE:
			depth1 = GL_DEPTH_COMPONENT32F;
			color1 = GL_R32F;
			color2 = GL_RG32F;
			color3 = GL_RGB32F;
			color4 = GL_RGBA32F;
			break;
		default:
			depth1 = GL_DEPTH_COMPONENT24;
			color1 = GL_R16;
			color2 = GL_RG16;
			color3 = GL_RGB16;
			color4 = GL_RGBA16;
			break;
	}

	switch (channel) {
		case 1:
			if (type != TEXTURE_TYPE_DEPTH) {
				preColor = color1;
				format = GL_RED;
			} else {
				preDepth = depth1;
				format = GL_DEPTH_COMPONENT;
			}
			break;
		case 2:
			preColor = color2;
			format = GL_RG;
			break;
		case 3:
			preColor = color3;
			format = GL_RGB;
			break;
		case 4:
			preColor = color4;
			format = GL_RGBA;
			break;
	}

	switch (precision) {
		case LOW_PRE:
		case HIGH_PRE:
			texType = GL_UNSIGNED_BYTE;
			depthType = GL_UNSIGNED_BYTE;
			break;
		case HALF_PRE:
			texType = GL_HALF_FLOAT;
			break;
		case FLOAT_PRE:
			texType = GL_FLOAT;
			depthType = GL_FLOAT;
			break;
		default:
			texType = GL_UNSIGNED_BYTE;
			depthType = GL_UNSIGNED_BYTE;
			break;
	}

	void* data = initData ? initData : 0;
	if (data) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	switch (type) {
		case TEXTURE_TYPE_COLOR:
		case TEXTURE_TYPE_ANIME:
			glTexImage2D(GL_TEXTURE_2D, 0, preColor, width, height, 0, format, texType, data);
			break;
		case TEXTURE_TYPE_DEPTH:
			glTexImage2D(GL_TEXTURE_2D, 0, preDepth, width, height, 0, format, depthType, 0);
			break;
	}

	if (useMip) {
		GLint filterParam = filter == LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	hnd = genBindless();
}

Texture2D::~Texture2D() {
	releaseBindless(hnd);
}

u64 Texture2D::genBindless() {
	u64 texHnd = glGetTextureHandleARB(id);
	glMakeTextureHandleResidentARB(texHnd);
	return texHnd;
}

void Texture2D::releaseBindless(u64 texHnd) {
	glMakeTextureHandleNonResidentARB(texHnd);
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
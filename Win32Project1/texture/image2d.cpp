#include "image2d.h"
#include "../util/util.h"
#include <string.h>
#include <stdlib.h>

Image2D::Image2D(uint w, uint h, int p, int c, int layout, int filter, int wrapMode, void* initData) {
	width = w, height = h;
	precision = p;
	channel = c;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	GLint filterParam = filter == LINEAR ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	GLenum color1, color2, color3, color4;
	GLenum format1, format2, format3, format4;
	switch (precision) {
		case UINT_PRE:
			color1 = GL_R32UI;
			color2 = GL_RG32UI;
			color3 = GL_RGB32UI;
			color4 = GL_RGBA32UI;
			format1 = GL_RED_INTEGER;
			format2 = GL_RG_INTEGER;
			format3 = GL_RGB_INTEGER;
			format4 = GL_RGBA_INTEGER;
			break;
		case INT_PRE:
			color1 = GL_R32I;
			color2 = GL_RG32I;
			color3 = GL_RGB32I;
			color4 = GL_RGBA32I;
			format1 = GL_RED_INTEGER;
			format2 = GL_RG_INTEGER;
			format3 = GL_RGB_INTEGER;
			format4 = GL_RGBA_INTEGER;
			break;
		case FLOAT_PRE:
			color1 = GL_R32F;
			color2 = GL_RG32F;
			color3 = GL_RGB32F;
			color4 = GL_RGBA32F;
			format1 = GL_RED;
			format2 = GL_RG;
			format3 = GL_RGB;
			format4 = GL_RGBA;
			break;
	}

	switch (channel) {
		case 1:
			preColor = color1;
			format = format1;
			break;
		case 2:
			preColor = color2;
			format = format2;
			break;
		case 3:
			preColor = color3;
			format = format3;
			break;
		case 4:
			preColor = color4;
			format = format4;
			break;
	}

	switch (precision) {
		case UINT_PRE:
			texType = GL_UNSIGNED_INT;
			break;
		case INT_PRE:
			texType = GL_INT;
			break;
		case FLOAT_PRE:
			texType = GL_FLOAT;
			break;
	}

	void* data = initData ? initData : 0;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, preColor, width, height, 0, format, texType, data);
	setLayout(layout);

	glBindTexture(GL_TEXTURE_2D, 0);

	hnd = genBindless();
}

Image2D::~Image2D() {
	releaseBindless(hnd);
	glDeleteTextures(1, &id);
}

u64 Image2D::genBindless() {
	u64 texHnd = glGetTextureHandleARB(id);
	glMakeTextureHandleResidentARB(texHnd);
	return texHnd;
}

void Image2D::releaseBindless(u64 texHnd) {
	glMakeTextureHandleNonResidentARB(texHnd);
}

void Image2D::setLayout(int layout) {
	glBindImageTexture(layout, id, 0, GL_FALSE, 0, GL_READ_WRITE, preColor);
}

void Image2D::updateData(void* data) {
	glTextureSubImage2D(id, 0, 0, 0, width, height, format, texType, data);
}

void Image2D::readData(int bitSize, void* ret) {
	int bufSize = width * height * channel * bitSize;
	glGetTextureSubImage(id, 0, 0, 0, 0, width, height, 1, format, texType, bufSize, ret);
}
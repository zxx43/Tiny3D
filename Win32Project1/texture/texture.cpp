#include "texture.h"

Texture::Texture(uint w, uint h, int p, int c) {
	width = w, height = h;
	precision = p;
	channel = c;

	glGenTextures(1, &id);
}

Texture::~Texture() {
	releaseBindless(hnd);
	glDeleteTextures(1, &id);
}

u64 Texture::genBindless() {
	u64 texHnd = glGetTextureHandleARB(id);
	glMakeTextureHandleResidentARB(texHnd);
	return texHnd;
}

void Texture::releaseBindless(u64 texHnd) {
	glMakeTextureHandleNonResidentARB(texHnd);
}

void Texture::copyDataFrom(Texture* src) {
	glCopyImageSubData(src->id, GL_TEXTURE_2D, 0, 0, 0, 0,
		id, GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1);
}
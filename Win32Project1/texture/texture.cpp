#include "texture.h"

Texture::Texture(uint w, uint h, int p, int c) {
	width = w, height = h;
	precision = p;
	channel = c;

	glGenTextures(1, &id);
}

Texture::~Texture() {
	glDeleteTextures(1, &id);
}

void Texture::copyDataFrom(Texture* src) {
	glCopyImageSubData(src->id, GL_TEXTURE_2D, 0, 0, 0, 0,
		id, GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1);
}
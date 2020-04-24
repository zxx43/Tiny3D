#include "texturebindless.h"
#include "../render/render.h"
using namespace std;

TextureBindless::TextureBindless() {
	texinds.clear();
	texnames.clear();
	texSrgbs.clear();
	imgs.clear();
	texids = NULL;
	texhnds = NULL;
	size = 0;
}

TextureBindless::~TextureBindless() {
	for (int i = 0; i < size; i++)
		glMakeTextureHandleNonResidentARB(texhnds[i]);
	if (texhnds) free(texhnds); texhnds = NULL;

	if (texids) {
		glDeleteTextures(size, texids);
		free(texids); texids = NULL;
	}

	releaseMemory();
	
	texnames.clear();
	texSrgbs.clear();
	texinds.clear();
}

void TextureBindless::releaseMemory() {
	for (uint i = 0; i < imgs.size(); i++)
		delete imgs[i];
	imgs.clear();
}

void TextureBindless::addTexture(const char* name, bool srgb) {
	texnames.push_back(name);
	texSrgbs.push_back(srgb);
	texinds[name] = size;
	size++;
}

int TextureBindless::findTexture(const char* name) {
	map<string, int>::iterator it = texinds.find(name);
	if (it != texinds.end())
		return it->second;
	return -1;
}

void TextureBindless::initData(string dir) {
	string path = dir.append("/");

	texids = (GLuint*)malloc(size * sizeof(GLuint));
	memset(texids, 0, size * sizeof(GLuint));
	glGenTextures(size, texids);
	texhnds = (u64*)malloc(size * sizeof(u64));
	memset(texhnds, 0, size * sizeof(u64));

	for (int i = 0; i < size; i++) {
		BmpImage* img = new BmpImage((path + texnames[i]).data());
		imgs.push_back(img);
		glBindTexture(GL_TEXTURE_2D, texids[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Render::MaxAniso);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		GLint interFormat = GL_SRGB_ALPHA;
		if (texSrgbs[i])
			interFormat = GL_SRGB_ALPHA;
		else
			interFormat = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, interFormat, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
		glGenerateMipmap(GL_TEXTURE_2D);

		GLuint64 texHnd = glGetTextureHandleARB(texids[i]);
		glMakeTextureHandleResidentARB(texHnd);
		texhnds[i] = texHnd;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

#ifndef _DEBUG
	releaseMemory();
#endif
}
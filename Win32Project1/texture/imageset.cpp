#include "imageset.h"
#include "../constants/constants.h"
#include "../render/render.h"
using namespace std;

ImageSet::ImageSet() {
	setId=0;
	imageNames.clear();
	set.clear();
	images = NULL;
}

ImageSet::~ImageSet() {
	if (images) {
		for (uint i = 0; i < imageNames.size(); i++)
			delete images[i];
		delete[] images;
		images = NULL;
	}

	releaseTextureArray();
	set.clear();
	imageNames.clear();
}

void ImageSet::addTexture(const char* name) {
	set[name] = imageNames.size(); // Start at 0
	imageNames.push_back(name);
}

int ImageSet::findTexture(const char* name) {
	map<string,int>::iterator itor=set.find(name);
	if(itor!=set.end())
		return itor->second;
	return -1;
}

void ImageSet::initTextureArray(string dir) {
	if (imageNames.size() <= 0) return;
	images = new BmpImage*[imageNames.size()];

	glGenTextures(1,&setId);
	glBindTexture(GL_TEXTURE_2D_ARRAY,setId);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, Render::MaxAniso);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	string path=dir.append("/");
	string name("");
	for (unsigned int i = 0; i < imageNames.size(); i++) {
		name = imageNames[i];
		images[i] = new BmpImage((path + name).c_str());
		if (i == 0) {
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB_ALPHA,
				images[i]->width, images[i]->height,
				imageNames.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
			images[i]->width, images[i]->height,
			1, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->data);
	}

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	/*
	if(images){
		for (uint i = 0; i<imageNames.size(); i++)
			delete images[i];
		delete[] images;
		images = NULL;
	}
	//*/

	hnd = genBindless();
}

void ImageSet::releaseTextureArray() {
	if (setId > 0) {
		releaseBindless(hnd);
		glDeleteTextures(1, &setId);
	}
}

u64 ImageSet::genBindless() {
	u64 texHnd = glGetTextureHandleARB(setId);
	glMakeTextureHandleResidentARB(texHnd);
	return texHnd;
}

void ImageSet::releaseBindless(u64 texHnd) {
	glMakeTextureHandleNonResidentARB(texHnd);
}

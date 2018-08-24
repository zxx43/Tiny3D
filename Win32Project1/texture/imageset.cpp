#include "imageset.h"
using namespace std;

ImageSet::ImageSet() {
	setId=0;
	imageNames.clear();
	set.clear();
}

ImageSet::~ImageSet() {
	releaseTextureArray();
	set.clear();
	for (unsigned int i = 0; i<imageNames.size(); i++)
		delete images[i];
	delete[] images;
	images=NULL;
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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY,setId);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	string path=dir.append("/");
	string name("");
	for (unsigned int i = 0; i < imageNames.size(); i++) {
		name = imageNames[i];
		images[i] = new BmpImage((path + name).c_str());
		if (i == 0) {
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA,
				images[i]->width, images[i]->height,
				imageNames.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
			images[i]->width, images[i]->height,
			1, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->data);
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
}

void ImageSet::releaseTextureArray() {
	if(setId>0)
		glDeleteTextures(1,&setId);
}

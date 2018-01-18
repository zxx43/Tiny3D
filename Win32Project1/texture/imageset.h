/*
 * textureset.h
 *
 *  Created on: 2017-4-21
 *      Author: a
 */

#ifndef IMAGESET_H_
#define IMAGESET_H_

#include "../render/glheader.h"
#include "bmpimage.h"
#include <map>
#include <string>
#include <vector>

class ImageSet {
private:
	BmpImage** images;
	std::map<std::string,int> set;
	void releaseTextureArray();
public:
	GLuint setId;
	std::vector<std::string> imageNames;

	ImageSet();
	~ImageSet();
	void addTexture(const char* name);
	int findTexture(const char* name);
	void initTextureArray(std::string dir);
};


#endif /* IMAGESET_H_ */

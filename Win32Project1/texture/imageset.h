/*
 * textureset.h
 *
 *  Created on: 2017-4-21
 *      Author: a
 */

#ifndef IMAGESET_H_
#define IMAGESET_H_

#include "../render/glheader.h"
#include "../constants/constants.h"
#include "imageloader.h"
#include <map>
#include <string>
#include <vector>

class ImageSet {
private:
	ImageLoader** images;
	std::map<std::string,int> set;
	void releaseTextureArray();
public:
	GLuint setId;
	std::vector<std::string> imageNames;
	u64 hnd;
private:
	u64 genBindless();
	void releaseBindless(u64 texHnd);
public:
	ImageSet();
	~ImageSet();
	void addTexture(const std::string name);
	int findTexture(const std::string name);
	void initTextureArray(std::string dir);
};


#endif /* IMAGESET_H_ */

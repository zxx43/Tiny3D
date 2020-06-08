#ifndef TEXTURE_ATLAS_H_
#define TEXTURE_ATLAS_H_

#include "../render/glheader.h"
#include "imageloader.h"
#include <map>
#include <string>
#include <vector>

struct TexOffset {
	int x, y;
	TexOffset() {
		x = 0, y = 0;
	}
};

class TextureAtlas {
private:
	ImageLoader** images;
	std::vector<std::string> imageNames;
	unsigned char* data;
	std::map<std::string, TexOffset*> offsetMap;
	int pCountW, pCountH;
public:
	GLuint texId;
	int perImgWidth, perImgHeight;
	float pixW, pixH;
	float* atlasInfo;
private:
	void releaseAtlas();
public:
	TextureAtlas();
	~TextureAtlas();
	void addTexture(const char* name);
	TexOffset* findTextureOfs(const char* name);
	void createAtlas(std::string dir);
};

#endif
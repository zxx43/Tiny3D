#ifndef TEXTURE_BINDLESS_H_
#define TEXTURE_BINDLESS_H_

#include "../render/glheader.h"
#include "../constants/constants.h"
#include "imageloader.h"
#include <map>
#include <string>
#include <vector>

class TextureBindless {
private:
	std::map<std::string, int> texinds;
	GLuint* texids;
	u64* texhnds;
	std::vector<bool> texSrgbs;
	std::vector<std::string> texnames;
	std::vector<ImageLoader*> imgs;
	std::vector<int> wraps;
	int size;
private:
	void releaseMemory();
public:
	TextureBindless();
	~TextureBindless();
	void addTexture(const std::string name, bool srgb, int wrap = WRAP_REPEAT);
	int findTexture(const std::string name);
	void initData(std::string dir);
	int getSize() { return size; }
	GLuint64* getHnds() { return texhnds; }
};

#endif
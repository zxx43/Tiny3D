#include "textureatlas.h"
#include "../render/render.h"
#include "../constants/constants.h"
#include <stdlib.h>
using namespace std;

TextureAtlas::TextureAtlas() {
	texId = 0;
	perImgWidth = 0, perImgHeight = 0;
	pCountW = 0, pCountH = 0;
	pixW = 0, pixH = 0;
	atlasInfo = (float*)malloc(4 * sizeof(float));
	imageNames.clear();
	images = NULL;
	data = NULL;
	offsetMap.clear();
}

TextureAtlas::~TextureAtlas() {
	if (images) {
		for (uint i = 0; i < imageNames.size(); i++)
			delete images[i];
		delete[] images;
		images = NULL;
	}

	free(atlasInfo);
	releaseAtlas();
	imageNames.clear();

	map<string, TexOffset*>::iterator it = offsetMap.begin();
	while (it != offsetMap.end()) {
		delete it->second;
		++it;
	}
	offsetMap.clear();
}

void TextureAtlas::releaseAtlas() {
	if (texId) glDeleteTextures(1, &texId);
}

void TextureAtlas::addTexture(const char* name) {
	imageNames.push_back(name);
	offsetMap[name] = new TexOffset();
}

TexOffset* TextureAtlas::findTextureOfs(const char* name) {
	map<string, TexOffset*>::iterator it = offsetMap.find(name);
	if (it != offsetMap.end())
		return it->second;
	return NULL;
}

void TextureAtlas::createAtlas(string dir) {
	int imageCount = imageNames.size();
	if (imageCount <= 0) return;
	images = new BmpImage*[imageCount];

	string path = dir.append("/");
	string name("");
	for (unsigned int i = 0; i < imageCount; i++) {
		name = imageNames[i];
		images[i] = new BmpImage((path + name).c_str());
	}

	float sqrtCount = sqrtf((float)imageCount);
	pCountW = ceilf(sqrtCount);
	pCountH = ceilf(sqrtCount);

	perImgWidth = images[0]->width;
	perImgHeight = images[0]->height;

	uint bw = 32, bh = 32;

	uint atlasWidth = pCountW * (perImgWidth + 2 * bw);
	uint atlasHeight = pCountH * (perImgHeight + 2 * bh);
	pixW = 1.0 / (float)atlasWidth;
	pixH = 1.0 / (float)atlasHeight;

	atlasInfo[0] = pixW, atlasInfo[1] = pixH;
	atlasInfo[2] = perImgWidth, atlasInfo[3] = perImgHeight;

	uint pixelCount = atlasWidth * atlasHeight;
	data = (byte*)malloc(pixelCount * 4 * sizeof(byte));
	memset(data, 0, pixelCount * 4 * sizeof(byte));

	uint curPix = 0;
	for (uint pch = 0; pch < pCountH; pch++) {
		for (uint h = 0; h < perImgHeight + 2 * bh; h++) {
			for (uint pcw = 0; pcw < pCountW; pcw++) {
				uint imgIndex = pch * pCountW + pcw;
				for (uint w = 0; w < perImgWidth + 2 * bw; w++) {
					bool border = w<bw || w>(perImgWidth - 1 + bw) || h<bh || h>(perImgHeight - 1 + bh);
					if (!border) {
						uint pixIndex = (h - bh) * perImgWidth + (w - bw);
						if (imageCount - 1 >= imgIndex) {
							data[curPix * 4 + 0] = images[imgIndex]->data[pixIndex * 4 + 0];
							data[curPix * 4 + 1] = images[imgIndex]->data[pixIndex * 4 + 1];
							data[curPix * 4 + 2] = images[imgIndex]->data[pixIndex * 4 + 2];
							data[curPix * 4 + 3] = images[imgIndex]->data[pixIndex * 4 + 3];
						}
					} else {
						data[curPix * 4 + 0] = 0;
						data[curPix * 4 + 1] = 0;
						data[curPix * 4 + 2] = 0;
						data[curPix * 4 + 3] = 0;
					}
					curPix++;
				}
			}
		}
	}

	for (uint pch = 0; pch < pCountH; pch++) {
		for (uint pcw = 0; pcw < pCountW; pcw++) {
			uint imgIndex = pch * pCountW + pcw;
			if (imageCount - 1 >= imgIndex) {
				string imgName = imageNames[imgIndex];
				TexOffset* ofs = offsetMap[imgName];
				ofs->x = pcw * (2 * bw + perImgWidth) + bw;
				ofs->y = pch * (2 * bh + perImgHeight) + bh;
			}
		}
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Render::MaxAniso);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, atlasWidth, atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (data) free(data); data = NULL;
	/*
	if (images) {
		for (uint i = 0; i < imageCount; i++)
			delete images[i];
		delete[] images;
		images = NULL;
	}
	*/
	printf("img: %d,%d,%d\n", pCountW, pCountH, imageCount);
	map<string, TexOffset*>::iterator it = offsetMap.begin();
	while (it != offsetMap.end()) {
		printf("%s:%d,%d\n", it->first.data(), it->second->x, it->second->y);
		++it;
	}
}
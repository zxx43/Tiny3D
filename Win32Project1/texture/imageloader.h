#ifndef IMAGELOADER_H_
#define IMAGELOADER_H_

class ImageLoader {
public:
	int width, height;
	unsigned char* data;
public:
	ImageLoader(const char* path);
	~ImageLoader();
};

void InitImageLoaders();
void ReleaseImageLoaders();

#endif

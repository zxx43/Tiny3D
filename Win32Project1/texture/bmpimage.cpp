#include "bmpimage.h"
#include "../constants/constants.h"

BmpImage::BmpImage(const char* path) {
	loader = new BmpLoader();
	loader->loadBitmap(path);
	width = loader->getWidth();
	height = loader->getHeight();
	data = loader->data;
}

BmpImage::~BmpImage() {
	delete loader;
	loader=NULL;
}


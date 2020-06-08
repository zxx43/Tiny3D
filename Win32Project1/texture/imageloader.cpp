#include "imageloader.h"
#include <FreeImage.h>
#include <stdlib.h>
#include <string.h>
#include "../constants/constants.h"

void InitImageLoaders() {
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif
}

void ReleaseImageLoaders() {
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif
}

ImageLoader::ImageLoader(const char* path) {
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path, 0);
	FIBITMAP* dib = NULL;
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, path);
	else return;

	bool hasAlpha = FreeImage_GetBPP(dib) == 32;
	if (!hasAlpha) {
		FIBITMAP* tmpDib = dib;
		dib = FreeImage_ConvertTo32Bits(tmpDib);
		FreeImage_Unload(tmpDib);
	}

	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	byte* imgData = FreeImage_GetBits(dib);
	int imageSize = width * height;

	data = (byte*)malloc(imageSize * 4 * sizeof(byte));
	for (int i = 0; i < imageSize; i++) {
		data[i * 4 + 0] = imgData[i * 4 + 2];
		data[i * 4 + 1] = imgData[i * 4 + 1];
		data[i * 4 + 2] = imgData[i * 4 + 0];
		if (!hasAlpha) data[i * 4 + 3] = 255;
		else data[i * 4 + 3] = imgData[i * 4 + 3];
	}

	FreeImage_Unload(dib);
}

ImageLoader::~ImageLoader() {
	free(data); data = NULL;
}
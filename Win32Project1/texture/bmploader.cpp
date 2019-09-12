#include "bmploader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BmpLoader::BmpLoader() {
	header=(unsigned char*)malloc(54*sizeof(unsigned char));
	memset(header, 0, 54 * sizeof(unsigned char));
	data = NULL;
	tmp = NULL;
}

BmpLoader::~BmpLoader() {
	if (header) free(header); header = NULL;
	if (data) free(data); data = NULL;
	if (tmp) free(tmp); tmp = NULL;
}

bool BmpLoader::loadBitmap(const char* fileName) {
	FILE* file = fopen(fileName,"rb");
	if (!file) {
		printf("Image could not be opened %s\n", fileName);
		return false;
	}
	if (fread(header, 1, 54, file)!=54) {//文件头并非54字节 读取失败
	    printf("Not a correct BMP file\n");
		fclose(file);
	    return false;
	}
	if (header[0]!='B' || header[1]!='M') {//文件头开头并非BM 读取失败
	    printf("Not a correct BMP file\n");
		fclose(file);
	    return false;
	}

	dataPos    = *(int*)&(header[0x0A]);//读取位置 位置在文件头0x0A处
	imageSize  = *(int*)&(header[0x22]);//图片内容大小数据 位置在文件头0x22处
	width      = *(int*)&(header[0x12]);//图片宽度数据 位置在文件头0x12处
	height     = *(int*)&(header[0x16]);//图片高度数据 位置在文件头0x16处
	bit        = *(unsigned short*)&(header[0x1C]);//图片位数 位置在文件头0x1C处

	hasAlpha=bit==32?true:false; //32位?24位?

	int channelCount=hasAlpha?4:3;

	if (imageSize==0)
		imageSize=width*height*channelCount; //图片大小x颜色通道数
	if (dataPos==0)
		dataPos=54;//文件头读完 位置在54字节处

	tmp = (unsigned char*)malloc(imageSize * sizeof(unsigned char));//data放像素信息
	memset(tmp, 0, imageSize * sizeof(unsigned char));
	fread(tmp, 1, imageSize, file);//读取像素
	fclose(file);

	for(int i = 0;i<(int)imageSize;i+=channelCount)
		swapcolor(tmp[i],tmp[i+2]);//bgr变为rgb

	if (!hasAlpha) {
		imageSize = width * height * 4;
		bit = 32;
	}

	data = (unsigned char*)malloc(imageSize * sizeof(unsigned char));
	memset(data, 0, imageSize * sizeof(unsigned char));
	if (hasAlpha)
		memcpy(data, tmp, imageSize * sizeof(unsigned char));
	else {
		for (unsigned int p = 0; p < width * height; p++) {
			data[p * 4 + 0] = tmp[p * 3 + 0];
			data[p * 4 + 1] = tmp[p * 3 + 1];
			data[p * 4 + 2] = tmp[p * 3 + 2];
			data[p * 4 + 3] = 255;
		}
		hasAlpha = true;
	}

	printf("load image: %s OK!\n", fileName);
	return true;
}

int BmpLoader::getWidth() {
	return width;
}

int BmpLoader::getHeight() {
	return height;
}



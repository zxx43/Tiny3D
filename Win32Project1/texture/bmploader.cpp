#include "bmploader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BmpLoader::BmpLoader() {
	header=(unsigned char*)malloc(54*sizeof(unsigned char));
	memset(header, 0, 54 * sizeof(unsigned char));
	data = NULL;
}

BmpLoader::~BmpLoader() {
	if(header) free(header); header = NULL;
	if(data) free(data); data = NULL;
}

bool BmpLoader::loadBitmap(const char* fileName) {
	FILE* file = fopen(fileName,"rb");
	if (!file) {
		printf("Image could not be opened\n");
		return false;
	}
	if (fread(header, 1, 54, file)!=54) {//文件头并非54字节 读取失败
	    printf("Not a correct BMP file\n");
	    return false;
	}
	if (header[0]!='B' || header[1]!='M') {//文件头开头并非BM 读取失败
	    printf("Not a correct BMP file\n");
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

	data = (unsigned char*)malloc(imageSize*sizeof(unsigned char));//data放像素信息
	memset(data, 0, imageSize*sizeof(unsigned char));
	fread(data,1,imageSize,file);//读取像素
	fclose(file);

	for(int i = 0;i<(int)imageSize;i+=channelCount)
		swapcolor(data[i],data[i+2]);//bgr变为rgb
	
	if(!hasAlpha) {
		imageSize = width*height*4;
		bit=32;
		hasAlpha=true;
		unsigned char* tmp = (unsigned char*)malloc(imageSize*sizeof(unsigned char));
		memset(tmp, 0, imageSize*sizeof(unsigned char));
		for(unsigned int i=0;i<width*height;i++) {
			tmp[i * 4] = data[i * 3];
			tmp[i * 4 + 1] = data[i * 3 + 1];
			tmp[i * 4 + 2] = data[i * 3 + 2];
			tmp[i * 4 + 3] = 255;
		}

		free(data); data = NULL;
		data = tmp;
	}

	return true;
}

int BmpLoader::getWidth() {
	return width;
}

int BmpLoader::getHeight() {
	return height;
}



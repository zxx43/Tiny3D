#include "cubemap.h"
#include "../constants/constants.h"

CubeMap::CubeMap(const char* xpos,const char* xneg,const char* ypos,
		const char* yneg,const char* zpos,const char* zneg) {
	xposImg=new BmpImage(xpos);
	xnegImg=new BmpImage(xneg);
	yposImg=new BmpImage(ypos);
	ynegImg=new BmpImage(yneg);
	zposImg=new BmpImage(zpos);
	znegImg=new BmpImage(zneg);

	glGenTextures(1,&id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP,id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_SRGB_ALPHA,
		xposImg->width, xposImg->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, xposImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_SRGB_ALPHA,
		xnegImg->width, xnegImg->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, xnegImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_SRGB_ALPHA,
		yposImg->width, yposImg->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, yposImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_SRGB_ALPHA,
		ynegImg->width, ynegImg->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, ynegImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_SRGB_ALPHA,
		zposImg->width, zposImg->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, zposImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_SRGB_ALPHA,
		znegImg->width, znegImg->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, znegImg->data);

	glBindTexture(GL_TEXTURE_CUBE_MAP,0);

	delete xposImg; xposImg = NULL;
	delete xnegImg; xnegImg = NULL;
	delete yposImg; yposImg = NULL;
	delete ynegImg; ynegImg = NULL;
	delete zposImg; zposImg = NULL;
	delete znegImg; znegImg = NULL;
}

CubeMap::~CubeMap() {
	glDeleteTextures(1,&id);
}


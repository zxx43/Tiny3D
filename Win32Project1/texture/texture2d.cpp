#include "texture2d.h"

Texture2D::Texture2D(float w,float h,int t,int p,int c) {
	width=w, height=h;
	type=t;
	precision=p;

	glGenTextures(1,&id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,id);
	GLint filterParam = precision == HIGH_PRE ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	float borderColor[4]={1,1,1,1};
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
	GLint preDepth = precision == HIGH_PRE ? GL_DEPTH_COMPONENT32 : GL_DEPTH_COMPONENT24;
	GLint preColor = precision == HIGH_PRE ? GL_RGBA16 : GL_RGBA8;
	GLenum format = GL_RGBA;
	if (c == 1) {
		format = GL_RED;
		preColor = precision == HIGH_PRE ? GL_R16 : GL_R8;
	} else if (c == 2) {
		format = GL_RG;
		preColor = precision == HIGH_PRE ? GL_RG16 : GL_RG8;
	} else if (c == 3) {
		format = GL_RGB;
		preColor = precision == HIGH_PRE ? GL_RGB16 : GL_RGB8;
	}

	switch(type) {
		case TEXTURE_TYPE_COLOR:
			glTexImage2D(GL_TEXTURE_2D, 0, preColor, width, height, 0, format, GL_UNSIGNED_BYTE, 0);
			break;
		case TEXTURE_TYPE_DEPTH:
			glTexImage2D(GL_TEXTURE_2D, 0, preDepth, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			break;
	}
	glBindTexture(GL_TEXTURE_2D,0);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1,&id);
}


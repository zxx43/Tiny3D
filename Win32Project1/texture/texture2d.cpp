#include "texture2d.h"

Texture2D::Texture2D(float w,float h,int t) {
	width=w;
	height=h;
	type=t;

	glGenTextures(1,&id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	float borderColor[4]={1,1,1,1};
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
	switch(type) {
		case TEXTURE_TYPE_COLOR:
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,0);
			break;
		case TEXTURE_TYPE_DEPTH:
			glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT16,width,height,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);
			break;
	}
	glBindTexture(GL_TEXTURE_2D,0);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1,&id);
}

void Texture2D::use(int slot) {
	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(GL_TEXTURE_2D,id);
}


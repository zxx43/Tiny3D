#include "framebuffer.h"

FrameBuffer::FrameBuffer(float width, float height, int precision) {
	this->width=width;
	this->height=height;
	hasDepth=false;

	glGenFramebuffersEXT(1,&fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);

	colorBuffers.clear();
	colorBuffers.push_back(new Texture2D(width,height,TEXTURE_TYPE_COLOR,precision));
	colorBufferCount=1;
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,
			colorBuffers[0]->id,0);

	depthBuffer=NULL;

	glDrawBuffers(colorBufferCount,ColorAttachments);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
}

FrameBuffer::~FrameBuffer() {
	for(unsigned int i=0;i<colorBuffers.size();i++)
		delete colorBuffers[i];
	colorBuffers.clear();

	if(depthBuffer)
		delete depthBuffer;
	depthBuffer=NULL;

	glDeleteFramebuffersEXT(1,&fboId);
}

void FrameBuffer::attachDepthBuffer(int precision) {
	hasDepth = true;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
	
	depthBuffer = new Texture2D(width, height, TEXTURE_TYPE_DEPTH, precision);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBuffer->id, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void FrameBuffer::addColorBuffer(int precision) {
	colorBufferCount++;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);

	colorBuffers.push_back(new Texture2D(width,height,TEXTURE_TYPE_COLOR,precision));
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT+(colorBufferCount-1),GL_TEXTURE_2D,
			colorBuffers[colorBufferCount-1]->id,0);
	glDrawBuffers(colorBufferCount,ColorAttachments);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
}

Texture2D* FrameBuffer::getColorBuffer(int n) {
	if((int)colorBuffers.size()<n+1)
		return NULL;
	return colorBuffers[n];
}

void FrameBuffer::use() {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);
	GLbitfield clearMask=GL_COLOR_BUFFER_BIT;
	if(hasDepth)
		clearMask|=GL_DEPTH_BUFFER_BIT;
	glClear(clearMask);
	glViewport(0,0,width,height);
}

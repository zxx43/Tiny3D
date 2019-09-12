#include "framebuffer.h"

FrameBuffer::FrameBuffer(float width, float height, int precision, int component, bool clampBorder) {
	this->width = width;
	this->height = height;
	this->clampBorder = clampBorder;

	glGenFramebuffers(1, &fboId);

	colorBuffers.clear();
	colorBufferCount = 0;

	depthBuffer = NULL;
	depthOnly = false;
	addColorBuffer(precision, component);
}

FrameBuffer::FrameBuffer(float width, float height, int precision) {
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &fboId);

	colorBuffers.clear();
	colorBufferCount = 0;

	depthBuffer = NULL;
	depthOnly = true;
	attachDepthBuffer(precision);
}

FrameBuffer::~FrameBuffer() {
	for(unsigned int i=0;i<colorBuffers.size();i++)
		delete colorBuffers[i];
	colorBuffers.clear();

	if(depthBuffer)
		delete depthBuffer;
	depthBuffer=NULL;

	glDeleteFramebuffers(1,&fboId);
}

void FrameBuffer::attachDepthBuffer(int precision) {
	depthBuffer = new Texture2D(width, height, TEXTURE_TYPE_DEPTH, precision, 4);
	glNamedFramebufferTexture2DEXT(fboId, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer->id, 0);

	if (depthOnly) {
		glNamedFramebufferDrawBuffer(fboId, GL_NONE);
		glNamedFramebufferReadBuffer(fboId, GL_NONE);
	}
}

void FrameBuffer::addColorBuffer(int precision, int component) {
	colorBufferCount++;
	colorBuffers.push_back(new Texture2D(width,height,TEXTURE_TYPE_COLOR,precision,component,clampBorder));
	glNamedFramebufferTexture2DEXT(fboId,GL_COLOR_ATTACHMENT0+(colorBufferCount-1),GL_TEXTURE_2D,
			colorBuffers[colorBufferCount-1]->id,0);
	glNamedFramebufferDrawBuffers(fboId,colorBufferCount,ColorAttachments);
}

Texture2D* FrameBuffer::getColorBuffer(int n) {
	if((int)colorBuffers.size()<n+1)
		return NULL;
	return colorBuffers[n];
}

Texture2D* FrameBuffer::getDepthBuffer() {
	return depthBuffer;
}

void FrameBuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	GLbitfield clearMask = GL_COLOR_BUFFER_BIT;
	if (depthOnly)
		clearMask = GL_DEPTH_BUFFER_BIT;
	else if (depthBuffer)
		clearMask |= GL_DEPTH_BUFFER_BIT;
	glClear(clearMask);
	glViewport(0, 0, width, height);
}

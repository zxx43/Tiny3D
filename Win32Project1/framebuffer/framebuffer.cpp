#include "framebuffer.h"

FrameBuffer::FrameBuffer(float width, float height, int precision, int component, int wrap, int filt) :cubeBuffer(NULL) {
	this->width = width;
	this->height = height;
	this->wrapMode = wrap;

	glGenFramebuffers(1, &fboId);

	colorBuffers.clear();
	colorRefs.clear();

	depthBuffer = NULL;
	depthOnly = false;
	depthRef = false;
	setReadOnly(false);
	addColorBuffer(precision, component, filt);
}

FrameBuffer::FrameBuffer(float width, float height, int precision) :wrapMode(WRAP_CLAMP_TO_BORDER), cubeBuffer(NULL) {
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &fboId);

	colorBuffers.clear();
	colorRefs.clear();

	depthBuffer = NULL;
	depthOnly = true;
	depthRef = false;
	setReadOnly(false);
	attachDepthBuffer(precision, false);
}

FrameBuffer::FrameBuffer(float width, float height) :wrapMode(WRAP_CLAMP_TO_BORDER), cubeBuffer(NULL) {
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &fboId);

	colorBuffers.clear();
	colorRefs.clear();

	depthBuffer = NULL;
	depthOnly = true;
	depthRef = false;
	setReadOnly(false);
}

FrameBuffer::FrameBuffer(const CubeMap* cube) :wrapMode(WRAP_CLAMP_TO_EDGE) {
	glGenFramebuffers(1, &fboId);

	colorBuffers.clear();
	colorRefs.clear();

	depthBuffer = NULL;
	depthOnly = false;
	depthRef = false;
	setReadOnly(false);

	cubeBuffer = (CubeMap*)cube;
	width = (float)(cubeBuffer->getWidth());
	height = (float)(cubeBuffer->getHeight());
}

FrameBuffer::~FrameBuffer() {
	for (unsigned int i = 0; i < colorBuffers.size(); i++) {
		if (!colorRefs[i]) delete colorBuffers[i];
	}
	colorBuffers.clear();
	colorRefs.clear();

	if (depthBuffer && !depthRef) delete depthBuffer;
	depthBuffer = NULL;
	cubeBuffer = NULL;

	glDeleteFramebuffers(1, &fboId);
}

void FrameBuffer::attachDepthBuffer(int precision, bool useMip) {
	depthBuffer = new Texture2D((int)(width), (int)(height), useMip, TEXTURE_TYPE_DEPTH, precision, 4, NEAREST, WRAP_CLAMP_TO_BORDER, true, NULL);
	glNamedFramebufferTexture2DEXT(fboId, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer->id, 0);

	if (depthOnly) {
		glNamedFramebufferDrawBuffer(fboId, GL_NONE);
		glNamedFramebufferReadBuffer(fboId, GL_NONE);
	}
}

void FrameBuffer::setDepthBuffer(Texture* depthTex) {
	depthBuffer = depthTex;
	depthRef = true;
	glNamedFramebufferTexture2DEXT(fboId, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer->id, 0);
	if (depthOnly) {
		glNamedFramebufferDrawBuffer(fboId, GL_NONE);
		glNamedFramebufferReadBuffer(fboId, GL_NONE);
	}
}

void FrameBuffer::addColorRef(Texture* colorTex) {
	depthOnly = false;
	colorBuffers.push_back(colorTex);
	colorRefs.push_back(true);
	glNamedFramebufferTexture2DEXT(fboId, GL_COLOR_ATTACHMENT0 + (colorBuffers.size() - 1), GL_TEXTURE_2D,
		colorBuffers[colorBuffers.size() - 1]->id, 0);
	glNamedFramebufferDrawBuffers(fboId, colorBuffers.size(), ColorAttachments);
}

void FrameBuffer::addColorBuffer(int precision, int component, int filt) {
	depthOnly = false;
	colorBuffers.push_back(new Texture2D((int)(width), (int)(height), false, TEXTURE_TYPE_COLOR, precision, component, filt, wrapMode));
	colorRefs.push_back(false);
	glNamedFramebufferTexture2DEXT(fboId, GL_COLOR_ATTACHMENT0 + (colorBuffers.size() - 1), GL_TEXTURE_2D,
		colorBuffers[colorBuffers.size() - 1]->id, 0);
	glNamedFramebufferDrawBuffers(fboId, colorBuffers.size(), ColorAttachments);
}

Texture* FrameBuffer::getColorBuffer(int n) {
	if((int)colorBuffers.size()<n+1)
		return NULL;
	return colorBuffers[n];
}

Texture* FrameBuffer::getDepthBuffer() {
	return depthBuffer;
}

CubeMap* FrameBuffer::getCubeBuffer() {
	return cubeBuffer;
}

void FrameBuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	GLbitfield clearMask = GL_COLOR_BUFFER_BIT;
	if (depthOnly)
		clearMask = GL_DEPTH_BUFFER_BIT;
	else if (depthBuffer)
		clearMask |= GL_DEPTH_BUFFER_BIT;
	if (!readOnly) glClear(clearMask);
	glViewport(0, 0, width, height);
}

void FrameBuffer::useFbo() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
}

void FrameBuffer::useCube(int i, int mip) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeBuffer->id, mip);
	if (!readOnly) glClear(GL_COLOR_BUFFER_BIT);
	uint mipWidth = width, mipHeight = height;
	if (mip > 0) {
		mipWidth = width * std::pow(0.5, mip);
		mipHeight = height * std::pow(0.5, mip);
	}
	glViewport(0, 0, mipWidth, mipHeight);
}

/*
 * framebuffer.h
 *
 *  Created on: 2017-8-20
 *      Author: a
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "../render/glheader.h"
#include "../texture/texture2d.h"
#include <vector>

const GLenum ColorAttachments[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT , GL_COLOR_ATTACHMENT2_EXT,
		GL_COLOR_ATTACHMENT3_EXT, GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT,
		GL_COLOR_ATTACHMENT7_EXT};

class FrameBuffer {
private:
	unsigned int fboId;
	float width,height;
	int colorBufferCount;
public:
	std::vector<Texture2D*> colorBuffers;
	Texture2D* depthBuffer;
public:
	FrameBuffer(float width, float height, int precision, int component);
	~FrameBuffer();
public:
	void addColorBuffer(int precision, int component);
	void attachDepthBuffer(int precision);
	Texture2D* getColorBuffer(int n);
	void use();
};


#endif /* FRAMEBUFFER_H_ */

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
#include "../texture/cubemap.h"
#include <vector>

const GLenum ColorAttachments[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT , GL_COLOR_ATTACHMENT2_EXT,
		GL_COLOR_ATTACHMENT3_EXT, GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT,
		GL_COLOR_ATTACHMENT7_EXT};

class FrameBuffer {
private:
	float width, height;
	unsigned int fboId;
	int colorBufferCount;
	bool depthOnly;
	bool clampBorder;
public:
	std::vector<Texture2D*> colorBuffers;
	Texture2D* depthBuffer;
public:
	FrameBuffer(float width, float height, int precision, int component, bool clampBorder);
	FrameBuffer(float width, float height, int precision);
	~FrameBuffer();
public:
	void addColorBuffer(int precision, int component);
	void attachDepthBuffer(int precision);
	Texture2D* getColorBuffer(int n);
	Texture2D* getDepthBuffer();
	void use();
private:
	CubeMap* cubeBuffer;
public:
	FrameBuffer(const CubeMap* cube);
	CubeMap* getCubeBuffer();
	void useFbo();
	void useCube(int i);
};


#endif /* FRAMEBUFFER_H_ */

/*
 * filter.h
 *
 *  Created on: 2017-10-8
 *      Author: a
 */

#ifndef FILTER_H_
#define FILTER_H_

#include "../render/render.h"
#include "../node/staticNode.h"

class Filter {
private:
	float width,height,pixWidth,pixHeight;
	FrameBuffer* framebuffer;
	StaticNode* boardNode;
public:
	Filter(float width, float height, bool useFramebuffer, int precision, int component);
	~Filter();

	void draw(Camera* camera, Render* render, RenderState* state, 
		const std::vector<Texture2D*>& inputTextures, const Texture2D* depthTexture);
	Texture2D* getOutput();
};


#endif /* FILTER_H_ */

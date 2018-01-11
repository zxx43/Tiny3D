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
	float width,height;
	FrameBuffer* framebuffer;
	StaticNode* boardNode;
	RenderState* state;
public:
	Filter(float width,float height,bool useFramebuffer);
	~Filter();

	void draw(Render* render, Shader* shader, const std::vector<Texture2D*>& inputTextures);
	Texture2D* getOutput();
};


#endif /* FILTER_H_ */

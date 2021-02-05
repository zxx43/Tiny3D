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
private:
	bool bindTex(int slot, const Texture2D* tex, Shader* shader);
public:
	Filter(float width, float height, bool useFramebuffer, int precision, int component, int filt, bool clampBorder = true);
	~Filter();
public:
	void draw(Camera* camera, Render* render, RenderState* state,
		Texture2D* inputTexture, const Texture2D* depthTexture);
	void draw(Camera* camera, Render* render, RenderState* state, 
		const std::vector<Texture2D*>& inputTextures, const Texture2D* depthTexture);
	void addOutput(int precision, int component, int filt);
	void addDepthBuffer(int precision);
	FrameBuffer* getFrameBuffer();
	Texture2D* getOutput(int i);
};

struct FilterChain {
	std::vector<Texture2D*> input;
	Filter* output;
	FilterChain(int width, int height, bool useFramebuffer, int precision, int component, bool clampBorder = true) {
		input.clear(); 
		output = new Filter(width, height, useFramebuffer, precision, component, clampBorder);
	}
	~FilterChain() {
		input.clear();
		delete output;
	}
	void addInputTex(Texture2D* texIn) {
		input.push_back(texIn);
	}
	Texture2D* getOutputTex(int i) {
		return output->getOutput(i);
	}
	FrameBuffer* getOutFrameBuffer() {
		return output->getFrameBuffer();
	}
};

struct DualFilter {
	FilterChain *filter1, *filter2;
	DualFilter(int width, int height, bool useFramebuffer, int precision, int component, bool clampBorder = true) {
		filter1 = new FilterChain(width, height, useFramebuffer, precision, component, clampBorder);
		filter2 = new FilterChain(width, height, useFramebuffer, precision, component, clampBorder);
		filter2->addInputTex(filter1->getOutputTex(0));
	}
	~DualFilter() {
		delete filter1;
		delete filter2;
	}
	void addInputTex(Texture2D* tex) {
		filter1->addInputTex(tex);
	}
	std::vector<Texture2D*> getInput1() {
		return filter1->input;
	}
	Filter* getOutput1() {
		return filter1->output;
	}
	std::vector<Texture2D*> getInput2() {
		return filter2->input;
	}
	Filter* getOutput2() {
		return filter2->output;
	}
	Texture2D* getOutputTex() {
		return filter2->getOutputTex(0);
	}
};

#endif /* FILTER_H_ */

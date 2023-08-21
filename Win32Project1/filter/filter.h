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
	bool isDebug;
private:
	bool bindTex(int slot, const Texture* tex, Shader* shader);
public:
	Filter(float width, float height, bool useFramebuffer, int precision, int component, int filt, int wrap);
	~Filter();
public:
	void draw(Camera* camera, Render* render, RenderState* state,
		Texture* inputTexture, const Texture* depthTexture);
	void draw(Camera* camera, Render* render, RenderState* state, 
		const std::vector<Texture*>& inputTextures, const Texture* depthTexture);
	void addOutput(int precision, int component, int filt);
	void addDepthBuffer(int precision, bool useMip);
	FrameBuffer* getFrameBuffer();
	Texture* getOutput(int i);
};

struct FilterChain {
	std::vector<Texture*> input;
	Filter* output;
	FilterChain(int width, int height, bool useFramebuffer, int precision, int component, int wrapMode = WRAP_CLAMP_TO_BORDER) {
		input.clear(); 
		output = new Filter(width, height, useFramebuffer, precision, component, LINEAR, wrapMode);
	}
	~FilterChain() {
		input.clear();
		delete output;
	}
	void addInputTex(Texture* texIn) {
		input.push_back(texIn);
	}
	Texture* getOutputTex(int i) {
		return output->getOutput(i);
	}
	FrameBuffer* getOutFrameBuffer() {
		return output->getFrameBuffer();
	}
};

struct DualFilter {
	FilterChain *filter1, *filter2;
	DualFilter(int width, int height, bool useFramebuffer, int precision, int component, int wrap) {
		filter1 = new FilterChain(width, height, useFramebuffer, precision, component, wrap);
		filter2 = new FilterChain(width, height, useFramebuffer, precision, component, wrap);
		filter2->addInputTex(filter1->getOutputTex(0));
	}
	~DualFilter() {
		delete filter1;
		delete filter2;
	}
	void addInputTex(Texture* tex) {
		filter1->addInputTex(tex);
	}
	std::vector<Texture*> getInput1() {
		return filter1->input;
	}
	Filter* getOutput1() {
		return filter1->output;
	}
	std::vector<Texture*> getInput2() {
		return filter2->input;
	}
	Filter* getOutput2() {
		return filter2->output;
	}
	Texture* getOutputTex() {
		return filter2->getOutputTex(0);
	}
};

#endif /* FILTER_H_ */

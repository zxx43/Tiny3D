#include "filter.h"
#include "../object/staticObject.h"
#include "../mesh/board.h"

Filter::Filter(float width, float height, bool useFramebuffer, int precision, int component, bool clampBorder) {
	this->width=width;
	this->height=height;
	pixWidth = 1.0 / width;
	pixHeight = 1.0 / height;
	framebuffer = useFramebuffer ? new FrameBuffer(width, height, precision, component, clampBorder) : NULL;

	Board* board=new Board(2,2,2);
	boardNode=new StaticNode(vec3(0,0,0));
	boardNode->setFullStatic(true);
	StaticObject* boardObject=new StaticObject(board);
	boardNode->addObject(NULL, boardObject);
	boardNode->prepareDrawcall();
	delete board;

	setResize(true);
}

Filter::~Filter() {
	delete boardNode; boardNode=NULL;
	if(framebuffer) {
		delete framebuffer;
		framebuffer=NULL;
	}
}

void Filter::draw(Camera* camera, Render* render, RenderState* state,
		const std::vector<Texture2D*>& inputTextures, const Texture2D* depthTexture) {
	Shader* shader = state->shader;
	render->setFrameBuffer(framebuffer);
	render->setShaderVec2(shader, "pixelSize", pixWidth, pixHeight);
	render->setShaderFloat(shader, "quality", state->quality);

	if (state->shadow) {
		float shadowPixSize = state->shadow->shadowPixSize;
		render->setShaderVec2(shader, "shadowPixSize", shadowPixSize, shadowPixSize);
	}
	uint bufferid;
	for (bufferid = 0; bufferid < inputTextures.size(); bufferid++) {
		if (!shader->isTexBinded(inputTextures[bufferid]->hnd) || shouldResize) {
			if (!shader->hasSlot(bufferid)) {
				printf("error slot:%d\n", bufferid);
				continue;
			}
			shader->setHandle64(shader->getSlot(bufferid).data(), inputTextures[bufferid]->hnd);
		}
	}
	if (depthTexture) {
		if (!shader->isTexBinded(depthTexture->hnd) || shouldResize) {
			if (!shader->hasSlot(bufferid))
				printf("error slot:%d\n", bufferid);
			else
				shader->setHandle64(shader->getSlot(bufferid).data(), depthTexture->hnd);
		}
	}
	if (shouldResize) setResize(false);
	render->draw(camera, boardNode->drawcall, state);
}

void Filter::addOutput(int precision, int component) {
	if (framebuffer)
		framebuffer->addColorBuffer(precision, component);
}

void Filter::addDepthBuffer(int precision) {
	if (framebuffer)
		framebuffer->attachDepthBuffer(precision);
}

Texture2D* Filter::getOutput(int i) {
	if(framebuffer)
		return framebuffer->getColorBuffer(i);
	return NULL;
}

FrameBuffer* Filter::getFrameBuffer() {
	return framebuffer;
}


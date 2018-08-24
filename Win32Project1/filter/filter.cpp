#include "filter.h"
#include "../object/staticObject.h"
#include "../mesh/board.h"

Filter::Filter(float width, float height, bool useFramebuffer, int precision, int component) {
	this->width=width;
	this->height=height;
	pixWidth = 1.0 / width;
	pixHeight = 1.0 / height;
	framebuffer = useFramebuffer ? new FrameBuffer(width, height, precision, component) : NULL;

	Board* board=new Board(2,2,2);
	boardNode=new StaticNode(VECTOR3D(0,0,0));
	boardNode->setFullStatic(true);
	StaticObject* boardObject=new StaticObject(board);
	boardNode->addObject(boardObject);
	boardNode->prepareDrawcall();
	delete board;
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
	render->setFrameBuffer(framebuffer);
	render->useShader(state->shader);
	state->shader->setVector2("pixelSize",pixWidth,pixHeight);
	if (state->shadow) {
		float shadowPixSize = state->shadow->shadowPixSize;
		state->shader->setVector2("shadowPixSize", shadowPixSize, shadowPixSize);
	}
	uint bufferid;
	for (bufferid = 0; bufferid < inputTextures.size(); bufferid++)
		render->useTexture(TEXTURE_2D, bufferid, inputTextures[bufferid]->id);
	if (depthTexture)
		render->useTexture(TEXTURE_2D, bufferid, depthTexture->id);
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


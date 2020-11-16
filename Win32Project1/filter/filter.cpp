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
}

Filter::~Filter() {
	delete boardNode; boardNode=NULL;
	if(framebuffer) {
		delete framebuffer;
		framebuffer=NULL;
	}
}

bool Filter::bindTex(int slot, const Texture2D* tex, Shader* shader) {
	if (!shader->hasSlot(slot)) {
		printf("shader: %s error slot:%d\n", shader->name.data(), slot);
		return false;
	}
	int hnd = shader->getSlotHnd(slot);
	if (hnd < 0 || hnd != tex->hnd) {
		shader->setSlotHnd(slot, tex->hnd);
		return true;
	}
	return false;
}

void Filter::draw(Camera* camera, Render* render, RenderState* state,
		Texture2D* inputTexture, const Texture2D* depthTexture) {
	Shader* shader = state->shader;
	render->setFrameBuffer(framebuffer);
	render->setShaderVec2(shader, "pixelSize", pixWidth, pixHeight);
	render->setShaderFloat(shader, "quality", state->quality);

	if (state->shadow) {
		float shadowPixSize = state->shadow->shadowPixSize;
		render->setShaderVec2(shader, "shadowPixSize", shadowPixSize, shadowPixSize);
	}
	uint bufferid = 0;
	bindTex(bufferid, inputTexture, shader);
	++bufferid;
	if (depthTexture)
		bindTex(bufferid, depthTexture, shader);
	render->draw(camera, boardNode->drawcall, state);
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
	for (bufferid = 0; bufferid < inputTextures.size(); bufferid++)
		bindTex(bufferid, inputTextures[bufferid], shader);
	if (depthTexture) 
		bindTex(bufferid, depthTexture, shader);
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


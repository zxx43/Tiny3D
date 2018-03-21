#include "filter.h"
#include "../object/staticObject.h"
#include "../mesh/board.h"

Filter::Filter(float width, float height, bool useFramebuffer, int precision) {
	this->width=width;
	this->height=height;
	pixWidth = 1.0 / width;
	pixHeight = 1.0 / height;
	framebuffer=NULL;
	if(useFramebuffer)
		framebuffer = new FrameBuffer(width, height, precision);

	Board* board=new Board();
	boardNode=new StaticNode(VECTOR3D(0,0,0));
	boardNode->fullStatic = true;
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
	uint bufferid;
	for (bufferid = 0; bufferid < inputTextures.size(); bufferid++)
		render->useTexture(TEXTURE_2D, bufferid, inputTextures[bufferid]->id);
	if (depthTexture)
		render->useTexture(TEXTURE_2D, bufferid, depthTexture->id);
	render->draw(camera, boardNode->drawcall, state);
}

Texture2D* Filter::getOutput() {
	if(framebuffer)
		return framebuffer->getColorBuffer(0);
	return NULL;
}


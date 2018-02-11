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
		framebuffer = new FrameBuffer(width, height, false, precision);

	Board* board=new Board();
	boardNode=new StaticNode(VECTOR3D(0,0,0));
	boardNode->fullStatic = true;
	StaticObject* boardObject=new StaticObject(board);
	boardNode->addObject(boardObject);
	boardNode->prepareDrawcall();
	delete board;

	state = new RenderState();
	state->enableCull = false;
	state->lightEffect = false;
}

Filter::~Filter() {
	delete boardNode; boardNode=NULL;
	if(framebuffer) {
		delete framebuffer;
		framebuffer=NULL;
	}
	delete state; state = NULL;
}

void Filter::draw(Render* render, Shader* shader, const std::vector<Texture2D*>& inputTextures) {
	state->shader = shader;
	render->setFrameBuffer(framebuffer);
	render->useShader(shader);
	shader->setVector2("pixelSize",pixWidth,pixHeight);
	for (uint i = 0; i < inputTextures.size(); i++)
		render->useTexture(TEXTURE_2D, i, inputTextures[i]->id);
	render->draw(NULL,boardNode->drawcall,state);
	render->finishDraw();
}

Texture2D* Filter::getOutput() {
	if(framebuffer)
		return framebuffer->getColorBuffer(0);
	return NULL;
}


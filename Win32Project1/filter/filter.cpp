#include "filter.h"
#include "../object/staticObject.h"
#include "../mesh/board.h"

Filter::Filter(float width,float height,bool useFramebuffer) {
	this->width=width;
	this->height=height;
	framebuffer=NULL;
	if(useFramebuffer)
		framebuffer=new FrameBuffer(width,height,false);

	Board* board=new Board();
	boardNode=new StaticNode(VECTOR3D(0,0,0));
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
	shader->setVector2("pixelSize",1.0/width,1.0/height);
	for (unsigned int i = 0; i < inputTextures.size(); i++)
		inputTextures[i]->use(i);
	render->draw(NULL,boardNode->drawcall,state);
	render->finishDraw();
}

Texture2D* Filter::getOutput() {
	if(framebuffer)
		return framebuffer->getColorBuffer(0);
	return NULL;
}


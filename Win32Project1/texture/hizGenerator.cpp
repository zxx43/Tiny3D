#include "hizGenerator.h"
#include "../node/staticNode.h"
#include "../object/staticObject.h"
#include "../mesh/board.h"
#include "../render/render.h"

HizGenerator::HizGenerator() {
	Board board(2, 2, 2);
	boardNode = new StaticNode(vec3(0, 0, 0));
	boardNode->setFullStatic(true);
	StaticObject* boardObject = new StaticObject(&board);
	boardNode->addObject(NULL, boardObject);
	boardNode->prepareDrawcall();

	state = new RenderState();
	state->pass = POST_PASS;
	state->depthTestMode = ALWAYS;
	state->lightEffect = false;
	
	glGenFramebuffers(1, &fbo);
	numLevels = 0;
}

HizGenerator::~HizGenerator() {
	glDeleteFramebuffers(1, &fbo);
	delete boardNode; boardNode = NULL;
	delete state; state = NULL;
}

void HizGenerator::genMipmap(Render* render, Shader* shader, Texture2D* texDepth) {
	int currentWidth = render->viewWidth, currentHeight = render->viewHeight;
	numLevels = 1 + (int)floorf(log2f(fmaxf(currentWidth, currentHeight)));

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	state->shader = shader;
	uint texBefore = render->useTexture(TEXTURE_2D, 0, texDepth->id);
	render->setColorWrite(false);

	for (int i = 1; i < numLevels; i++) {
		state->shader->setFloat("uOdd", (currentWidth % 2 != 0 || currentHeight % 2 != 0) ? 1.0 : 0.0);
		state->shader->setFloat("uLastLevel", i - 1);

		currentWidth /= 2, currentHeight /= 2;
		currentWidth = currentWidth > 0 ? currentWidth : 1;
		currentHeight = currentHeight > 0 ? currentHeight : 1;

		glViewport(0, 0, currentWidth, currentHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texDepth->id, i);

		render->draw(NULL, boardNode->drawcall, state);
	}
	render->setColorWrite(true);
	render->useTexture(TEXTURE_2D, 0, texBefore);
}

void HizGenerator::drawDebug(Camera* camera, Render* render, Shader* shader, Texture2D* texDepth, int level) {
	render->setFrameBuffer(NULL);

	int levelUse = level > numLevels - 1 ? numLevels - 1 : level;
	state->shader = shader;
	state->shader->setFloat("uLevel", (float)levelUse);
	state->shader->setVector2("uCamParam", camera->zNear, camera->zFar);
	
	uint texBefore = render->useTexture(TEXTURE_2D, 0, texDepth->id);
	render->draw(NULL, boardNode->drawcall, state);

	render->useTexture(TEXTURE_2D, 0, texBefore);
}
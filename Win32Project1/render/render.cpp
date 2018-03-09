#include "render.h"
#include "../constants/constants.h"
#include <stdio.h>

Render::Render() {
	shaders=new ShaderManager();
	initEnvironment();
}

Render::~Render() {
	delete shaders;
	shaders=NULL;
	textureInUse.clear();
}

void Render::initEnvironment() {
	GLenum err=glewInit();
	if(GLEW_OK!=err)
		printf("Error: %s\n",glewGetErrorString(err));
	// Force to refresh states
	enableDepthTest=false;
	depthTestMode=GREATER;
	enableAlphaTest = true;
	alphaTestMode = LESS;
	alphaThreshold = 1.0;
	enableCull=false;
	cullMode=CULL_NONE;
	drawLine=true;
	setDepthTest(true,LEQUAL);
	setAlphaTest(false, GREATER, 0);
	setCullState(true);
	setCullMode(CULL_BACK);
	setDrawLine(false);
	setClearColor(1,1,1,1);
	currentShader=NULL;
	textureInUse.clear();
}

void Render::clearFrame(bool clearColor,bool clearDepth,bool clearStencil) {
	GLbitfield clearMask=0;
	if(clearColor) clearMask|=GL_COLOR_BUFFER_BIT;
	if(clearDepth) clearMask|=GL_DEPTH_BUFFER_BIT;
	if(clearStencil) clearMask|=GL_STENCIL_BUFFER_BIT;
	glClear(clearMask);
}

void Render::setState(const RenderState* state) {
	setDepthTest(state->enableDepthTest,state->depthTestMode);
	setAlphaTest(state->enableAlphaTest, state->alphaTestMode, state->alphaThreshold);
	setCullState(state->enableCull);
	setCullMode(state->cullMode);
	setDrawLine(state->drawLine);
}

void Render::setDepthTest(bool enable,int testMode) {
	if(enable!=enableDepthTest) {
		enableDepthTest=enable;
		if(enable) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}
	if(testMode!=depthTestMode) {
		depthTestMode=testMode;
		switch(testMode) {
			case LESS:
				glDepthFunc(GL_LESS);
				break;
			case LEQUAL:
				glDepthFunc(GL_LEQUAL);
				break;
			case GREATER:
				glDepthFunc(GL_GREATER);
				break;
			case GEQUAL:
				glDepthFunc(GL_GEQUAL);
				break;
		}
	}
}

void Render::setAlphaTest(bool enable, int testMode, float threshold) {
	if (enable != enableAlphaTest) {
		enableAlphaTest = enable;
		if (enable) glEnable(GL_ALPHA_TEST);
		else glDisable(GL_ALPHA_TEST);
	}
	if (testMode != alphaTestMode || threshold != alphaThreshold) {
		alphaTestMode = testMode;
		alphaThreshold = threshold;
		switch (testMode) {
			case LESS:
				glAlphaFunc(GL_LESS, alphaThreshold);
				break;
			case LEQUAL:
				glAlphaFunc(GL_LEQUAL, alphaThreshold);
				break;
			case GREATER:
				glAlphaFunc(GL_GREATER, alphaThreshold);
				break;
			case GEQUAL:
				glAlphaFunc(GL_GEQUAL, alphaThreshold);
				break;
		}
	}
}

void Render::setCullState(bool enable) {
	if(enable==enableCull) return;
	enableCull=enable;
	if(enable) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
}

void Render::setCullMode(int mode) {
	if(mode==cullMode) return;
	cullMode=mode;
	switch(mode) {
		case CULL_BACK:
			glCullFace(GL_BACK);
			break;
		case CULL_FRONT:
			glCullFace(GL_FRONT);
			break;
		case CULL_NONE:
			glCullFace(GL_NONE);
			break;
	}
}

void Render::setDrawLine(bool line) {
	if(line==drawLine) return;
	drawLine=line;
	if(!line) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

void Render::setClearColor(float r,float g,float b,float a) {
	clearColor.r=r; clearColor.g=g;
	clearColor.b=b; clearColor.a=a;
	glClearColor(r,g,b,a);
}

void Render::setViewPort(int width,int height) {
	viewWidth=width; viewHeight=height;
	glViewport(0,0,width,height);
}

void Render::resize(int width,int height,Camera* mainCamera) {
	if(height==0) height=1;
	setViewPort(width,height);
	float fAspect=1;
	fAspect=(float)width/(float)height;
	mainCamera->initPerspectCamera(60.0,fAspect,1.0,2000.0);
}

Shader* Render::findShader(const char* shader) {
	return shaders->findShader(shader);
}

void Render::useShader(Shader* shader) {
	if (shader == currentShader) return;
	currentShader = shader;
	shader->use();
}

void Render::draw(Camera* camera,Drawcall* drawcall,RenderState* state) {
	bool beforeCullState = state->enableCull;
	int beforeCullMode = state->cullMode;
	if (drawcall->isSingleSide()) { // Special case
		if (state->pass == 4)
			state->enableCull = false;
		else
			state->cullMode = CULL_BACK;
	}
	setState(state);

	Shader* shader = drawcall->getType() == INSTANCE_DC ? state->shaderIns : state->shader;
	useShader(shader);
	if (camera) {
		if (state->pass == 4) {
			if (drawcall->getType() == INSTANCE_DC) 
				shader->setMatrix4("viewProjectMatrix", camera->viewProjectMatrix);
			else {
				shader->setMatrix4("projectMatrix", camera->projectMatrix);
				shader->setMatrix4("viewMatrix", camera->viewMatrix);
				if (state->shadow) {
					shader->setMatrix4("lightViewProjNear", state->shadow->lightNearMat);
					shader->setMatrix4("lightViewProjMid", state->shadow->lightMidMat);
					shader->setMatrix4("lightViewProjFar", state->shadow->lightFarMat);
					shader->setVector2("levels", state->shadow->level1, state->shadow->level2);
				}
			}
			if (state->lightEffect)
				shader->setVector3("light", state->light.x, state->light.y, state->light.z);
		} else 
			shader->setMatrix4("viewProjectMatrix", camera->viewProjectMatrix);
	}

	if (drawcall->getType() == STATIC_DC && !drawcall->isFullStatic() && state->lightEffect) 
		shader->setMatrix3x4("modelMatrices", drawcall->objectCount, drawcall->uModelMatrix);
	drawcall->draw(shader, state->pass);

	if (drawcall->isSingleSide()) { // Reset state
		state->enableCull = beforeCullState;
		state->cullMode = beforeCullMode;
	}
}

void Render::finishDraw() {
	glBindVertexArray(0);
}

void Render::setFrameBuffer(FrameBuffer* framebuffer) {
	if(framebuffer) framebuffer->use();
	else {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		clearFrame(true,true,false);
		glViewport(0,0,viewWidth,viewHeight);
	}
}

void Render::useTexture(uint type, uint slot, uint texid) {
	if (textureInUse.count(slot) > 0) {
		if (textureInUse[slot] == texid) 
			return;
	}
	GLenum textureType = GL_TEXTURE_2D;
	switch (type) {
		case TEXTURE_2D:
			textureType = GL_TEXTURE_2D;
			break;
		case TEXTURE_2D_ARRAY:
			textureType = GL_TEXTURE_2D_ARRAY;
			break;
		case TEXTURE_CUBE:
			textureType = GL_TEXTURE_CUBE_MAP;
			break;
	}
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(textureType, texid);
	textureInUse[slot] = texid;
}



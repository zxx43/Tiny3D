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
	depthTestMode=DEPTH_GREATER;
	enableCull=false;
	cullMode=CULL_NONE;
	drawLine=true;
	setDepthTest(true,DEPTH_LEQUAL);
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
			case DEPTH_LESS:
				glDepthFunc(GL_LESS);
				break;
			case DEPTH_LEQUAL:
				glDepthFunc(GL_LEQUAL);
				break;
			case DEPTH_GREATER:
				glDepthFunc(GL_GREATER);
				break;
			case DEPTH_GEQUAL:
				glDepthFunc(GL_GEQUAL);
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

void Render::draw(Camera* camera,Drawcall* drawcall,const RenderState* state) {
	setState(state);
	if (drawcall->isSingleSide())
		setCullState(false);
	Shader* shader = state->shader;
	useShader(shader);
	if (camera) {
		if (!state->shadowPass) {
			shader->setMatrix4("projectMatrix", camera->projectMatrix);
			shader->setMatrix4("viewMatrix", camera->viewMatrix);
			if (state->lightEffect)
				shader->setVector3("light", state->light.x, state->light.y, state->light.z);
		} else 
			shader->setMatrix4("viewProjectMatrix", camera->projectMatrix * camera->viewMatrix);
	}
	if (state->shadow) {
		shader->setMatrix4("lightViewProjNear", state->shadow->lightNearMat);
		shader->setMatrix4("lightViewProjMid", state->shadow->lightMidMat);
		shader->setMatrix4("lightViewProjFar", state->shadow->lightFarMat);
		shader->setFloat("level1", state->shadow->level1);
		shader->setFloat("level2", state->shadow->level2);
	}
	drawcall->draw(shader, state->shadowPass);
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



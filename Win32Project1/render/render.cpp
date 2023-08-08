#include "render.h"
#include "../constants/constants.h"
#include "../assets/assetManager.h"
#include <stdio.h>

Render::Render() {
	initEnvironment();
	shaders = new ShaderManager();
	currentFrame = NULL;
}

Render::~Render() {
	delete shaders;
	shaders = NULL;
	clearTextureSlots();
}

float Render::MaxAniso = 0.0;

void Render::initEnvironment() {
	glewExperimental = GL_TRUE;
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
	enableBlend = true;
	enableColor = false;
	depthWrite = false;
	setDepthTest(true,LEQUAL);
	setAlphaTest(false, GREATER, 0);
	setCullState(true);
	setCullMode(CULL_BACK);
	setDrawLine(false);
	setBlend(false);
	setColorWrite(true);
	setDepthWrite(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	setClearColor(1,1,1,1);
	currentShader=NULL;
	clearTextureSlots();

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAniso);
	debugMode = false;
	debugTerrain = false;
	drawFog = true;

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
	setBlend(state->blend);
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
			case ALWAYS:
				glDepthFunc(GL_ALWAYS);
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
		case CULL_ALL:
			glCullFace(GL_FRONT_AND_BACK);
			break;
	}
}

void Render::setDrawLine(bool line) {
	if(line==drawLine) return;
	drawLine=line;
	if(!line) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

void Render::setBlend(bool enable) {
	if (enable == enableBlend) return;
	enableBlend = enable;
	if (enable) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}

void Render::setColorWrite(bool enable) {
	if (enable == enableColor) return;
	enableColor = enable;
	if (enable) glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	else glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
}

void Render::setDepthWrite(bool enable) {
	if (enable == depthWrite) return;
	depthWrite = enable;
	if (enable) glDepthMask(GL_TRUE);
	else glDepthMask(GL_FALSE);
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

void Render::resize(int width, int height, Camera* camera1, Camera* camera2, Camera* reflectCamera) {
	if (height == 0) height = 1;
	setViewPort(width, height);
	float fAspect = (float)width / height;
	camera1->initPerspectCamera(60.0, fAspect, 1.0, 2000.0);
	if (camera2 && camera2 != camera1)
		camera2->initPerspectCamera(60.0, fAspect, 1.0, 2000.0);
	if (reflectCamera) 
		reflectCamera->initPerspectCamera(60.0, fAspect, 0.1, 2000.0);
}

Shader* Render::findShader(const char* shader) {
	return shaders->findShader(shader);
}

void Render::useShader(Shader* shader) {
	if (shader == currentShader) return;
	currentShader = shader;
	shader->use();
}

// Pass 1 draw near shadow
// Pass 2 draw mid shadow
// Pass 3 draw far shadow (use simple buffer)
// Pass 4 draw scene with color
// Pass 5 deferred process
// Pass 6 fxaa dof ssr ssg
void Render::draw(Camera* camera,Drawcall* drawcall,RenderState* state) {
	if (drawcall == NULL) { 
		if (getDebug()) printf("Render: No drawcall!\n");
		return; 
	}

	setState(state);
	Shader* shader = state->shader;
	if (drawcall->getType() == INSTANCE_DC || drawcall->getType() == MULTI_DC)
		shader = state->shaderIns;
	else if (drawcall->getType() == ANIMATE_DC)
		shader = state->shaderBone;
	
	if (camera) {
		if (state->pass < DEFERRED_PASS) {
			if (!state->skyPass && !state->atmoPass && !state->iblPass) {
				shader->setMatrix4("viewProjectMatrix", camera->viewProjectMatrix);
				if (state->tess && state->grassPass) {
					shader->setFloat("time", state->time * 0.025);
					shader->setVector3v("eyePos", camera->position);
					shader->setMatrix4("viewMatrix", camera->viewMatrix);
					shader->setFloat("distortionId", AssetManager::assetManager->getDistortionTex());
					shader->setFloat("quality", state->quality);
				}

				if (state->shaderCompute) {
					state->shaderCompute->setMatrix4("viewProjectMatrix", camera->viewProjectMatrix);
					if(!state->grassPass)
						state->shaderCompute->setFloat("isColor", state->pass >= COLOR_PASS ? 1.0 : 0.0);
					else {
						state->shaderCompute->setVector3v("eyePos", camera->position);
						state->shaderCompute->setMatrix4("viewMatrix", camera->viewMatrix);
						state->shaderCompute->setHandle64("distortionTex", AssetManager::assetManager->getDistortionHnd());
						state->shaderCompute->setHandle64("roadTex", AssetManager::assetManager->getRoadHnd());
						state->shaderCompute->setHandle64("heightTex", AssetManager::assetManager->getHeightHnd());
						state->shaderCompute->setHandle64("heightNormal", AssetManager::assetManager->getHeightNormalHnd());
						state->shaderCompute->setFloat("time", state->time * 0.025);
						state->shaderCompute->setFloat("quality", state->quality);
					}
				}

				if (drawcall->getType() == MULTI_DC) {
					// Billboard drawcall
					if (state->shaderBill) {
						state->shaderBill->setMatrix4("viewProjectMatrix", camera->viewProjectMatrix);
						if (state->pass != COLOR_PASS)
							state->shaderBill->setVector3("viewRight", state->light.z, 0.0, -state->light.x);
						else if (state->pass == COLOR_PASS) {
							static vec3 upVec(0.0, 1.0, 0.0);
							vec3 viewRight(camera->viewMatrix.entries[0], camera->viewMatrix.entries[4], camera->viewMatrix.entries[8]);
							vec3 normal = (viewRight.CrossProduct(upVec)).GetNormalized();
							normal = normal * 0.5 + 0.5;
							state->shaderBill->setVector3v("uNormal", normal);
							state->shaderBill->setVector3v("viewRight", viewRight);
						}
					}
				}

				if (state->waterPass) {
					shader->setFloat("time", state->time);
					shader->setVector3v("light", -state->light);
					shader->setFloat("udotl", state->udotl);
					shader->setVector3v("eyePos", camera->position);
					shader->setMatrix4("viewMatrix", camera->viewMatrix);
					shader->setMatrix4("invViewProjMatrix", camera->invViewProjectMatrix);
					if (state->enableSsr)
						shader->setVector2("waterBias", 0.0, 0.0);
					else
						shader->setVector2("waterBias", 0.05, -0.05);
					if (AssetManager::assetManager->getEnvTexture() &&
						!shader->isTexBinded(AssetManager::assetManager->getEnvTexture()->hnd))
							shader->setHandle64("texEnv", AssetManager::assetManager->getEnvTexture()->hnd);
					if (AssetManager::assetManager->getSkyTexture() &&
						!shader->isTexBinded(AssetManager::assetManager->getSkyTexture()->hnd))
							shader->setHandle64("texSky", AssetManager::assetManager->getSkyTexture()->hnd);
					if (AssetManager::assetManager->getReflectTexture() &&
						!shader->isTexBinded(AssetManager::assetManager->getReflectTexture()->hnd))
							shader->setHandle64("texRef", AssetManager::assetManager->getReflectTexture()->hnd);
					if (AssetManager::assetManager->getSceneTexture() &&
						!shader->isTexBinded(AssetManager::assetManager->getSceneTexture()->hnd))
							shader->setHandle64("texScene", AssetManager::assetManager->getSceneTexture()->hnd);
					if (AssetManager::assetManager->getSceneDepth() &&
						!shader->isTexBinded(AssetManager::assetManager->getSceneDepth()->hnd))
							shader->setHandle64("sceneDepth", AssetManager::assetManager->getSceneDepth()->hnd);
				}
			} else if (state->skyPass) {
				mat4 viewMat = camera->viewMatrix;
				viewMat.entries[12] = 0.0, viewMat.entries[13] = 0.0, viewMat.entries[14] = 0.0;
				shader->setMatrix4("viewProjectMatrix", camera->projectMatrix * viewMat);
				if (AssetManager::assetManager->getSkyTexture() &&
					!shader->isTexBinded(AssetManager::assetManager->getSkyTexture()->hnd))
						shader->setHandle64("texSky", AssetManager::assetManager->getSkyTexture()->hnd);
			}
		} else if (state->pass == DEFERRED_PASS) {
			shader->setMatrix4("invViewProjMatrix", camera->invViewProjectMatrix);
			shader->setMatrix4("invProjMatrix", camera->invProjMatrix);
			shader->setMatrix4("viewMatrix", camera->viewMatrix);
			shader->setVector3v("eyePos", camera->position);
			shader->setFloat("time", state->time);

			float invViewMat[9] = { camera->invViewMatrix[0], camera->invViewMatrix[1], camera->invViewMatrix[2],
								camera->invViewMatrix[4], camera->invViewMatrix[5], camera->invViewMatrix[6],
								camera->invViewMatrix[8], camera->invViewMatrix[9], camera->invViewMatrix[10] };
			shader->setMatrix3("invViewMatrix", invViewMat);

			if (state->shadow) {
				Camera* camDyn = state->shadow->renderLightCameraDyn;
				Camera* camNear = state->shadow->renderLightCameraNear;
				Camera* camMid = state->shadow->renderLightCameraMid;
				Camera* camFar = state->shadow->renderLightCameraFar;

				if (state->shadow->flushDyn) shader->setMatrix4("lightViewProjDyn", camDyn->viewProjectMatrix);
				if (state->shadow->flushNear) shader->setMatrix4("lightViewProjNear", camNear->viewProjectMatrix);
				if (state->shadow->flushMid) shader->setMatrix4("lightViewProjMid", camMid->viewProjectMatrix);
				if (state->shadow->flushFar) shader->setMatrix4("lightViewProjFar", camFar->viewProjectMatrix);
				shader->setVector2("gaps", state->shadow->gap, state->shadow->inv2Gap);
				shader->setVector3("levels", state->shadow->level1, state->shadow->level2, state->shadow->radius);
				shader->setVector3v("shadowCenter", camNear->position);
			}
			if (state->lightEffect) {
				shader->setVector3("light", -state->light.x, -state->light.y, -state->light.z);
				shader->setFloat("udotl", state->udotl);
			}
		} else if (state->pass > DEFERRED_PASS && state->ssgPass) {
			shader->setMatrix4("invProjMatrix", camera->invProjMatrix);
			shader->setFloat("time", state->time);
		} else if (state->pass == POST_PASS) {
			shader->setMatrix4("invViewProjMatrix", camera->invViewProjectMatrix);
			shader->setVector3v("eyePos", camera->position);
			shader->setFloat("udotl", state->udotl);
		}
		if (state->ssrPass) {
			shader->setMatrix4("viewMatrix", camera->viewMatrix);
			shader->setMatrix4("projectMatrix", camera->projectMatrix);
			shader->setMatrix4("invProjMatrix", camera->invProjMatrix);
			shader->setVector2("screenSize", viewWidth, viewHeight);
		}

		if (state->pass < COLOR_PASS) {
			shader->setMatrix4("projectMatrix", camera->projectMatrix);
			shader->setMatrix4("viewMatrix", camera->viewMatrix);
			shader->setVector2("camPara", camera->zNear, camera->invDist);

			if (state->shaderBill) {
				state->shaderBill->setMatrix4("projectMatrix", camera->projectMatrix);
				state->shaderBill->setMatrix4("viewMatrix", camera->viewMatrix);
				state->shaderBill->setVector2("camPara", camera->zNear, camera->invDist);
			}
		} else if (state->pass == DEFERRED_PASS) {
			if (state->shadow) {
				Camera* camDyn = state->shadow->renderLightCameraDyn;
				Camera* camNear = state->shadow->renderLightCameraNear;
				Camera* camMid = state->shadow->renderLightCameraMid;
				Camera* camFar = state->shadow->renderLightCameraFar;

				if (state->shadow->flushDyn) {
					shader->setMatrix4("lightProjDyn", camDyn->projectMatrix);
					shader->setMatrix4("lightViewDyn", camDyn->viewMatrix);
				}
				if (state->shadow->flushNear) {
					shader->setMatrix4("lightProjNear", camNear->projectMatrix);
					shader->setMatrix4("lightViewNear", camNear->viewMatrix);
				}

				if (state->shadow->flushMid) {
					shader->setMatrix4("lightProjMid", camMid->projectMatrix);
					shader->setMatrix4("lightViewMid", camMid->viewMatrix);
				}

				if (state->shadow->flushFar) {
					shader->setMatrix4("lightProjFar", camFar->projectMatrix);
					shader->setMatrix4("lightViewFar", camFar->viewMatrix);
				}

				float camParas[12] = { 
					camDyn->zNear, camDyn->invDist, camDyn->zFar,
					camNear->zNear, camNear->invDist, camNear->zFar,
					camMid->zNear, camMid->invDist, camMid->zFar,
					camFar->zNear, camFar->invDist, camFar->zFar 
				};
				shader->setVector3v("camParas", 4, camParas);
			}
		}
	} else {
		if (state->atmoPass) {
			if (AssetManager::assetManager->getNoiseTex() >= 0 &&
				!shader->isTexBinded(AssetManager::assetManager->getNoiseHnd()))
					shader->setHandle64("texNoise", AssetManager::assetManager->getNoiseHnd());
		} else if (state->iblPass) {
			if (AssetManager::assetManager->getSkyTexture() &&
				!shader->isTexBinded(AssetManager::assetManager->getSkyTexture()->hnd)) {
					shader->setFloat("uResolution", AssetManager::assetManager->getSkyTexture()->getWidth());
					shader->setHandle64("environmentMap", AssetManager::assetManager->getSkyTexture()->hnd);
			}
		}
	}

	drawcall->draw(this, state, shader);
}

void Render::finishDraw() {
	//glBindVertexArray(0);
	getError();
}

void Render::setFrameBuffer(FrameBuffer* framebuffer) {
	currentFrame = framebuffer;
	if(framebuffer) framebuffer->use();
	else {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		clearFrame(true,true,false);
		glViewport(0,0,viewWidth,viewHeight);
	}
}

void Render::useFrameBuffer(FrameBuffer* framebuffer) {
	if (framebuffer) {
		currentFrame = framebuffer;
		currentFrame->useFbo();
	}
}

void Render::useFrameCube(int i, int mip) {
	if (currentFrame) currentFrame->useCube(i, mip);
}

void Render::setColorMask(bool r, bool g, bool b, bool a) {
	glColorMask(r, g, b, a);
}

int Render::getError() {
	GLenum error = debugMode ? glGetError() : GL_NO_ERROR;
	if (error != GL_NO_ERROR)
		printf("gl error! %d\n", error);
	return (int)error;
}

uint Render::useTexture(uint type, uint slot, uint texid) {
	std::map<uint, std::map<uint, uint>*>::iterator typeItor = textureTypeSlots.find(type);
	std::map<uint, uint>* textureInUse = NULL;
	if (typeItor != textureTypeSlots.end())
		textureInUse = typeItor->second;
	else {
		std::map<uint, uint>* slotMap = new std::map<uint, uint>();
		textureTypeSlots[type] = slotMap;
		slotMap->clear();
		textureInUse = slotMap;
	}

	int beforeTex = 0; // Default no texture use
	std::map<uint, uint>::iterator texItor = textureInUse->find(slot);
	if (texItor != textureInUse->end()) { // Can find before texture
		beforeTex = texItor->second;
		if (beforeTex == texid) return beforeTex;
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
	glBindTextureUnit(slot, texid);
	(*textureInUse)[slot] = texid;

	return beforeTex;
}

void Render::clearTextureSlots() {
	std::map<uint, std::map<uint, uint>*>::iterator itor = textureTypeSlots.begin();
	while (itor != textureTypeSlots.end()) {
		delete itor->second;
		++itor;
	}
	textureTypeSlots.clear();
}

void Render::setTextureBindless2Shaders(TextureBindless* tex) {
	std::vector<Shader*>* shaders2Bind = shaders->getShaderBindTex();
	for(uint i = 0; i<shaders2Bind->size(); i++)
		(*shaders2Bind)[i]->setHandle64v("texBlds", tex->getSize(), tex->getHnds());
}


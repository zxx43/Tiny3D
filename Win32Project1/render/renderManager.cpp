#include "renderManager.h"
#include "../assets/assetManager.h"
#include "../mesh/board.h"
#include "../object/staticObject.h"

RenderManager::RenderManager(int quality, Camera* view, float distance1, float distance2, bool copyData, const vec3& light) {
	graphicQuality = quality;
	int precision = LOW_PRE;
	shadow = new Shadow(view);
	float nearSize = 1024;
	float midSize = 1024;
	float farSize = 512;
	if (graphicQuality > 6.0) {
		nearSize = 4096;
		midSize = 1024;
		farSize = 512;
		precision = HIGH_PRE;
	} else if (graphicQuality > 4.0) {
		nearSize = 2048;
		midSize = 1024;
		farSize = 512;
		precision = HIGH_PRE;
	}
	shadow->shadowMapSize = nearSize;
	shadow->shadowPixSize = 0.4 / nearSize;

	nearBuffer = new FrameBuffer(nearSize, nearSize, precision);
	midBuffer = new FrameBuffer(midSize, midSize, precision);
	farBuffer = new FrameBuffer(farSize, farSize, LOW_PRE);
	lightDir = light.GetNormalized();

	queue1 = new Renderable(distance1, distance2, copyData); 
	queue2 = new Renderable(distance1, distance2, copyData); 
	currentQueue = queue1;
	nextQueue = queue2;

	useShadow = false;
	drawBounding = true;
	state = new RenderState();

	time = 0.0;
	enableSsr = false;
	reflectBuffer = NULL;
}

RenderManager::~RenderManager() {
	delete shadow; shadow = NULL;
	delete nearBuffer; nearBuffer = NULL;
	delete midBuffer; midBuffer = NULL;
	delete farBuffer; farBuffer = NULL;

	delete queue1; queue1 = NULL;
	delete queue2; queue2 = NULL;

	delete state; state = NULL;
	if (reflectBuffer) delete reflectBuffer; reflectBuffer = NULL;
}

void RenderManager::act(float dTime) {
	time = dTime * 0.025;
}

void RenderManager::resize(float width, float height) {
	if (reflectBuffer) delete reflectBuffer; reflectBuffer = NULL;
	if (!enableSsr) {
		reflectBuffer = new FrameBuffer(width, height, LOW_PRE, 4, false);
		reflectBuffer->addColorBuffer(LOW_PRE, 4);
		reflectBuffer->addColorBuffer(LOW_PRE, 3);
		reflectBuffer->attachDepthBuffer(LOW_PRE);
	}
}

void RenderManager::updateShadowCamera(Camera* mainCamera) {
	shadow->prepareViewCamera(mainCamera->zFar * 0.25, mainCamera->zFar * 0.75);
}

void RenderManager::updateMainLight() {
	shadow->update(lightDir);
}

void RenderManager::flushRenderQueues() {
	renderData->flush();
}

void RenderManager::updateRenderQueues(Scene* scene) {
	Camera* cameraNear = shadow->lightCameraNear;
	Camera* cameraMid = shadow->lightCameraMid;
	Camera* cameraFar = shadow->lightCameraFar;
	Camera* cameraMain = scene->mainCamera;

	PushNodeToQueue(renderData->queues[QUEUE_STATIC_SN], scene, scene->staticRoot, cameraNear, cameraMain);
	PushNodeToQueue(renderData->queues[QUEUE_STATIC_SM], scene, scene->staticRoot, cameraMid, cameraMain);
	PushNodeToQueue(renderData->queues[QUEUE_STATIC_SF], scene, scene->staticRoot, cameraFar, cameraMain);
	PushNodeToQueue(renderData->queues[QUEUE_STATIC], scene, scene->staticRoot, cameraMain, cameraMain);
	PushNodeToQueue(renderData->queues[QUEUE_ANIMATE_SN], scene, scene->animationRoot, cameraNear, cameraMain);
	PushNodeToQueue(renderData->queues[QUEUE_ANIMATE_SM], scene, scene->animationRoot, cameraMid, cameraMain);
	PushNodeToQueue(renderData->queues[QUEUE_ANIMATE_SF], scene, scene->animationRoot, cameraFar, cameraMain);
	PushNodeToQueue(renderData->queues[QUEUE_ANIMATE], scene, scene->animationRoot, cameraMain, cameraMain);
}

void RenderManager::animateQueues(long startTime, long currentTime) {
	currentQueue->queues[QUEUE_ANIMATE_SN]->animate(startTime, currentTime);
	currentQueue->queues[QUEUE_ANIMATE_SM]->animate(startTime, currentTime);
	currentQueue->queues[QUEUE_ANIMATE_SF]->animate(startTime, currentTime);
	currentQueue->queues[QUEUE_ANIMATE]->animate(startTime, currentTime);
}

void RenderManager::swapRenderQueues(Scene* scene, bool swapQueue) {
	if (swapQueue) {
		currentQueue = (currentQueue == queue1) ? queue2 : queue1;
		nextQueue = (nextQueue == queue1) ? queue2 : queue1;

		renderData = nextQueue;
	} else {
		currentQueue = queue1;
		renderData = currentQueue;
	}

	flushRenderQueues();
	updateRenderQueues(scene);
}

void RenderManager::renderShadow(Render* render, Scene* scene) {
	if (!useShadow) return;

	static Shader* phongShadowShader = render->findShader("phong_s");
	static Shader* phongShadowInsShader = render->findShader("phong_s_ins");
	static Shader* phongShadowInsSimpShader = render->findShader("phong_s_ins_simp");
	static Shader* billboardShadowShader = render->findShader("billboard_s");
	static Shader* boneShadowShader = render->findShader("bone_s");
	static Shader* phongShadowLowShader = render->findShader("phong_sl");
	static Shader* phongShadowLowInsShader = render->findShader("phong_sl_ins");
	static Shader* grassShadowShader = render->findShader("grass_s");

	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	state->cullMode = CULL_FRONT;
	state->light = lightDir;
	state->time = time;

	Camera* mainCamera = scene->mainCamera;

	render->setFrameBuffer(nearBuffer);
	Camera* cameraNear=shadow->lightCameraNear;
	state->pass = NEAR_SHADOW_PASS;
	state->shader = phongShadowShader;
	state->shaderIns = phongShadowInsShader;
	state->shaderBillboard = billboardShadowShader;
	state->shaderSimple = phongShadowInsSimpShader;
	state->shaderGrass = grassShadowShader;
	currentQueue->queues[QUEUE_STATIC_SN]->draw(scene, cameraNear, render, state);
	state->shader = boneShadowShader;
	currentQueue->queues[QUEUE_ANIMATE_SN]->draw(scene, cameraNear, render, state);

	render->setFrameBuffer(midBuffer);
	Camera* cameraMid=shadow->lightCameraMid;
	state->pass = MID_SHADOW_PASS;
	state->shader = phongShadowShader;
	currentQueue->queues[QUEUE_STATIC_SM]->draw(scene, cameraMid, render, state);
	state->shader = boneShadowShader;
	currentQueue->queues[QUEUE_ANIMATE_SM]->draw(scene, cameraMid, render, state);

	///*
	static ushort flushCount = 1;
	if (flushCount % 3 == 0) 
		flushCount = 1;
	else {
		if (flushCount % 3 == 1) {
			render->setFrameBuffer(farBuffer);
			if (graphicQuality > 5) {
				Camera* cameraFar = shadow->lightCameraFar;
				state->pass = FAR_SHADOW_PASS;
				state->shader = phongShadowLowShader;
				state->shaderIns = phongShadowLowInsShader;
				currentQueue->queues[QUEUE_STATIC_SF]->draw(scene, cameraFar, render, state);
				state->shader = boneShadowShader;
				currentQueue->queues[QUEUE_ANIMATE_SF]->draw(scene, cameraFar, render, state);
			}
		}
		flushCount++;
	}
	//*/
}

void RenderManager::renderScene(Render* render, Scene* scene) {
	static Shader* phongShader = render->findShader("phong");
	static Shader* phongInsShader = render->findShader("phong_ins");
	static Shader* phongInsSimpShader = render->findShader("phong_ins_simp");
	static Shader* billboardShader = render->findShader("billboard");
	static Shader* boneShader = render->findShader("bone");
	static Shader* skyShader = render->findShader("sky");
	static Shader* grassShader = render->findShader("grass");

	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	state->light = lightDir;
	state->time = time;
	state->enableSsr = enableSsr;

	Camera* camera = scene->mainCamera;

	// Draw grass & terrain
	TerrainNode* terrainNode = scene->terrainNode;
	if (terrainNode && terrainNode->checkInCamera(camera)) {
		static Shader* grassLayerShader = render->findShader("grassLayer");
		state->shader = grassLayerShader;
		state->tess = true;
		state->enableCull = false;
		Terrain* terrainMesh = terrainNode->getMesh();
		((StaticDrawcall*)terrainNode->drawcall)->updateBuffers(state->pass, terrainMesh->visualIndices, terrainMesh->visualIndCount);
		render->draw(camera, terrainNode->drawcall, state);

		static Shader* terrainShader = render->findShader("terrain");
		state->shader = terrainShader;
		state->tess = false;
		state->enableCull = true;
		((StaticDrawcall*)terrainNode->drawcall)->updateBuffers(state->pass);
		render->draw(camera, terrainNode->drawcall, state);
	}

	state->shader = phongShader;
	state->shaderIns = phongInsShader;
	state->shaderBillboard = billboardShader;
	state->shaderSimple = phongInsSimpShader;
	state->shaderGrass = grassShader;
	currentQueue->queues[QUEUE_STATIC]->draw(scene, camera, render, state);

	state->shader = boneShader;
	currentQueue->queues[QUEUE_ANIMATE]->draw(scene, camera, render, state);

	// Draw sky
	if (scene->skyBox)
		scene->skyBox->draw(render, skyShader, camera);

	// Debug mode
	if (drawBounding) {
		static bool boxInit = false;
		if (!boxInit && scene->inited) {
			scene->clearAllAABB();
			scene->createNodeAABB(scene->staticRoot);
			scene->createNodeAABB(scene->animationRoot);
			boxInit = true;
		}

		if (boxInit) {
			static Shader* debugShader = render->findShader("debug");
			state->enableCull = false;
			state->drawLine = true;
			state->enableAlphaTest = false;
			state->shader = debugShader;
			drawBoundings(render, state, scene, camera);
		}
	}

	scene->flushNodes();
}

void RenderManager::renderWater(Render* render, Scene* scene) {
	static Shader* waterShader = render->findShader("water");
	Camera* camera = scene->mainCamera;
	if (scene->water && scene->water->checkInCamera(camera)) {
		state->reset();
		state->eyePos = &(scene->mainCamera->position);
		state->light = lightDir;
		state->time = time;
		state->enableSsr = enableSsr;
		state->waterPass = true;
		state->shader = waterShader;

		render->draw(camera, scene->water->drawcall, state);
	}
}

void RenderManager::renderReflect(Render* render, Scene* scene) {
	if (!scene->water || !scene->reflectCamera || !reflectBuffer) return;
	render->setFrameBuffer(reflectBuffer);
	if (scene->terrainNode) {
		if (scene->terrainNode->checkInCamera(scene->reflectCamera)) {
			if (scene->terrainNode->drawcall) {
				static Shader* terrainShader = render->findShader("terrain");
				state->reset();
				state->eyePos = &(scene->mainCamera->position);
				state->cullMode = CULL_FRONT;
				state->light = lightDir;
				state->shader = terrainShader;
				
				render->setShaderFloat(terrainShader, "isReflect", 1.0);
				render->setShaderFloat(terrainShader, "waterHeight", scene->water->position.y);
				render->draw(scene->reflectCamera, scene->terrainNode->drawcall, state);
				render->setShaderFloat(terrainShader, "isReflect", 0.0);
			}
		}
	}
}

void RenderManager::drawDeferred(Render* render, Scene* scene, FrameBuffer* screenBuff, Filter* filter) {
	static Shader* deferredShader = render->findShader("deferred");
	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	//state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = DEFERRED_PASS;
	state->shader = deferredShader;
	state->shadow = shadow;
	state->light = lightDir;
	state->time = time;
	state->quality = graphicQuality;

	uint baseSlot = screenBuff->colorBuffers.size() + 1; // Color buffers + Depth buffer
	if(!deferredShader->isTexBinded(nearBuffer->getDepthBuffer()->hnd))
		deferredShader->setHandle64("depthBufferNear", nearBuffer->getDepthBuffer()->hnd);
	if(!deferredShader->isTexBinded(midBuffer->getDepthBuffer()->hnd))
		deferredShader->setHandle64("depthBufferMid", midBuffer->getDepthBuffer()->hnd);
	if(!deferredShader->isTexBinded(farBuffer->getDepthBuffer()->hnd))
		deferredShader->setHandle64("depthBufferFar", farBuffer->getDepthBuffer()->hnd);
	render->setShaderInt(deferredShader, "useShadow", useShadow ? 1 : 0);
	filter->draw(scene->mainCamera, render, state, screenBuff->colorBuffers, screenBuff->depthBuffer);
}

void RenderManager::drawCombined(Render* render, Scene* scene, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	static Shader* combinedShader = render->findShader("combined");
	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	//state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = DEFERRED_PASS;
	state->shader = combinedShader;
	state->quality = graphicQuality;

	filter->draw(scene->mainCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, FrameBuffer* inputBuff, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	//state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;

	filter->draw(scene->mainCamera, render, state, inputBuff->colorBuffers, NULL);
}

void RenderManager::drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	//state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;

	filter->draw(scene->mainCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawSSRFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	//state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;
	state->ssrPass = true;

	filter->draw(scene->mainCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawSSGFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	//state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;
	state->ssgPass = true;

	filter->draw(scene->mainCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawTexture2Screen(Render* render, Scene* scene, u64 texhnd) {
	static Shader* screenShader = render->findShader("screen");
	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	//state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = screenShader;
	state->shader->setHandle64("tex", texhnd);

	if (!scene->textureNode) {
		Board* board = new Board(2, 2, 2);
		scene->textureNode = new StaticNode(vec3(0, 0, 0));
		scene->textureNode->setFullStatic(true);
		StaticObject* boardObject = new StaticObject(board);
		scene->textureNode->addObject(scene, boardObject);
		scene->textureNode->prepareDrawcall();
		delete board;
	}

	render->setFrameBuffer(NULL);
	render->draw(scene->mainCamera, scene->textureNode->drawcall, state);
}

void RenderManager::drawBoundings(Render* render, RenderState* state, Scene* scene, Camera* camera) {
	for (unsigned int i = 0; i<scene->boundingNodes.size(); i++) {
		Node* node = scene->boundingNodes[i];
		render->draw(camera, node->drawcall, state);
	}
}

void RenderManager::showShadow(bool enable) {
	useShadow = enable;
}

void RenderManager::showBounding(bool enable) {
	drawBounding = enable;
}

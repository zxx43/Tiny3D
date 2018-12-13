#include "renderManager.h"
#include "../assets/assetManager.h"
#include "../mesh/board.h"
#include "../object/staticObject.h"

RenderManager::RenderManager(int quality, Camera* view, float distance1, float distance2, const VECTOR3D& light) {
	graphicQuality = quality;
	int precision = LOW_PRE;
	shadow=new Shadow(view,distance1,distance2);
	float nearSize = 1024;
	float midSize = 1024;
	float farSize = 1024;
	if (graphicQuality > 4.0) {
		nearSize = 2048;
		midSize = 2048;
		farSize = 2048;
		precision = HIGH_PRE;
	}
	shadow->shadowMapSize = nearSize;
	shadow->shadowPixSize = 0.4 / nearSize;

	nearBuffer = new FrameBuffer(nearSize, nearSize, precision);
	midBuffer = new FrameBuffer(midSize, midSize, precision);
	farBuffer = new FrameBuffer(farSize, farSize, precision);
	lightDir = light; lightDir.Normalize();

	queue1 = new Renderable(distance1, distance2); queue1->copyCamera(view);
	queue2 = new Renderable(distance1, distance2); queue2->copyCamera(view);
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

void RenderManager::act() {
	time += 0.1;
}

void RenderManager::resize(float width, float height) {
	if (reflectBuffer) delete reflectBuffer;
	reflectBuffer = new FrameBuffer(width, height, LOW_PRE, 4);
	reflectBuffer->addColorBuffer(LOW_PRE, 4);
	reflectBuffer->addColorBuffer(LOW_PRE, 3);
	reflectBuffer->attachDepthBuffer(LOW_PRE);
}

void RenderManager::updateShadowCamera() {
	shadow->prepareViewCamera();
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

	pushNodeToQueue(renderData->queues[QUEUE_STATIC_SN], scene->staticRoot, cameraNear, cameraMain->position);
	pushNodeToQueue(renderData->queues[QUEUE_STATIC_SM], scene->staticRoot, cameraMid, cameraMain->position);
	pushNodeToQueue(renderData->queues[QUEUE_STATIC_SF], scene->staticRoot, cameraFar, cameraMain->position);
	pushNodeToQueue(renderData->queues[QUEUE_STATIC], scene->staticRoot, cameraMain, cameraMain->position);
	pushNodeToQueue(renderData->queues[QUEUE_ANIMATE_SN], scene->animationRoot, cameraNear, cameraMain->position);
	pushNodeToQueue(renderData->queues[QUEUE_ANIMATE_SM], scene->animationRoot, cameraMid, cameraMain->position);
	pushNodeToQueue(renderData->queues[QUEUE_ANIMATE_SF], scene->animationRoot, cameraFar, cameraMain->position);
	pushNodeToQueue(renderData->queues[QUEUE_ANIMATE], scene->animationRoot, cameraMain, cameraMain->position);
}

void RenderManager::animateQueues(long startTime, long currentTime) {
	currentQueue->queues[QUEUE_ANIMATE_SN]->animate(startTime, currentTime);
	currentQueue->queues[QUEUE_ANIMATE_SM]->animate(startTime, currentTime);
	currentQueue->queues[QUEUE_ANIMATE_SF]->animate(startTime, currentTime);
	currentQueue->queues[QUEUE_ANIMATE]->animate(startTime, currentTime);
}

void RenderManager::swapRenderQueues(Scene* scene) {
	currentQueue = (currentQueue == queue1) ? queue2 : queue1;
	nextQueue = (nextQueue == queue1) ? queue2 : queue1;

	renderData = nextQueue;
	flushRenderQueues();
	updateRenderQueues(scene);
}

void RenderManager::renderShadow(Render* render, Scene* scene) {
	if (!useShadow) return;

	//render->setColorMask(true, false, false, false);

	static Shader* phongShadowShader = render->findShader("phong_s");
	static Shader* phongShadowInsShader = render->findShader("phong_s_ins");
	static Shader* billboardShadowShader = render->findShader("billboard_s");
	static Shader* boneShadowShader = render->findShader("bone_s");
	static Shader* phongShadowLowShader = render->findShader("phong_sl");
	static Shader* phongShadowLowInsShader = render->findShader("phong_sl_ins");

	state->reset();
	state->cullMode = CULL_FRONT;
	state->light = lightDir;
	state->time = time;

	Camera* mainCamera = scene->mainCamera;
	VECTOR3D eye = mainCamera->position;

	render->useTexture(TEXTURE_2D, 0, AssetManager::assetManager->texAlt->texId);

	render->setFrameBuffer(nearBuffer);
	Camera* cameraNear=shadow->lightCameraNear;
	state->pass = NEAR_SHADOW_PASS;
	state->shader = phongShadowShader;
	state->shaderIns = phongShadowInsShader;
	state->shaderBillboard = billboardShadowShader;
	currentQueue->queues[QUEUE_STATIC_SN]->draw(cameraNear, eye, render, state);
	state->shader = boneShadowShader;
	currentQueue->queues[QUEUE_ANIMATE_SN]->draw(cameraNear, eye, render, state);

	render->setFrameBuffer(midBuffer);
	Camera* cameraMid=shadow->lightCameraMid;
	state->pass = MID_SHADOW_PASS;
	state->shader = phongShadowShader;
	currentQueue->queues[QUEUE_STATIC_SM]->draw(cameraMid, eye, render, state);
	state->shader = boneShadowShader;
	currentQueue->queues[QUEUE_ANIMATE_SM]->draw(cameraMid, eye, render, state);

	///*
	//static ushort flushCount = 1;
	//if (flushCount % 2 == 0) 
	//	flushCount = 1;
	//else {
	//	if (flushCount % 2 == 1) {
			render->setFrameBuffer(farBuffer);
			if (graphicQuality > 2) {
				Camera* cameraFar = shadow->lightCameraFar;
				state->pass = FAR_SHADOW_PASS;
				state->shader = phongShadowLowShader;
				state->shaderIns = phongShadowLowInsShader;
				currentQueue->queues[QUEUE_STATIC_SF]->draw(cameraFar, eye, render, state);
				state->shader = boneShadowShader;
				currentQueue->queues[QUEUE_ANIMATE_SF]->draw(cameraFar, eye, render, state);
			}
	//	}
	//	flushCount++;
	//}
	//*/

	//render->setColorMask(true, true, true, true);
}

void RenderManager::renderScene(Render* render, Scene* scene) {
	static Shader* phongShader = render->findShader("phong");
	static Shader* phongInsShader = render->findShader("phong_ins");
	static Shader* billboardShader = render->findShader("billboard");
	static Shader* boneShader = render->findShader("bone");

	state->reset();
	state->light = lightDir;
	state->time = time;

	Camera* camera = scene->mainCamera;
	render->useTexture(TEXTURE_2D, 0, AssetManager::assetManager->texAlt->texId);
	state->shader = phongShader;
	state->shaderIns = phongInsShader;
	state->shaderBillboard = billboardShader;
	currentQueue->queues[QUEUE_STATIC]->draw(camera, camera->position, render, state);

	state->shader = boneShader;
	currentQueue->queues[QUEUE_ANIMATE]->draw(camera, camera->position, render, state);

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

	// Draw sky
	if (scene->skyBox) {
		static Shader* skyShader = render->findShader("sky");
		scene->skyBox->draw(render, skyShader, camera);
	}

	// Draw water
	if (scene->water && scene->water->checkInCamera(scene->mainCamera)) {
		if (scene->water->needCreateDrawcall) scene->water->prepareDrawcall();
		else {
			static Shader* waterShader = render->findShader("water");
			Shader* shader = state->shader;
			state->shader = waterShader;
			state->waterPass = true;
			state->enableSsr = enableSsr;
			render->draw(camera, scene->water->drawcall, state);
		}
	}

	scene->flushNodes();
}

void RenderManager::renderReflect(Render* render, Scene* scene) {
	if (!scene->water || !scene->reflectCamera || !reflectBuffer) return;
	render->setFrameBuffer(reflectBuffer);
	if (scene->terrainNode) {
		if (scene->terrainNode->checkInCamera(scene->reflectCamera)) {
			if (scene->terrainNode->drawcall) {
				static Shader* terrainShader = render->findShader("terrain");
				state->reset();
				state->cullMode = CULL_FRONT;
				state->light = lightDir;
				state->shader = terrainShader;
				
				render->setShaderFloat(terrainShader, "isReflect", 1.0);
				render->setShaderFloat(terrainShader, "waterHeight", scene->water->position.y);
				render->useTexture(TEXTURE_2D_ARRAY, 0, AssetManager::assetManager->texArray->setId);
				render->draw(scene->reflectCamera, scene->terrainNode->drawcall, state);
				render->setShaderFloat(terrainShader, "isReflect", 0.0);
			}
		}
	}
}

void RenderManager::drawDeferred(Render* render, Scene* scene, FrameBuffer* screenBuff, Filter* filter) {
	static Shader* deferredShader = render->findShader("deferred");
	state->reset();
	state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = DEFERRED_PASS;
	state->shader = deferredShader;
	state->shadow = shadow;
	state->light = lightDir;
	state->time = time;

	uint baseSlot = screenBuff->colorBuffers.size() + 1; // Color buffers + Depth buffer
	render->useTexture(TEXTURE_2D, baseSlot, nearBuffer->getDepthBuffer()->id);
	render->useTexture(TEXTURE_2D, baseSlot + 1, midBuffer->getDepthBuffer()->id);
	render->useTexture(TEXTURE_2D, baseSlot + 2, farBuffer->getDepthBuffer()->id);
	filter->draw(scene->mainCamera, render, state, screenBuff->colorBuffers, screenBuff->depthBuffer);
}

void RenderManager::drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, FrameBuffer* inputBuff, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	state->reset();
	state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;

	filter->draw(scene->mainCamera, render, state, inputBuff->colorBuffers, NULL);
}

void RenderManager::drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	state->reset();
	state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;

	filter->draw(scene->mainCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawSSRFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	state->reset();
	state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;
	state->ssrPass = true;

	filter->draw(scene->mainCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawTexture2Screen(Render* render, Scene* scene, uint texid) {
	static Shader* screenShader = render->findShader("screen");
	state->reset();
	state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = screenShader;

	if (!scene->textureNode) {
		Board* board = new Board(2, 2, 2);
		scene->textureNode = new StaticNode(VECTOR3D(0, 0, 0));
		scene->textureNode->setFullStatic(true);
		StaticObject* boardObject = new StaticObject(board);
		scene->textureNode->addObject(boardObject);
		scene->textureNode->prepareDrawcall();
		delete board;
	}

	render->setFrameBuffer(NULL);
	render->useTexture(TEXTURE_2D, 0, texid);
	render->draw(scene->mainCamera, scene->textureNode->drawcall, state);
}

void RenderManager::drawBoundings(Render* render, RenderState* state, Scene* scene, Camera* camera) {
	for (unsigned int i = 0; i<scene->boundingNodes.size(); i++) {
		Node* node = scene->boundingNodes[i];
		render->draw(camera, node->drawcall, state);
	}
}

void RenderManager::enableShadow(Render* render) {
	static Shader* deferredShader = render->findShader("deferred");
	useShadow = true;
	render->setShaderInt(deferredShader, "useShadow", 1);
}

void RenderManager::disableShadow(Render* render) {
	static Shader* deferredShader = render->findShader("deferred");
	useShadow = false;
	render->setShaderInt(deferredShader, "useShadow", 0);
}

void RenderManager::showBounding() {
	drawBounding = true;
}

void RenderManager::hideBounding() {
	drawBounding = false;
}

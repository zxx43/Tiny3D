#include "renderManager.h"
#include "../assets/assetManager.h"

RenderManager::RenderManager(Camera* view, float distance1, float distance2, VECTOR3D light) {
	shadow=new Shadow(view,distance1,distance2);
	float nearSize=1024;
	float midSize=2048;
	float farSize=2048;
	nearBuffer=new FrameBuffer(nearSize,nearSize,false,HIGH_PRE);
	midBuffer=new FrameBuffer(midSize,midSize,false,HIGH_PRE);
	farBuffer=new FrameBuffer(farSize,farSize,false,LOW_PRE);
	lightDir=light;

	filterNear=new Filter(nearSize,nearSize,true,LOW_PRE);

	renderData = new Renderable(); renderData->copyCamera(view);
	queue1 = new Renderable(); queue1->copyCamera(view);
	queue2 = new Renderable(); queue2->copyCamera(view);
	currentQueue = queue1;
	nextQueue = queue2;

	useShadow = false;
	drawBounding = true;
	state = new RenderState();

	phongShadow = NULL;
	phongShadowLow = NULL;
	phong = NULL;
	phongShadowIns = NULL;
	phongShadowLowIns = NULL;
	phongIns = NULL;
	boneShadow = NULL;
	bone = NULL;
	mix = NULL;
	skyCube = NULL;
}

RenderManager::~RenderManager() {
	delete shadow; shadow=NULL;
	delete nearBuffer; nearBuffer=NULL;
	delete midBuffer; midBuffer=NULL;
	delete farBuffer; farBuffer=NULL;
	delete filterNear; filterNear=NULL;

	delete renderData; renderData = NULL;
	delete queue1; queue1 = NULL;
	delete queue2; queue2 = NULL;

	delete state; state = NULL;
}

void RenderManager::updateShadowCamera() {
	shadow->prepareViewCamera();
}

void RenderManager::updateMainLight(VECTOR3D light) {
	lightDir=light;
	shadow->update(light.x,light.y,light.z);
}

void RenderManager::flushRenderQueues() {
	renderData->flush();
}

void RenderManager::updateRenderQueues(Scene* scene) {
	Camera* cameraNear = shadow->lightCameraNear;
	Camera* cameraMid = shadow->lightCameraMid;
	Camera* cameraFar = shadow->lightCameraFar;
	Camera* cameraMain = scene->mainCamera;

	pushNodeToQueue(renderData->shadowNearStaticQueue, scene->staticRoot, cameraNear, false);
	pushNodeToQueue(renderData->shadowMidStaticQueue, scene->staticRoot, cameraMid, false);
	pushNodeToQueue(renderData->shadowFarStaticQueue, scene->staticRoot, cameraFar, true);
	pushNodeToQueue(renderData->staticQueue, scene->staticRoot, cameraMain, true);
	pushNodeToQueue(renderData->shadowNearAnimateQueue, scene->animationRoot, cameraNear, false);
	pushNodeToQueue(renderData->shadowMidAnimateQueue, scene->animationRoot, cameraMid, false);
	pushNodeToQueue(renderData->shadowFarAnimateQueue, scene->animationRoot, cameraFar, true);
	pushNodeToQueue(renderData->animateQueue, scene->animationRoot, cameraMain, true);
}

void RenderManager::animateQueues(long startTime, long currentTime) {
	currentQueue->shadowNearAnimateQueue->animate(startTime, currentTime);
	currentQueue->shadowMidAnimateQueue->animate(startTime, currentTime);
	currentQueue->shadowFarAnimateQueue->animate(startTime, currentTime);
	currentQueue->animateQueue->animate(startTime, currentTime);
}

void RenderManager::swapRenderQueues(Scene* scene) {
	currentQueue = (currentQueue == queue1) ? queue2 : queue1;
	nextQueue = (nextQueue == queue1) ? queue2 : queue1;

	//nextQueue->copyCamera(scene->mainCamera); // No exchange to avoid delay
	flushRenderQueues();
	updateRenderQueues(scene);
	nextQueue->flush();
	nextQueue->copyData(renderData);
}

void RenderManager::renderShadow(Render* render, Scene* scene) {
	if (!useShadow) return;

	state->reset();
	state->cullMode = CULL_FRONT;
	state->shadowPass = true;
	state->enableAlphaTest = true;
	state->alphaThreshold = 0.0;
	state->alphaTestMode = GREATER;

	if (!phongShadow) phongShadow = render->findShader("phong_s");
	if (!phongShadowLow) phongShadowLow = render->findShader("phong_sl");
	if (!phongShadowIns) phongShadowIns = render->findShader("phong_s_ins");
	if (!phongShadowLowIns) phongShadowLowIns = render->findShader("phong_sl_ins");
	if (!boneShadow) boneShadow = render->findShader("bone_s");

	render->useTexture(TEXTURE_2D_ARRAY, 0, assetManager->textures->setId);

	render->setFrameBuffer(nearBuffer);
	Camera* cameraNear=shadow->lightCameraNear;
	state->shader = phongShadow;
	state->shaderIns = phongShadowIns;
	currentQueue->shadowNearStaticQueue->draw(cameraNear, render, state);
	state->shader = boneShadow;
	currentQueue->shadowNearAnimateQueue->draw(cameraNear, render, state);

	render->setFrameBuffer(midBuffer);
	Camera* cameraMid=shadow->lightCameraMid;
	state->shader = phongShadow;
	currentQueue->shadowMidStaticQueue->draw(cameraMid, render, state);
	state->shader = boneShadow;
	currentQueue->shadowMidAnimateQueue->draw(cameraMid, render, state);

	static ushort flushCount = 1;
	if (flushCount % 2 == 0) 
		flushCount = 1;
	else {
		render->setFrameBuffer(farBuffer);
		Camera* cameraFar = shadow->lightCameraFar;
		state->shader = phongShadowLow;
		state->shaderIns = phongShadowLowIns;
		currentQueue->shadowFarStaticQueue->draw(cameraFar, render, state);
		state->shader = boneShadow;
		currentQueue->shadowFarAnimateQueue->draw(cameraFar, render, state);
		
		flushCount++;
	}
}

void RenderManager::renderScene(Render* render, Scene* scene) {
	state->reset();
	state->shadow = shadow;
	state->light = lightDir;

	if (!phong) phong = render->findShader("phong");
	if (!phongIns) phongIns = render->findShader("phong_ins");
	if (!bone) bone = render->findShader("bone");
	if (!mix) mix = render->findShader("terrain");
	if (!skyCube) skyCube = render->findShader("sky");

	//Camera* camera=currentQueue->mainCamera; // Exchange camera will lead delay
	Camera* camera = scene->mainCamera;
	render->useTexture(TEXTURE_2D_ARRAY, 0, assetManager->textures->setId);
	render->useTexture(TEXTURE_2D, 1, nearBuffer->getColorBuffer(0)->id);
	render->useTexture(TEXTURE_2D, 2, midBuffer->getColorBuffer(0)->id);
	render->useTexture(TEXTURE_2D, 3, farBuffer->getColorBuffer(0)->id);
	state->shader = phong;
	state->shaderIns = phongIns;
	currentQueue->staticQueue->draw(camera, render, state);

	// Draw terrain
	if (scene->terrainNode) {
		state->shader = mix;
		render->draw(camera, scene->terrainNode->drawcall, state);
	}

	state->shader = bone;
	currentQueue->animateQueue->draw(camera, render, state);

	// Debug mode
	if (drawBounding) {
		scene->clearAllAABB();
		scene->createNodeAABB(scene->staticRoot);
		scene->createNodeAABB(scene->animationRoot);

		state->enableCull = false;
		state->drawLine = true;
		state->shader = phong;
		drawBoundings(render, state, scene, camera);
	}

	// Draw sky
	if (scene->skyBox) 
		scene->skyBox->draw(render, skyCube, camera);

	render->finishDraw();
}

void RenderManager::drawBoundings(Render* render, RenderState* state, Scene* scene, Camera* camera) {
	for (unsigned int i = 0; i<scene->boundingNodes.size(); i++) {
		Node* node = scene->boundingNodes[i];
		render->draw(camera, node->drawcall, state);
	}
}

void RenderManager::enableShadow(Render* render) {
	if (!phong) phong = render->findShader("phong");
	if (!phongIns) phongIns = render->findShader("phong_ins");
	if (!mix) mix = render->findShader("terrain");

	useShadow = true;
	render->useShader(phong);
	phong->setInt("useShadow", 1);
	render->useShader(phongIns);
	phongIns->setInt("useShadow", 1);
	render->useShader(mix);
	mix->setInt("useShadow", 1);
}

void RenderManager::disableShadow(Render* render) {
	if (!phong) phong = render->findShader("phong");
	if (!phongIns) phongIns = render->findShader("phong_ins");
	if (!mix) mix = render->findShader("terrain");

	useShadow = false;
	render->useShader(phong);
	phong->setInt("useShadow", 0);
	render->useShader(phongIns);
	phongIns->setInt("useShadow", 0);
	render->useShader(mix);
	mix->setInt("useShadow", 0);
}

void RenderManager::showBounding() {
	drawBounding = true;
}

void RenderManager::hideBounding() {
	drawBounding = false;
}


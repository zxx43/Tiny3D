#include "renderManager.h"
#include "../assets/assetManager.h"

RenderManager::RenderManager(Camera* view, float distance1, float distance2, VECTOR3D light) {
	shadow=new Shadow(view,distance1,distance2);
	float nearSize=512;
	float midSize=1024;
	float farSize=2048;
	nearBuffer=new FrameBuffer(nearSize,nearSize,false);
	midBuffer=new FrameBuffer(midSize,midSize,false);
	farBuffer=new FrameBuffer(farSize,farSize,false);
	lightDir=light;

	filterNear=new Filter(nearSize,nearSize,true);

	renderData = new Renderable(); renderData->copyCamera(view);
	queue1 = new Renderable(); queue1->copyCamera(view);
	queue2 = new Renderable(); queue2->copyCamera(view);
	currentQueue = queue1;
	nextQueue = queue2;

	useShadow = false;
	drawBounding = true;
	state = new RenderState();
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

	pushNodeToQueue(renderData->shadowNearStaticQueue, scene->staticRoot, cameraNear);
	pushNodeToQueue(renderData->shadowMidStaticQueue, scene->staticRoot, cameraMid);
	pushNodeToQueue(renderData->shadowFarStaticQueue, scene->staticRoot, cameraFar);
	pushNodeToQueue(renderData->staticQueue, scene->staticRoot, cameraMain);
	pushNodeToQueue(renderData->shadowNearAnimateQueue, scene->animationRoot, cameraNear);
	pushNodeToQueue(renderData->shadowMidAnimateQueue, scene->animationRoot, cameraMid);
	pushNodeToQueue(renderData->shadowFarAnimateQueue, scene->animationRoot, cameraFar);
	pushNodeToQueue(renderData->animateQueue, scene->animationRoot, cameraMain);
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
	Shader* phong = render->findShader("phong");
	Shader* bone = render->findShader("bone");

	render->setFrameBuffer(nearBuffer);
	Camera* cameraNear=shadow->lightCameraNear;
	state->shader = phong;
	currentQueue->shadowNearStaticQueue->draw(cameraNear, render, state);
	state->shader = bone;
	currentQueue->shadowNearAnimateQueue->draw(cameraNear, render, state);

	render->setFrameBuffer(midBuffer);
	Camera* cameraMid=shadow->lightCameraMid;
	state->shader = phong;
	currentQueue->shadowMidStaticQueue->draw(cameraMid, render, state);
	state->shader = bone;
	currentQueue->shadowMidAnimateQueue->draw(cameraMid, render, state);

	render->setFrameBuffer(farBuffer);
	Camera* cameraFar=shadow->lightCameraFar;
	state->shader = phong;
	currentQueue->shadowFarStaticQueue->draw(cameraFar, render, state);
	state->shader = bone;
	currentQueue->shadowFarAnimateQueue->draw(cameraFar, render, state);
}

void RenderManager::renderScene(Render* render, Scene* scene) {
	state->reset();
	state->shadow = shadow;
	state->light = lightDir;

	Shader* phong = render->findShader("phong");
	Shader* bone = render->findShader("bone");
	Shader* terrain = render->findShader("terrain");

	//Camera* camera=currentQueue->mainCamera; // Exchange camera will lead delay
	Camera* camera = scene->mainCamera;
	assetManager->textures->use(0);
	nearBuffer->getColorBuffer(0)->use(1);
	midBuffer->getColorBuffer(0)->use(2);
	farBuffer->getColorBuffer(0)->use(3);
	state->shader = phong;
	currentQueue->staticQueue->draw(camera, render, state);

	// Draw terrain
	if (scene->terrainNode) {
		state->shader = terrain;
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
	if (scene->skyBox) {
		Shader* sky = render->findShader("sky");
		scene->skyBox->draw(render, sky, camera);
	}

	render->finishDraw();
}

void RenderManager::drawBoundings(Render* render, RenderState* state, Scene* scene, Camera* camera) {
	for (unsigned int i = 0; i<scene->boundingNodes.size(); i++) {
		Node* node = scene->boundingNodes[i];
		render->draw(camera, node->drawcall, state);
	}
}

void RenderManager::enableShadow(Render* render) {
	useShadow = true;
	Shader* phong = render->findShader("phong");
	render->useShader(phong);
	phong->setInt("useShadow", 1);
	Shader* terrain = render->findShader("terrain");
	render->useShader(terrain);
	terrain->setInt("useShadow", 1);
}

void RenderManager::disableShadow(Render* render) {
	useShadow = false;
	Shader* phong = render->findShader("phong");
	render->useShader(phong);
	phong->setInt("useShadow", 0);
	Shader* terrain = render->findShader("terrain");
	render->useShader(terrain);
	terrain->setInt("useShadow", 0);
}

void RenderManager::showBounding() {
	drawBounding = true;
}

void RenderManager::hideBounding() {
	drawBounding = false;
}


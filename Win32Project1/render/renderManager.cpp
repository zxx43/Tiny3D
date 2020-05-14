#include "renderManager.h"
#include "../assets/assetManager.h"
#include "../mesh/board.h"
#include "../object/staticObject.h"

RenderManager::RenderManager(ConfigArg* cfg, Camera* view, float distance1, float distance2, const vec3& light) {
	int precision = LOW_PRE;
	cfgs = cfg;
	shadow = new Shadow(view);
	float nearSize = 1024;
	float midSize = 1024;
	float farSize = 512;
	if (cfgs->shadowQuality > 2) {
		nearSize = 4096;
		midSize = 1024;
		farSize = 512;
		precision = HIGH_PRE;
	} else if (cfgs->shadowQuality > 1) {
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

	queue1 = new Renderable(distance1, distance2, cfgs);
	queue2 = new Renderable(distance1, distance2, cfgs);
	currentQueue = queue1;
	nextQueue = queue2;

	state = new RenderState();

	reflectBuffer = NULL;
	occluderDepth = NULL;
	needResize = true;
	updateSky();

	grassDrawcall = NULL;
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
	if (occluderDepth) delete occluderDepth; occluderDepth = NULL;
	if (grassDrawcall) delete grassDrawcall; grassDrawcall = NULL;
}

void RenderManager::resize(float width, float height) {
	if (reflectBuffer) delete reflectBuffer; reflectBuffer = NULL;
	if (!cfgs->ssr) {
		reflectBuffer = new FrameBuffer(width, height, LOW_PRE, 4, false);
		reflectBuffer->addColorBuffer(LOW_PRE, 4);
		reflectBuffer->addColorBuffer(LOW_PRE, 3);
		reflectBuffer->attachDepthBuffer(LOW_PRE);
	}

	if (occluderDepth) delete occluderDepth;
	occluderDepth = new Texture2D(width, height, TEXTURE_TYPE_DEPTH, LOW_PRE, 1);
	needResize = true;
	updateSky();
}

void RenderManager::updateShadowCamera(Camera* mainCamera) {
	shadow->prepareViewCamera(mainCamera->zFar * 0.25, mainCamera->zFar * 0.75);
}

void RenderManager::updateMainLight() {
	shadow->update(lightDir);
}

void RenderManager::updateSky() { 
	needRefreshSky = true; 
	udotl = vec3(0.0, 1.0, 0.0).DotProduct(-lightDir);
	if (udotl < 0.0) udotl = 0.0;
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

void RenderManager::animateQueues(float velocity) {
	currentQueue->queues[QUEUE_ANIMATE_SN]->animate(velocity);
	currentQueue->queues[QUEUE_ANIMATE_SM]->animate(velocity);
	currentQueue->queues[QUEUE_ANIMATE_SF]->animate(velocity);
	currentQueue->queues[QUEUE_ANIMATE]->animate(velocity);
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
	if (cfgs->shadowQuality < 1) return;

	static Shader* phongShadowShader = render->findShader("phong_s");
	static Shader* phongShadowInsShader = render->findShader("phong_s_ins");
	static Shader* boneShadowShader = render->findShader("bone_s");
	static Shader* phongShadowLowShader = render->findShader("phong_sl");
	static Shader* billShadowInsShader = render->findShader("bill_s_ins");
	static Shader* billShadowLowShader = render->findShader("bill_sl_ins");
	static Shader* cullShader = render->findShader("cull");
	static Shader* multiShader = render->findShader("multi_s");
	static Shader* flushShader = render->findShader("flush");
	static Shader* animMultiShader = render->findShader("animMulti_s");
	static Shader* animFlushShader = render->findShader("animFlush");

	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	state->cullMode = CULL_FRONT;
	state->light = lightDir;
	state->udotl = udotl;
	state->time = scene->time;

	Camera* mainCamera = scene->mainCamera;

	render->setFrameBuffer(nearBuffer);
	Camera* cameraNear=shadow->lightCameraNear;
	state->pass = NEAR_SHADOW_PASS;
	state->shader = phongShadowShader;
	state->shaderIns = phongShadowInsShader;
	state->shaderBill = billShadowInsShader;
	state->shaderCompute = cullShader;
	state->shaderMulti = multiShader;
	state->shaderFlush = flushShader;
	currentQueue->queues[QUEUE_STATIC_SN]->draw(scene, cameraNear, render, state);
	state->shader = boneShadowShader;
	state->shaderMulti = animMultiShader;
	state->shaderFlush = animFlushShader;
	currentQueue->queues[QUEUE_ANIMATE_SN]->draw(scene, cameraNear, render, state);

	render->setFrameBuffer(midBuffer);
	Camera* cameraMid=shadow->lightCameraMid;
	state->pass = MID_SHADOW_PASS;
	state->shader = phongShadowShader;
	state->shaderMulti = multiShader;
	state->shaderFlush = flushShader;
	currentQueue->queues[QUEUE_STATIC_SM]->draw(scene, cameraMid, render, state);
	state->shader = boneShadowShader;
	state->shaderMulti = animMultiShader;
	state->shaderFlush = animFlushShader;
	currentQueue->queues[QUEUE_ANIMATE_SM]->draw(scene, cameraMid, render, state);

	///*
	static ushort flushCount = 1;
	static bool flushed = false;
	if (!flushed) {
		if (flushCount % 3 == 0)
			flushCount = 1;
		else {
			if (flushCount % 3 == 1) {
				render->setFrameBuffer(farBuffer);
				if (true) flushed = true;
				else {
					Camera* cameraFar = shadow->lightCameraFar;
					state->pass = FAR_SHADOW_PASS;
					state->shader = phongShadowLowShader;
					state->shaderIns = phongShadowInsShader;
					state->shaderBill = billShadowLowShader;
					state->shaderMulti = multiShader;
					state->shaderFlush = flushShader;
					currentQueue->queues[QUEUE_STATIC_SF]->draw(scene, cameraFar, render, state);
					state->shader = boneShadowShader;
					state->shaderMulti = animMultiShader;
					state->shaderFlush = animFlushShader;
					currentQueue->queues[QUEUE_ANIMATE_SF]->draw(scene, cameraFar, render, state);
				}
			}
			flushCount++;
		}
	}
	//*/
}

void RenderManager::drawGrass(Render* render, RenderState* state, Scene* scene, Camera* camera) {
	bool computeGrass = true;
	state->enableCull = false;
	state->grassPass = true;
	TerrainNode* node = scene->terrainNode;
	Terrain* mesh = node->getMesh();
	if (computeGrass) {
		if (!grassDrawcall) {
			BufferData terrainData(mesh->visualPoints, mesh->visualPointsSize, 4, 4);
			static int grassCount = cfgs->graphQuality > 5 ? 1024 : 512;
			grassDrawcall = new ComputeDrawcall(&terrainData, grassCount);
		}
		static Shader* grassShader = render->findShader("grass");
		Shader* compShader = render->findShader("grassComp");

		state->shader = grassShader;
		state->shaderCompute = compShader;
		state->eyePos = &(camera->position);

		StaticObject* terrain = (StaticObject*)node->objects[0];
		compShader->setVector3v("translate", terrain->transformMatrix.entries + 12);
		compShader->setVector3v("scale", terrain->size);
		compShader->setVector4("mapInfo", STEP_SIZE, node->lineSize, MAP_SIZE, MAP_SIZE);

		grassDrawcall->update();
		render->draw(camera, grassDrawcall, state);
	} else {
		static Shader* grassLayerShader = render->findShader("grassLayer");
		state->shader = grassLayerShader;
		state->tess = true;
		((StaticDrawcall*)node->drawcall)->updateBuffers(state->pass, mesh->visualIndices, mesh->visualIndCount);
		render->draw(camera, node->drawcall, state);
		state->tess = false;
	}
	state->enableCull = true;
	state->grassPass = false;
}

void RenderManager::renderScene(Render* render, Scene* scene) {
	static Shader* phongShader = render->findShader("phong");
	static Shader* phongInsShader = render->findShader("phong_ins");
	static Shader* billInsShader = render->findShader("bill_ins");
	static Shader* boneShader = render->findShader("bone");
	static Shader* skyShader = render->findShader("sky");
	static Shader* cullShader = render->findShader("cull");
	static Shader* multiShader = render->findShader("multi");
	static Shader* flushShader = render->findShader("flush");
	static Shader* animMultiShader = render->findShader("animMulti");
	static Shader* animFlushShader = render->findShader("animFlush");

	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	state->light = lightDir;
	state->udotl = udotl;
	state->time = scene->time;
	state->enableSsr = cfgs->ssr;
	state->quality = cfgs->graphQuality;
	state->dynSky = cfgs->dynsky;

	Camera* camera = scene->mainCamera;

	// Draw terrain & grass
	TerrainNode* terrainNode = scene->terrainNode;
	if (terrainNode && terrainNode->checkInCamera(camera)) {
		static Shader* terrainShader = render->findShader("terrain");
		state->shader = terrainShader;
		((StaticDrawcall*)terrainNode->drawcall)->updateBuffers(state->pass);

		StaticObject* terrain = (StaticObject*)terrainNode->objects[0];
		terrainShader->setVector3v("translate", terrain->transformMatrix.entries + 12);
		terrainShader->setVector3v("scale", terrain->size);
		terrainShader->setVector4("mapInfo", STEP_SIZE, terrainNode->lineSize, MAP_SIZE, MAP_SIZE);
		terrainShader->setHandle64("roadTex", AssetManager::assetManager->getRoadHnd());
		render->draw(camera, terrainNode->drawcall, state);

		occluderDepth->copyDataFrom(render->getFrameBuffer()->getDepthBuffer());

		if (!cfgs->cartoon && !cfgs->debug) {
			render->useTexture(TEXTURE_2D, 0, occluderDepth->id);
			drawGrass(render, state, scene, camera);
		}

		state->mapTrans.x = terrain->transformMatrix.entries[12];
		state->mapTrans.y = terrain->transformMatrix.entries[13];
		state->mapTrans.z = terrain->transformMatrix.entries[14];
		state->mapScl = terrain->size;
		state->mapInfo = vec4(STEP_SIZE, terrainNode->lineSize, MAP_SIZE, MAP_SIZE);
	}

	state->shader = phongShader;
	state->shaderIns = phongInsShader;
	state->shaderBill = billInsShader;
	state->shaderCompute = cullShader;
	state->shaderMulti = multiShader;
	state->shaderFlush = flushShader;
	render->useTexture(TEXTURE_2D, 0, occluderDepth->id);
	currentQueue->queues[QUEUE_STATIC]->draw(scene, camera, render, state);

	state->shader = boneShader;
	state->shaderMulti = animMultiShader;
	state->shaderFlush = animFlushShader;
	currentQueue->queues[QUEUE_ANIMATE]->draw(scene, camera, render, state);

	// Draw sky
	if (scene->skyBox)
		scene->skyBox->draw(render, skyShader, camera);

	// Debug mode
	if (cfgs->debug) {
		static bool boxInit = false;
		if (!boxInit && scene->isInited()) {
			scene->clearAllAABB();
			scene->createNodeAABB(scene->staticRoot);
			scene->createNodeAABB(scene->animationRoot);
			boxInit = true;
		}

		static Shader* debugShader = render->findShader("debug");
		state->enableCull = false;
		state->drawLine = true;
		state->enableAlphaTest = false;
		state->shader = debugShader;
		drawBoundings(render, state, scene, camera);

		static int frame = 0;
		if (frame % 40 == 0) boxInit = false;
		frame++;
	}

	scene->flushNodes();
	if (needResize) needResize = false;
}

void RenderManager::renderSkyTex(Render* render, Scene* scene) {
	if (!scene->skyBox) return;
	if (needRefreshSky) {
		static Shader* atmoShader = render->findShader("atmos");
		scene->skyBox->state->time = scene->time;
		scene->skyBox->state->udotl = udotl;
		scene->skyBox->update(render, lightDir, atmoShader);
		needRefreshSky = false;
	}
}

void RenderManager::renderWater(Render* render, Scene* scene) {
	static Shader* waterShader = render->findShader("water");
	Camera* camera = scene->mainCamera;
	if (scene->water && scene->water->checkInCamera(camera)) {
		state->reset();
		state->eyePos = &(scene->mainCamera->position);
		state->light = lightDir;
		state->udotl = udotl;
		state->time = scene->time;
		state->enableSsr = cfgs->ssr;
		state->waterPass = true;
		state->shader = waterShader;

		waterShader->setVector2("size", scene->water->size.x * 0.5, scene->water->size.y * 0.5);
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
				state->udotl = udotl;
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
	state->udotl = udotl;
	state->time = scene->time;
	state->quality = cfgs->graphQuality;
	state->dynSky = cfgs->dynsky;

	uint baseSlot = screenBuff->colorBuffers.size() + 1; // Color buffers + Depth buffer
	if(!deferredShader->isTexBinded(nearBuffer->getDepthBuffer()->hnd))
		deferredShader->setHandle64("depthBufferNear", nearBuffer->getDepthBuffer()->hnd);
	if(!deferredShader->isTexBinded(midBuffer->getDepthBuffer()->hnd))
		deferredShader->setHandle64("depthBufferMid", midBuffer->getDepthBuffer()->hnd);
	if(!deferredShader->isTexBinded(farBuffer->getDepthBuffer()->hnd))
		deferredShader->setHandle64("depthBufferFar", farBuffer->getDepthBuffer()->hnd);
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
	state->quality = cfgs->graphQuality;
	state->dynSky = cfgs->dynsky;
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

void RenderManager::drawDualFilter(Render* render, Scene* scene, const char* shaderStr, DualFilter* filter) {
	Shader* shader = render->findShader(shaderStr);
	state->reset();
	state->eyePos = &(scene->mainCamera->position);
	//state->enableCull = false;
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;

	shader->setFloat("pass", 1.0);
	drawScreenFilter(render, scene, "gauss", filter->getInput1(), filter->getOutput1());
	shader->setFloat("pass", 2.0);
	drawScreenFilter(render, scene, "gauss", filter->getInput2(), filter->getOutput2());
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

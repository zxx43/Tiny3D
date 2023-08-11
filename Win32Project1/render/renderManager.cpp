#include "renderManager.h"
#include "../assets/assetManager.h"
#include "../mesh/board.h"
#include "../object/staticObject.h"

RenderManager::RenderManager(ConfigArg* cfg, Scene* scene, float distance1, float distance2, const vec3& light) {
	depthPre = LOW_PRE;
	cfgs = cfg;
	float nearSize = 1024;
	float midSize = 1024;
	float farSize = 2;
	if (cfgs->shadowQuality > 5) {
		nearSize = 4096;
		midSize = 4096;
		farSize = 4096;
		depthPre = FLOAT_PRE;
	} else if (cfgs->shadowQuality > 4) {
		nearSize = 4096;
		midSize = 4096;
		depthPre = FLOAT_PRE;
	} else if (cfgs->shadowQuality > 3) {
		nearSize = 2048;
		midSize = 2048;
		depthPre = FLOAT_PRE;
	} else if (cfgs->shadowQuality > 2) {
		nearSize = 2048;
		midSize = 2048;
		depthPre = HIGH_PRE;
	} else if (cfgs->shadowQuality > 1) {
		nearSize = 1024;
		midSize = 1024;
		depthPre = HIGH_PRE;
	}
	shadow = new Shadow(scene->actCamera, nearSize, midSize, farSize);
	shadow->shadowMapSize = nearSize;
	shadow->shadowPixSize = 0.5 / nearSize;
	shadow->pixSize = 1.0 / nearSize;

	nearDynamicBuffer = new FrameBuffer(nearSize, nearSize, depthPre);
	nearStaticBuffer = new FrameBuffer(nearSize, nearSize, depthPre);
	midBuffer = new FrameBuffer(midSize, midSize, depthPre);
	farBuffer = new FrameBuffer(farSize, farSize, depthPre);

	lightDir = light.GetNormalized();
	queue1 = new Renderable(distance1, distance2, cfgs);
	queue2 = new Renderable(distance1, distance2, cfgs);
	currentQueue = queue1;
	nextQueue = queue2;
	renderData = NULL;

	state = new RenderState();

	reflectBuffer = NULL;
	needResize = true;
	updateSky();

	actShowWater = false;
	renderShowWater = false;

	grassDrawcall = NULL;
	hiz = new HizGenerator();
	prevCameraMat.LoadIdentity();
	hizDepth = NULL;
	ibl = NULL;

	lodParam.lodDist.x = distance1;
	lodParam.lodDist.y = distance2;

	clearRender = false, resetRender = false, resetGather = false;

	oit = new Oit();
}

RenderManager::~RenderManager() {
	delete shadow; shadow = NULL;
	delete nearDynamicBuffer; nearDynamicBuffer = NULL;
	delete nearStaticBuffer; nearStaticBuffer = NULL;
	delete midBuffer; midBuffer = NULL;
	delete farBuffer; farBuffer = NULL;

	delete queue1; queue1 = NULL;
	delete queue2; queue2 = NULL;

	delete state; state = NULL;
	if (reflectBuffer) delete reflectBuffer; reflectBuffer = NULL;
	if (grassDrawcall) delete grassDrawcall; grassDrawcall = NULL;
	if (hiz) delete hiz; hiz = NULL;
	if (hizDepth) delete hizDepth; hizDepth = NULL;
	if (ibl) delete ibl; ibl = NULL;
	if (oit) delete oit; oit = NULL;
}

void RenderManager::resize(float width, float height) {
	if (reflectBuffer) delete reflectBuffer; reflectBuffer = NULL;
	if (!cfgs->ssr) {
		reflectBuffer = new FrameBuffer(width, height, LOW_PRE, 4, WRAP_REPEAT);
		reflectBuffer->addColorBuffer(LOW_PRE, 4);
		reflectBuffer->addColorBuffer(LOW_PRE, 3);
		reflectBuffer->attachDepthBuffer(LOW_PRE, false);
	}

	if (hizDepth) delete hizDepth;
	hizDepth = new Texture2D(width, height, true, TEXTURE_TYPE_DEPTH, depthPre, 1, NEAREST, WRAP_CLAMP_TO_EDGE);
	needResize = true;
	updateSky();

	oit->resize(width, height);
}

void RenderManager::updateShadowCamera(Camera* mainCamera) {
	if (cfgs->shadowQuality > 2)
		shadow->prepareViewCamera(mainCamera->zFar * 0.1, mainCamera->zFar * 0.7);
	else
		shadow->prepareViewCamera(mainCamera->zFar * 0.05, mainCamera->zFar * 0.5);
}

void RenderManager::updateMainLight(Scene* scene) {
	shadow->update(scene->actCamera, lightDir);
}

void RenderManager::updateWaterVisible(const Scene* scene) {
	actShowWater = scene->water && scene->water->checkInCamera(scene->actCamera);
}

void RenderManager::updateSky() { 
	needRefreshSky = true; 
	udotl = vec3(0.0, 1.0, 0.0).DotProduct(-lightDir);
	if (udotl < 0.0) udotl = 0.0;
}

void RenderManager::flushRenderQueues(Scene* scene) {
	if (renderData) renderData->flush(scene);
}

void RenderManager::updateRenderQueues(Scene* scene) {
	if (!renderData) return;

	Camera* cameraDyn = shadow->actLightCameraDyn;
	Camera* cameraMid = shadow->actLightCameraMid;
	Camera* cameraFar = shadow->actLightCameraFar;
	Camera* cameraMain = scene->actCamera;

	if (renderData->queues[QUEUE_DYN_SNEAR]->isObjGatherPrepared())
		PushNodeToQueue(renderData->queues[QUEUE_DYN_SNEAR], scene, scene->root, cameraDyn, cameraMain);
	
	if (renderData->queues[QUEUE_DYN_SMID]->isObjGatherPrepared())
		PushNodeToQueue(renderData->queues[QUEUE_DYN_SMID], scene, scene->root, cameraMid, cameraMain);
	
	if (cfgs->shadowQuality > 5 && renderData->queues[QUEUE_DYN_SFAR]->isObjGatherPrepared())
		PushNodeToQueue(renderData->queues[QUEUE_DYN_SFAR], scene, scene->root, cameraFar, cameraMain);
	
	if (renderData->queues[QUEUE_DYN_MAIN]->isObjGatherPrepared())
		PushNodeToQueue(renderData->queues[QUEUE_DYN_MAIN], scene, scene->root, cameraMain, cameraMain);

	if (!renderData->queues[QUEUE_STATIC]->staticDataReady() && renderData->queues[QUEUE_STATIC]->isObjGatherPrepared())
		InitNodeToQueue(renderData->queues[QUEUE_STATIC], scene, scene->root);

	if (cfgs->debug && scene->isInited() && renderData->queues[QUEUE_DEBUG]->isObjGatherPrepared())
		PushDebugToQueue(renderData->queues[QUEUE_DEBUG], scene, cameraMain);
}

void RenderManager::flushRenderQueueDatas(Scene* scene) {
	if (scene->staticDatasShouldFlush()) {
		if (queue1) ResetStaticQueueData(queue1->queues[QUEUE_STATIC], scene, scene->root);
		if (queue2) ResetStaticQueueData(queue2->queues[QUEUE_STATIC], scene, scene->root);
		scene->staticDatasFlushed();
	}
}

void RenderManager::swapRenderQueues(Scene* scene, bool swapQueue) {
	if (swapQueue) {
		currentQueue = (currentQueue == queue1) ? queue2 : queue1;
		nextQueue = (nextQueue == queue1) ? queue2 : queue1;
		renderData = nextQueue;
		scene->renderCamera->copy(scene->actCamera);
		shadow->copyCameraData();
	} else {
		currentQueue = queue1;
		renderData = currentQueue;
		if (scene->renderCamera != scene->actCamera) {
			delete scene->renderCamera;
			scene->renderCamera = scene->actCamera;
		}
		shadow->mergeCamera();
	}
	renderShowWater = actShowWater;
}

void RenderManager::prepareData(Scene* scene) {
	updateWaterVisible(scene);
	flushRenderQueues(scene);
	updateRenderQueues(scene);
	flushRenderQueueDatas(scene);
}

void RenderManager::clearGatherDatas(Scene* scene) {
	if (scene->isNeedAddObject()) {
		scene->releaseMeshGather();
		scene->releaseDebugGather();
		clearRender = true;
		scene->finAddObject();
	}
}

void RenderManager::clearRenderDatas(Scene* scene) {
	if (clearRender) {
		if (queue1) {
			queue1->queues[QUEUE_DYN_SNEAR]->clearRenderData();
			queue1->queues[QUEUE_DYN_SMID]->clearRenderData();
			queue1->queues[QUEUE_DYN_SFAR]->clearRenderData();
			queue1->queues[QUEUE_DYN_MAIN]->clearRenderData();
			queue1->queues[QUEUE_STATIC]->clearRenderData();
			queue1->queues[QUEUE_DEBUG]->clearRenderData();
		}
		if (queue2) {
			queue2->queues[QUEUE_DYN_SNEAR]->clearRenderData();
			queue2->queues[QUEUE_DYN_SMID]->clearRenderData();
			queue2->queues[QUEUE_DYN_SFAR]->clearRenderData();
			queue2->queues[QUEUE_DYN_MAIN]->clearRenderData();
			queue2->queues[QUEUE_STATIC]->clearRenderData();
			queue2->queues[QUEUE_DEBUG]->clearRenderData();
		}

		scene->releaseMeshBuffer();
		scene->releaseDebugBuffer();
		clearRender = false;
		resetGather = true;
	}
}

void RenderManager::resetGatherDatas(Scene* scene) {
	if (resetGather) {
		scene->createMeshGather();
		if (scene->boundingNodes.size() > 0) 
			scene->createDebugGather();
		resetGather = false;
		resetRender = true;
	}
}

void RenderManager::resetRenderDatas(Scene* scene) {
	if (resetRender) {
		if (queue1) {
			queue1->queues[QUEUE_DYN_SNEAR]->resetObjGatherFin();
			queue1->queues[QUEUE_DYN_SMID]->resetObjGatherFin();
			queue1->queues[QUEUE_DYN_SFAR]->resetObjGatherFin();
			queue1->queues[QUEUE_DYN_MAIN]->resetObjGatherFin();
			queue1->queues[QUEUE_STATIC]->resetObjGatherFin();
			queue1->queues[QUEUE_DEBUG]->resetObjGatherFin();
		}
		if (queue2) {
			queue2->queues[QUEUE_DYN_SNEAR]->resetObjGatherFin();
			queue2->queues[QUEUE_DYN_SMID]->resetObjGatherFin();
			queue2->queues[QUEUE_DYN_SFAR]->resetObjGatherFin();
			queue2->queues[QUEUE_DYN_MAIN]->resetObjGatherFin();
			queue2->queues[QUEUE_STATIC]->resetObjGatherFin();
			queue2->queues[QUEUE_DEBUG]->resetObjGatherFin();
		}
		resetRender = false;
	}
}

void RenderManager::updateDebugData(Scene* scene) {
	if (cfgs->debug && scene->isInited()) {
		scene->updateNodeAABB(scene->water);
		scene->updateNodeAABB(scene->terrainNode);
		scene->updateNodeAABB(scene->root);
	}
}

void RenderManager::finishInit(Scene* scene) {
	scene->createMeshGather();
}

void RenderManager::renderShadow(Render* render, Scene* scene) {
	if (cfgs->shadowQuality < 1) return;
	if (cfgs->debug) {
		render->setFrameBuffer(nearStaticBuffer);
		render->setFrameBuffer(nearDynamicBuffer);
		render->setFrameBuffer(midBuffer);
		render->setFrameBuffer(farBuffer);
		shadow->setFlushDyn(false);
		shadow->setFlushNear(false);
		shadow->setFlushMid(false);
		shadow->setFlushFar(false);
		return;
	}

	static Shader* phongShadowShader = render->findShader("phong_s");
	static Shader* phongShadowInsShader = render->findShader("phong_s_ins");
	static Shader* boneShadowShader = render->findShader("bone_s");
	static Shader* phongShadowLowShader = render->findShader("phong_sl");
	static Shader* billShadowInsShader = render->findShader("bill_s_ins");
	static Shader* billShadowLowShader = render->findShader("bill_sl_ins");

	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->cullMode = CULL_FRONT;
	state->light = lightDir;
	state->udotl = udotl;
	state->time = scene->time;

	const static ushort fln = 10, flf = 20;
	Camera* mainCamera = scene->renderCamera;

	state->pass = NEAR_SHADOW_PASS;
	state->shader = phongShadowShader;
	state->shaderIns = phongShadowInsShader;
	state->shaderBone = boneShadowShader;
	state->shaderBill = billShadowInsShader;
	state->shaderCompute = NULL;

	static ushort fn = fln;
	if (fn % fln != 0) {
		++fn;
		shadow->setFlushNear(false);
	} else {
		fn = 1;
		shadow->setFlushNear(true);
		render->setFrameBuffer(nearStaticBuffer);
		Camera* cameraNear = shadow->renderLightCameraNear;
		state->dynPass = false;
		lodParam.vpMat = cameraNear->viewProjectMatrix;
		lodParam.eyePos = mainCamera->position;
		lodParam.shadowPass = 1;
		currentQueue->queues[QUEUE_STATIC]->process(scene, render, state, lodParam);
		currentQueue->queues[QUEUE_STATIC]->draw(scene, cameraNear, render, state);
	}
	
	shadow->setFlushDyn(true);
	render->setFrameBuffer(nearDynamicBuffer);
	Camera* cameraDyn=shadow->renderLightCameraDyn;
	state->dynPass = true;
	lodParam.vpMat = cameraDyn->viewProjectMatrix;
	lodParam.eyePos = mainCamera->position;
	lodParam.shadowPass = 1;
	currentQueue->queues[QUEUE_DYN_SNEAR]->process(scene, render, state, lodParam);
	currentQueue->queues[QUEUE_DYN_SNEAR]->draw(scene, cameraDyn, render, state);

	static ushort fm = flf;
	if (fm % flf != 0) {
		++fm;
		shadow->setFlushMid(false);
	} else {
		fm = 1;
		shadow->setFlushMid(true);
		render->setFrameBuffer(midBuffer);
		Camera* cameraMid = shadow->renderLightCameraMid;
		state->pass = MID_SHADOW_PASS;
		lodParam.vpMat = cameraMid->viewProjectMatrix;
		lodParam.eyePos = mainCamera->position;
		lodParam.shadowPass = 1;
		currentQueue->queues[QUEUE_DYN_SMID]->process(scene, render, state, lodParam);
		currentQueue->queues[QUEUE_DYN_SMID]->draw(scene, cameraMid, render, state);

		currentQueue->queues[QUEUE_STATIC]->process(scene, render, state, lodParam);
		currentQueue->queues[QUEUE_STATIC]->draw(scene, cameraMid, render, state);
	}

	if (cfgs->shadowQuality > 5) {
		static ushort ff = flf;
		if (ff % flf != 0) {
			++ff;
			shadow->setFlushFar(false);
		} else {
			ff = 1;
			shadow->setFlushFar(true);
			render->setFrameBuffer(farBuffer);
			Camera* cameraFar = shadow->renderLightCameraFar;
			state->pass = FAR_SHADOW_PASS;
			lodParam.vpMat = cameraFar->viewProjectMatrix;
			lodParam.eyePos = mainCamera->position;
			lodParam.shadowPass = 1;
			currentQueue->queues[QUEUE_DYN_SFAR]->process(scene, render, state, lodParam);
			currentQueue->queues[QUEUE_DYN_SFAR]->draw(scene, cameraFar, render, state);

			currentQueue->queues[QUEUE_STATIC]->process(scene, render, state, lodParam);
			currentQueue->queues[QUEUE_STATIC]->draw(scene, cameraFar, render, state);
		}
	}
}

void RenderManager::drawGrass(Render* render, RenderState* state, Scene* scene, Camera* camera, const vec3& ref) {
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
		compShader->setFloat("uMaxLevel", hiz->getMaxLevel());
		compShader->setMatrix4("prevVPMatrix", prevCameraMat);
		compShader->setVector2("uSize", (float)render->viewWidth, (float)render->viewHeight);
		compShader->setVector2("uCamParam", camera->zNear, camera->zFar);
		compShader->setIVector2("refChunk", floor(ref.x), floor(ref.z));

		if (!node->getTerrainUniforms()) node->createTerrainInfo();
		node->useTerrainUniforms(3);
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

	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->light = lightDir;
	state->udotl = udotl;
	state->time = scene->time;
	state->enableSsr = cfgs->ssr;
	state->quality = cfgs->graphQuality;
	state->dynSky = cfgs->dynsky;

	Camera* camera = scene->renderCamera;

	// Draw terrain & grass
	TerrainNode* terrainNode = scene->terrainNode;
	if (terrainNode && terrainNode->checkInCamera(camera)) {
		static Shader* terrainShader = render->findShader("terrain");
		static Shader* debugTerrainShader = render->findShader("terrain_debug");
		static Shader* terrainCullShader = render->findShader("terrainComp");

		StaticObject* terrain = (StaticObject*)terrainNode->objects[0];
		if (!terrainNode->getTerrainUniforms()) terrainNode->createTerrainInfo();
		TerrainInfo* terrainInfo = terrainNode->getTerrainInfo();
		state->mapTrans = vec3(terrainInfo->trans.x, terrainInfo->trans.y, terrainInfo->trans.z);
		state->mapScl = vec3(terrainInfo->scale.x, terrainInfo->scale.y, terrainInfo->scale.z);
		state->mapInfo = terrainInfo->info;

		render->useTexture(TEXTURE_2D, 0, hizDepth->id);
		terrainCullShader->setFloat("uMaxLevel", hiz->getMaxLevel());
		terrainCullShader->setMatrix4("prevVPMatrix", prevCameraMat);
		terrainCullShader->setVector2("uSize", (float)render->viewWidth, (float)render->viewHeight);
		terrainCullShader->setVector2("uCamParam", camera->zNear, camera->zFar);

		vec3 ref = ((camera->position - state->mapTrans) / (state->mapScl * STEP_SIZE)) / CHUNK_SIZE;
		terrainCullShader->setIVector2("refChunk", floor(ref.x), floor(ref.z));
		state->shaderCompute = terrainCullShader;
		terrainNode->useTerrainUniforms(5);
		((TerrainDrawcall*)terrainNode->drawcall)->update(camera, render, state);
		state->shaderCompute = NULL;
		
		state->shader = render->getDebugTerrain() ? debugTerrainShader : terrainShader;
		state->shader->setHandle64("roadTex", AssetManager::assetManager->getRoadHnd());
		if (render->getDebugTerrain())
			state->shader->setInt("uDebugMid", MaterialManager::materials->find(BLUE_MAT));
		terrainNode->useTerrainUniforms(2);
		render->draw(camera, terrainNode->drawcall, state);

		if (/*!cfgs->cartoon && */!cfgs->debug && !render->getDebugTerrain()) 
			drawGrass(render, state, scene, camera, ref);
	}

	state->shader = phongShader;
	state->shaderIns = phongInsShader;
	state->shaderBone = boneShader;
	state->shaderBill = billInsShader;
	state->shaderCompute = NULL;

	lodParam.vpMat = camera->viewProjectMatrix;
	lodParam.eyePos = camera->position;
	lodParam.shadowPass = 0;
	lodParam.prevMat = prevCameraMat;
	lodParam.maxLevel = hiz->getMaxLevel();
	lodParam.size.x = render->viewWidth;
	lodParam.size.y = render->viewHeight;
	lodParam.camParam.x = camera->zNear;
	lodParam.camParam.y = camera->zFar;
	lodParam.depthTex = hizDepth->id;
	currentQueue->queues[QUEUE_DYN_MAIN]->process(scene, render, state, lodParam);
	currentQueue->queues[QUEUE_DYN_MAIN]->draw(scene, camera, render, state);

	currentQueue->queues[QUEUE_STATIC]->process(scene, render, state, lodParam);
	currentQueue->queues[QUEUE_STATIC]->draw(scene, camera, render, state);

	// Debug mode
	if (cfgs->debug && scene->isInited()) {
		state->debug = true;
		currentQueue->queues[QUEUE_DEBUG]->process(scene, render, state, lodParam);
		currentQueue->queues[QUEUE_DEBUG]->draw(scene, camera, render, state);
	}

	// Draw sky
	if (scene->skyBox) scene->skyBox->draw(render, skyShader, camera);

	if (needResize) needResize = false;
}

void RenderManager::renderTransparent(Render* render, Scene* scene) {
	static Shader* oitClear = render->findShader("oitClear");
	oit->resetOit(render, oitClear);

	static Shader* oitRender = render->findShader("oitRender");
	state->shaderTrans = oitRender;
	state->transparent = true;
	Camera* camera = scene->renderCamera;
	oit->beginRenderOit(render, state, state->shaderTrans);
	currentQueue->queues[QUEUE_DYN_MAIN]->drawTransparent(scene, camera, render, state);
	currentQueue->queues[QUEUE_STATIC]->drawTransparent(scene, camera, render, state);
	oit->endRenderOit(render);
	state->transparent = false;
}

void RenderManager::renderSkyTex(Render* render, Scene* scene) {
	if (!scene->skyBox) return;
	if (needRefreshSky) {
		static Shader* atmoShader = render->findShader("atmos");
		scene->skyBox->state->time = scene->time;
		scene->skyBox->state->udotl = udotl;
		scene->skyBox->update(render, lightDir, atmoShader);

		if (!ibl) ibl = new Ibl(scene);
		static bool needRefreshIbl = true;
		if (needRefreshIbl) {
			static Shader* irrShader = render->findShader("irradiance");
			static Shader* prefilterShader = render->findShader("prefiltered");
			static Shader* brdfShader = render->findShader("brdf");
			ibl->genIrradiance(render, irrShader);
			ibl->genPrefiltered(render, prefilterShader);
			ibl->genBrdf(render, brdfShader);
			needRefreshIbl = false;
		}

		needRefreshSky = false;
	}
}

void RenderManager::renderWater(Render* render, Scene* scene) {
	static Shader* waterCullShader = render->findShader("waterComp");
	static Shader* waterShader = render->findShader("water");
	Camera* camera = scene->renderCamera;

	state->reset();
	render->useTexture(TEXTURE_2D, 0, hizDepth->id);
	waterCullShader->setFloat("uMaxLevel", hiz->getMaxLevel());
	waterCullShader->setMatrix4("prevVPMatrix", prevCameraMat);
	waterCullShader->setVector2("uSize", (float)render->viewWidth, (float)render->viewHeight);
	waterCullShader->setVector2("uCamParam", camera->zNear, camera->zFar);
	state->shaderCompute = waterCullShader;
	((WaterDrawcall*)(scene->water->drawcall))->update(camera, render, state);
	state->shaderCompute = NULL;

	state->eyePos = &(scene->renderCamera->position);
	state->light = lightDir;
	state->udotl = udotl;
	state->time = scene->time;
	state->enableSsr = cfgs->ssr;
	state->waterPass = true;
	state->shader = waterShader;

	waterShader->setVector2("size", scene->water->size.x * 0.5, scene->water->size.y * 0.5);
	waterShader->setFloat("wHeight", scene->water->position.y);
	render->draw(camera, scene->water->drawcall, state);
}

void RenderManager::renderReflect(Render* render, Scene* scene) {
	if (!scene->water || !scene->reflectCamera || !reflectBuffer) return;
	render->setFrameBuffer(reflectBuffer);
	if (scene->terrainNode) {
		if (scene->terrainNode->checkInCamera(scene->reflectCamera)) {
			if (scene->terrainNode->drawcall) {
				static Shader* terrainShader = render->findShader("terrain");
				if (!terrainShader) {
					terrainShader = render->findShader("terrain");
					return;
				}
				state->reset();
				state->eyePos = &(scene->renderCamera->position);
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
	if (!deferredShader) {
		deferredShader = render->findShader("deferred");
		return;
	}
	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = DEFERRED_PASS;
	state->shader = deferredShader;
	state->shadow = shadow;
	state->light = lightDir;
	state->udotl = udotl;
	state->time = scene->time;
	state->quality = cfgs->graphQuality;
	state->dynSky = cfgs->dynsky;

	if (!deferredShader->isTexBinded(nearDynamicBuffer->getDepthBuffer()->hnd) ||
		!deferredShader->isTexBinded(nearStaticBuffer->getDepthBuffer()->hnd) ||
		!deferredShader->isTexBinded(midBuffer->getDepthBuffer()->hnd) ||
		!deferredShader->isTexBinded(farBuffer->getDepthBuffer()->hnd)) {
		u64 shadowTexs[4] = { nearDynamicBuffer->getDepthBuffer()->hnd, nearStaticBuffer->getDepthBuffer()->hnd,
				midBuffer->getDepthBuffer()->hnd, farBuffer->getDepthBuffer()->hnd };
			deferredShader->setHandle64v("shadowBuffers", 4, shadowTexs);
	}

	if (ibl && !deferredShader->isTexBinded(ibl->getIrradianceTex()->hnd))
		deferredShader->setHandle64("irradianceMap", ibl->getIrradianceTex()->hnd);
	if (ibl && !deferredShader->isTexBinded(ibl->getPrefilteredTex()->hnd))
		deferredShader->setHandle64("prefilteredMap", ibl->getPrefilteredTex()->hnd);
	if (ibl && !deferredShader->isTexBinded(ibl->getBrdf()->hnd))
		deferredShader->setHandle64("brdfMap", ibl->getBrdf()->hnd);

	filter->draw(scene->renderCamera, render, state, screenBuff->colorBuffers, screenBuff->depthBuffer);
}

void RenderManager::drawCombined(Render* render, Scene* scene, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	static Shader* combinedShader = render->findShader("combined");
	static Shader* combinedNFGShader = render->findShader("combined_nfg");
	if (!combinedShader) {
		combinedShader = render->findShader("combined");
		return;
	}
	if (!combinedNFGShader) {
		combinedNFGShader = render->findShader("combined_nfg");
		return;
	}
	state->reset();
	state->quality = cfgs->graphQuality;
	state->dynSky = cfgs->dynsky;
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = DEFERRED_PASS;
	state->shader = render->getFog() ? combinedShader : combinedNFGShader;
	filter->draw(scene->renderCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, FrameBuffer* inputBuff, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	if (!shader) return;
	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;

	filter->draw(scene->renderCamera, render, state, inputBuff->colorBuffers, NULL);
}

void RenderManager::drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, Texture2D* inputTexture, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	if (!shader) return;
	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;
	filter->draw(scene->renderCamera, render, state, inputTexture, NULL);
}

void RenderManager::drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	if (!shader) return;
	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;
	state->shader->setVector2("uCamParam", scene->renderCamera->zNear, scene->renderCamera->zFar);
	filter->draw(scene->renderCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawDualFilter(Render* render, Scene* scene, const char* shader1, const char* shader2, DualFilter* filter) {
	drawScreenFilter(render, scene, shader1, filter->getInput1(), filter->getOutput1());
	drawScreenFilter(render, scene, shader2, filter->getInput2(), filter->getOutput2());
}

void RenderManager::drawSSRFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	if (!shader) return;
	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;
	state->ssrPass = true;

	filter->draw(scene->renderCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawSSGFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter) {
	Shader* shader = render->findShader(shaderStr);
	if (!shader) return;
	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = shader;
	state->ssgPass = true;

	filter->draw(scene->renderCamera, render, state, inputTextures, NULL);
}

void RenderManager::drawTexture2Screen(Render* render, Scene* scene, u64 texhnd) {
	static Shader* screenShader = render->findShader("screen");
	if (!screenShader) {
		screenShader = render->findShader("screen");
		return;
	}
	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = screenShader;
	state->shader->setHandle64("tex", texhnd);

	if (!scene->textureNode) {
		Board board(2, 2, 2);
		scene->textureNode = new StaticNode(vec3(0, 0, 0));
		scene->textureNode->setFullStatic(true);
		StaticObject* boardObject = new StaticObject(&board);
		scene->textureNode->addObject(scene, boardObject);
		scene->textureNode->prepareDrawcall();
	}

	render->setFrameBuffer(NULL);
	render->draw(NULL, scene->textureNode->drawcall, state);
}

void RenderManager::drawDepth2Screen(Render* render, Scene* scene, int texid) {
	static Shader* screenShader = render->findShader("depth");
	if (!screenShader) {
		screenShader = render->findShader("depth");
		return;
	}
	state->reset();
	state->eyePos = &(scene->renderCamera->position);
	state->enableDepthTest = false;
	state->pass = POST_PASS;
	state->shader = screenShader;
	state->shader->setFloat("uLevel", 0);
	state->shader->setVector2("uCamParam", scene->renderCamera->zNear, scene->renderCamera->zFar);

	if (!scene->textureNode) {
		Board board(2, 2, 2);
		scene->textureNode = new StaticNode(vec3(0, 0, 0));
		scene->textureNode->setFullStatic(true);
		StaticObject* boardObject = new StaticObject(&board);
		scene->textureNode->addObject(scene, boardObject);
		scene->textureNode->prepareDrawcall();
	}

	render->setFrameBuffer(NULL);
	uint texBefore = render->useTexture(TEXTURE_2D, 0, texid);
	render->draw(NULL, scene->textureNode->drawcall, state);
	render->useTexture(TEXTURE_2D, 0, texBefore);
}

void RenderManager::genHiz(Render* render, Scene* scene, Texture2D* depth) {
	hizDepth->copyDataFrom(depth);
	static Shader* hizShader = render->findShader("hiz");
	hiz->genMipmap(render, hizShader, hizDepth);
}

void RenderManager::drawHiz2Screen(Render* render, Scene* scene, int level) {
	static Shader* depthShader = render->findShader("depth");
	hiz->drawDebug(scene->renderCamera, render, depthShader, hizDepth, level);
}

void RenderManager::retrievePrev(Scene* scene) {
	if (scene->renderCamera) prevCameraMat = scene->renderCamera->viewProjectMatrix;
}

bool RenderManager::isWaterShow(Render* render, const Scene* scene) { 
	return scene->water && renderShowWater && !render->getDebugTerrain(); 
}

void RenderManager::drawNoise3d(Render* render, Scene* scene, FrameBuffer* noiseBuf) {
	mat4 proj = perspective(90.0, 1.0, 0.5, 10.0);
	mat4 matPosx = proj * viewMat(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0, 0.0, 0.0));
	mat4 matNegx = proj * viewMat(vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0, 0.0, 0.0));
	mat4 matNegy = proj * viewMat(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0, 0.0, 0.0));
	mat4 matPosy = proj * viewMat(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0, 0.0, 0.0));
	mat4 matNegz = proj * viewMat(vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0, 0.0, 0.0));
	mat4 matPosz = proj * viewMat(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0, 0.0, 0.0));

	Sphere mesh(16, 16);
	StaticObject* object = new StaticObject(&mesh);
	object->bindMaterial(MaterialManager::materials->add(new Material("noise_mat")));
	object->setPosition(0, 0, 0);
	object->setSize(4, 4, 4);
	StaticNode* node = new StaticNode(vec3(0, 0, 0));
	node->setFullStatic(true);
	node->addObject(scene, object);
	node->updateNode(scene);
	node->prepareDrawcall();
	scene->noise3d = node;

	RenderState state;
	state.reset();
	state.cullMode = CULL_FRONT;
	state.lightEffect = false;
	state.delay = 0;
	state.shader = render->findShader("noise");
	if (!state.shader) return;

	render->useFrameBuffer(noiseBuf);

	render->useFrameCube(0, 0);
	render->setShaderMat4(state.shader, "viewProjectMatrix", matPosx);
	render->draw(NULL, node->drawcall, &state);

	render->useFrameCube(1, 0);
	render->setShaderMat4(state.shader, "viewProjectMatrix", matNegx);
	render->draw(NULL, node->drawcall, &state);

	render->useFrameCube(2, 0);
	render->setShaderMat4(state.shader, "viewProjectMatrix", matPosy);
	render->draw(NULL, node->drawcall, &state);

	render->useFrameCube(3, 0);
	render->setShaderMat4(state.shader, "viewProjectMatrix", matNegy);
	render->draw(NULL, node->drawcall, &state);

	render->useFrameCube(4, 0);
	render->setShaderMat4(state.shader, "viewProjectMatrix", matPosz);
	render->draw(NULL, node->drawcall, &state);

	render->useFrameCube(5, 0);
	render->setShaderMat4(state.shader, "viewProjectMatrix", matNegz);
	render->draw(NULL, node->drawcall, &state);
}

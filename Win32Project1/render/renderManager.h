/*
 * renderManager.h
 *
 *  Created on: 2017-9-5
 *      Author: a
 */

#ifndef RENDERMANAGER_H_
#define RENDERMANAGER_H_

#include "../scene/scene.h"
#include "../filter/filter.h"
#include "../render/renderQueue.h"

struct Renderable {
	RenderQueue* shadowNearStaticQueue;
	RenderQueue* shadowMidStaticQueue;
	RenderQueue* shadowFarStaticQueue;
	RenderQueue* shadowNearAnimateQueue;
	RenderQueue* shadowMidAnimateQueue;
	RenderQueue* shadowFarAnimateQueue;
	RenderQueue* staticQueue;
	RenderQueue* animateQueue;
	Camera* mainCamera;
	Renderable() {
		shadowNearStaticQueue = new RenderQueue();
		shadowMidStaticQueue = new RenderQueue();
		shadowFarStaticQueue = new RenderQueue();
		shadowNearAnimateQueue = new RenderQueue();
		shadowMidAnimateQueue = new RenderQueue();
		shadowFarAnimateQueue = new RenderQueue();
		staticQueue = new RenderQueue();
		animateQueue = new RenderQueue();
		mainCamera = new Camera(0);
	}
	~Renderable() {
		delete shadowNearStaticQueue; shadowNearStaticQueue = NULL;
		delete shadowMidStaticQueue; shadowMidStaticQueue = NULL;
		delete shadowFarStaticQueue; shadowFarStaticQueue = NULL;
		delete shadowNearAnimateQueue; shadowNearAnimateQueue = NULL;
		delete shadowMidAnimateQueue; shadowMidAnimateQueue = NULL;
		delete shadowFarAnimateQueue; shadowFarAnimateQueue = NULL;
		delete staticQueue; staticQueue = NULL;
		delete animateQueue; animateQueue = NULL;
		delete mainCamera; mainCamera = NULL;
	}
	void copyData(Renderable* src) {
		shadowNearStaticQueue->copyData(src->shadowNearStaticQueue);
		shadowMidStaticQueue->copyData(src->shadowMidStaticQueue);
		shadowFarStaticQueue->copyData(src->shadowFarStaticQueue);
		shadowNearAnimateQueue->copyData(src->shadowNearAnimateQueue);
		shadowMidAnimateQueue->copyData(src->shadowMidAnimateQueue);
		shadowFarAnimateQueue->copyData(src->shadowFarAnimateQueue);
		staticQueue->copyData(src->staticQueue);
		animateQueue->copyData(src->animateQueue);
	}
	void copyCamera(Camera* srcCam) {
		mainCamera->copy(srcCam);
	}
	void flush() {
		shadowNearStaticQueue->flush();
		shadowMidStaticQueue->flush();
		shadowFarStaticQueue->flush();
		shadowNearAnimateQueue->flush();
		shadowMidAnimateQueue->flush();
		shadowFarAnimateQueue->flush();
		staticQueue->flush();
		animateQueue->flush();
	}
};

class RenderManager {
public:
	VECTOR3D lightDir;
private:
	Shadow* shadow;
public:
	Renderable* renderData;
	Renderable* queue1;
	Renderable* queue2;
	Renderable* currentQueue;
	Renderable* nextQueue;
private: // States
	bool useShadow;
	bool drawBounding;
	RenderState* state;
private: // Shaders
	Shader* phongShadow;
	Shader* phongShadowLow;
	Shader* phong;
	Shader* phongShadowIns;
	Shader* phongShadowLowIns;
	Shader* phongIns;
	Shader* boneShadow;
	Shader* bone;
	Shader* mix;
	Shader* skyCube;
private:
	void drawBoundings(Render* render, RenderState* state, Scene* scene, Camera* camera);
public:
	FrameBuffer* nearBuffer;
	FrameBuffer* midBuffer;
	FrameBuffer* farBuffer;

	RenderManager(Camera* view, float distance1, float distance2, const VECTOR3D& light);
	~RenderManager();

	void updateShadowCamera();
	void updateMainLight();
	void flushRenderQueues();
	void updateRenderQueues(Scene* scene);
	void animateQueues(long startTime, long currentTime);
	void swapRenderQueues(Scene* scene);
	void renderShadow(Render* render,Scene* scene);
	void renderScene(Render* render,Scene* scene);
	void enableShadow(Render* render);
	void disableShadow(Render* render);
	void showBounding();
	void hideBounding();
};


#endif /* RENDERMANAGER_H_ */

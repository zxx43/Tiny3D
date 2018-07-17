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

#ifndef QUEUE_STATIC
#define QUEUE_STATIC_SN 0
#define QUEUE_STATIC_SM 1
#define QUEUE_STATIC_SF 2
#define QUEUE_ANIMATE_SN 3
#define QUEUE_ANIMATE_SM 4
#define QUEUE_ANIMATE_SF 5
#define QUEUE_STATIC 6
#define QUEUE_ANIMATE 7
#endif

struct Renderable {
	std::vector<RenderQueue*> queues;
	Camera* mainCamera;
	Renderable(float midDis, float lowDis) {
		queues.clear();
		for (uint i = 0; i < 8; i++)
			queues.push_back(new RenderQueue(midDis, lowDis));
		mainCamera = new Camera(0);
	}
	~Renderable() {
		for (uint i = 0; i < queues.size(); i++)
			delete queues[i];
		delete mainCamera; mainCamera = NULL;
	}
	void copyCamera(Camera* srcCam) {
		mainCamera->copy(srcCam);
	}
	void flush() {
		for (uint i = 0; i < queues.size(); i++)
			queues[i]->flush();
	}
};

class RenderManager {
public:
	VECTOR3D lightDir;
	RenderState* state;
private:
	Shadow* shadow;
	float time;
public:
	Renderable* renderData;
	Renderable* queue1;
	Renderable* queue2;
	Renderable* currentQueue;
	Renderable* nextQueue;
private: // States
	bool useShadow;
	bool drawBounding;
	int graphicQuality;
private:
	void drawBoundings(Render* render, RenderState* state, Scene* scene, Camera* camera);
public:
	FrameBuffer* nearBuffer;
	FrameBuffer* midBuffer;
	FrameBuffer* farBuffer;

	RenderManager(int quality, Camera* view, float distance1, float distance2, const VECTOR3D& light);
	~RenderManager();

	void act();
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

	void drawDeferred(Render* render, Scene* scene, FrameBuffer* screenBuff, Filter* filter);
};


#endif /* RENDERMANAGER_H_ */

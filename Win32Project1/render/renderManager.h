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
	Renderable(float midDis, float lowDis, bool dual) {
		queues.clear();
		for (uint i = 0; i < 8; i++) {
			queues.push_back(new RenderQueue(midDis, lowDis));
			queues[i]->setDual(dual);
		}
		queues[QUEUE_STATIC_SN]->shadowLevel = 1;
		queues[QUEUE_STATIC_SM]->shadowLevel = 2;
		queues[QUEUE_STATIC_SF]->shadowLevel = 3;
		queues[QUEUE_ANIMATE_SN]->shadowLevel = 1;
		queues[QUEUE_ANIMATE_SM]->shadowLevel = 2;
		queues[QUEUE_ANIMATE_SF]->shadowLevel = 3;
	}
	~Renderable() {
		for (uint i = 0; i < queues.size(); i++)
			delete queues[i];
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
	bool enableSsr;
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
	FrameBuffer* reflectBuffer;

	RenderManager(int quality, Camera* view, float distance1, float distance2, bool copyData, const VECTOR3D& light);
	~RenderManager();

	void act(float dTime);
	void resize(float width, float height);
	void updateShadowCamera(Camera* mainCamera);
	void updateMainLight();
	void flushRenderQueues();
	void updateRenderQueues(Scene* scene);
	void animateQueues(long startTime, long currentTime);
	void swapRenderQueues(Scene* scene, bool swapQueue);
	void renderShadow(Render* render,Scene* scene);
	void renderScene(Render* render,Scene* scene);
	void renderWater(Render* render, Scene* scene);
	void renderReflect(Render* render, Scene* scene);
	void showShadow(bool enable);
	void showBounding(bool enable);

	void drawDeferred(Render* render, Scene* scene, FrameBuffer* screenBuff, Filter* filter);
	void drawCombined(Render* render, Scene* scene, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, FrameBuffer* inputBuff, Filter* filter);
	void drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawSSRFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawTexture2Screen(Render* render, Scene* scene, uint texid);
};


#endif /* RENDERMANAGER_H_ */

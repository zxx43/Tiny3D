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
#include "../render/computeDrawcall.h"

struct Renderable {
	std::vector<RenderQueue*> queues;
	Renderable(float midDis, float lowDis, ConfigArg* cfg) {
		queues.clear();
		for (uint i = 0; i < 8; i++) {
			queues.push_back(new RenderQueue(i, midDis, lowDis));
			queues[i]->setCfg(cfg);
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
	vec3 lightDir;
	float udotl;
	RenderState* state;
	ConfigArg* cfgs;
	Texture2D* occluderDepth;
private:
	Shadow* shadow;
	bool needResize, needRefreshSky;
	ComputeDrawcall* grassDrawcall;
public:
	Renderable* renderData;
	Renderable* queue1;
	Renderable* queue2;
	Renderable* currentQueue;
	Renderable* nextQueue;
private:
	void drawBoundings(Render* render, RenderState* state, Scene* scene, Camera* camera);
	void drawGrass(Render* render, RenderState* state, Scene* scene, Camera* camera);
public:
	FrameBuffer* nearBuffer;
	FrameBuffer* midBuffer;
	FrameBuffer* farBuffer;
	FrameBuffer* reflectBuffer;

	RenderManager(ConfigArg* cfg, Camera* view, float distance1, float distance2, const vec3& light);
	~RenderManager();

	void resize(float width, float height);
	void updateShadowCamera(Camera* mainCamera);
	void updateMainLight();
	void updateSky();
	void flushRenderQueues();
	void updateRenderQueues(Scene* scene);
	void animateQueues(float velocity);
	void swapRenderQueues(Scene* scene, bool swapQueue);
	void renderShadow(Render* render,Scene* scene);
	void renderScene(Render* render,Scene* scene);
	void renderWater(Render* render, Scene* scene);
	void renderReflect(Render* render, Scene* scene);
	void renderSkyTex(Render* render, Scene* scene);

	void drawDeferred(Render* render, Scene* scene, FrameBuffer* screenBuff, Filter* filter);
	void drawCombined(Render* render, Scene* scene, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, FrameBuffer* inputBuff, Filter* filter);
	void drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawDualFilter(Render* render, Scene* scene, const char* shaderStr, DualFilter* filter);
	void drawSSRFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawSSGFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawTexture2Screen(Render* render, Scene* scene, u64 texhnd);
	void drawNoise3d(Render* render, Scene* scene, FrameBuffer* noiseBuf);
};


#endif /* RENDERMANAGER_H_ */

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
#include "../texture/hizGenerator.h"
#include "../ibl/ibl.h"
#include "../oit/oit.h"

struct Renderable {
	std::vector<RenderQueue*> queues;
	Renderable(float midDis, float lowDis, ConfigArg* cfg) {
		queues.clear();
		for (uint i = 0; i < QUEUE_SIZE; i++) 
			queues.push_back(new RenderQueue(i, midDis, lowDis, cfg));

		queues[QUEUE_DYN_SNEAR]->shadowLevel = 1;
		queues[QUEUE_DYN_SMID]->shadowLevel = 2;
		queues[QUEUE_DYN_SFAR]->shadowLevel = 3;
	}
	~Renderable() {
		for (uint i = 0; i < queues.size(); i++)
			delete queues[i];
	}
	void flush(Scene* scene) {
		for (uint i = 0; i < queues.size(); i++)
			queues[i]->flush(scene);
	}
};

class RenderManager {
public:
	vec3 lightDir;
	float udotl;
	RenderState* state;
	ConfigArg* cfgs;
	int depthPre;
	HizGenerator* hiz;
	Texture2D* hizDepth;
	Ibl* ibl;
	Oit* oit;
private:
	Shadow* shadow;
	bool needResize, needRefreshSky, actShowWater, renderShowWater;
	ComputeDrawcall* grassDrawcall;
	mat4 prevCameraMat;
	LodParam lodParam;
private:
	bool clearRender, resetRender, resetGather;
public:
	Renderable* renderData;
	Renderable* queue1;
	Renderable* queue2;
	Renderable* currentQueue;
	Renderable* nextQueue;
private:
	void drawGrass(Render* render, RenderState* state, Scene* scene, Camera* camera, const vec3& ref);
	void updateWaterVisible(const Scene* scene);
private:
	FrameBuffer* nearStaticBuffer;
	FrameBuffer* nearDynamicBuffer;
	FrameBuffer* midBuffer;
	FrameBuffer* farBuffer;
public:
	FrameBuffer* reflectBuffer;
public:
	RenderManager(ConfigArg* cfg, Scene* scene, float distance1, float distance2, const vec3& light);
	~RenderManager();
public:
	void resize(float width, float height);
	void updateShadowCamera(Camera* mainCamera);
	void updateMainLight(Scene* scene);
	void updateSky();
	void flushRenderQueues(Scene* scene);
	void updateRenderQueues(Scene* scene);
	void flushRenderQueueDatas(Scene* scene);
	void swapRenderQueues(Scene* scene, bool swapQueue);
	void clearGatherDatas(Scene* sceen);
	void clearRenderDatas(Scene* scene);
	void resetGatherDatas(Scene* scene);
	void resetRenderDatas(Scene* scene);
	void prepareData(Scene* scene);
	void updateDebugData(Scene* scene);
	void renderShadow(Render* render,Scene* scene);
	void renderScene(Render* render,Scene* scene);
	void renderTransparent(Render* render, Scene* scene);
	void renderWater(Render* render, Scene* scene);
	void renderReflect(Render* render, Scene* scene);
	void renderSkyTex(Render* render, Scene* scene);
public:
	void drawDeferred(Render* render, Scene* scene, FrameBuffer* screenBuff, Filter* filter);
	void drawCombined(Render* render, Scene* scene, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, FrameBuffer* inputBuff, Filter* filter);
	void drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, Texture2D* inputTexture, Filter* filter);
	void drawScreenFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawDualFilter(Render* render, Scene* scene, const char* shader1, const char* shader2, DualFilter* filter);
	void drawSSRFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawSSGFilter(Render* render, Scene* scene, const char* shaderStr, const std::vector<Texture2D*>& inputTextures, Filter* filter);
	void drawTexture2Screen(Render* render, Scene* scene, u64 texhnd);
	void drawDepth2Screen(Render* render, Scene* scene, int texid);
	void genHiz(Render* render, Scene* scene, Texture2D* depth);
	void drawHiz2Screen(Render* render, Scene* scene, int level);
	void drawNoise3d(Render* render, Scene* scene, FrameBuffer* noiseBuf);
	bool isWaterShow(Render* render, const Scene* scene);
	int getDepthPre() { return depthPre; }
	void retrievePrev(Scene* scene);
	void finishInit(Scene* scene);
};


#endif /* RENDERMANAGER_H_ */

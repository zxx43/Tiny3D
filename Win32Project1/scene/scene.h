/*
 * scene.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef SCENE_H_
#define SCENE_H_

#include "../node/terrainNode.h"
#include "../node/waterNode.h"
#include "../node/animationNode.h"
#include "../node/instanceNode.h"
#include "../sky/sky.h"
#include "player.h"

#include "../gather/processor.h"

#ifndef MAX_DEBUG_OBJ
#define MAX_DEBUG_OBJ 8192
#endif

class StaticObject;
class Scene {
private:
	bool inited;
	bool needUpdateStatics;
private:
	void initNodes();
public:
	float time, velocity;
	Camera* actCamera;
	Camera* renderCamera;
	Camera* reflectCamera;
	Sky* skyBox;
	WaterNode* water;
	TerrainNode* terrainNode;
	Node* root;
	Node* staticRoot;
	Node* animationRoot;
	Node* noise3d;
	Player* player;
	StaticNode* textureNode; // Use it to draw texture for debugging
	std::vector<Node*> boundingNodes; // Used for debugging
	std::vector<AnimationNode*> animPlayers;
	DynamicWorld* collisionWorld;
	SoundManager* soundMgr;
	std::vector<SoundObject*> sounds;
public:
	MeshManager* meshMgr;
	MeshGather* meshGather;
	MeshBuffer* meshBuffer;
	MeshManager* debugMeshMgr;
	MeshGather* debugMeshGather;
	MeshBuffer* debugMeshBuffer;
public:
	Scene();
	~Scene();
public:
	void createMeshGather();
	void releaseMeshGather();
	void createMeshBuffer();
	void releaseMeshBuffer();
	void createDebugGather();
	void releaseDebugGather();
	void createDebugBuffer();
	void releaseDebugBuffer();
public:
	void createReflectCamera();
	void createSky(bool dyn);
	void createWater(const vec3& position, const vec3& size);
	void createTerrain(const vec3& position, const vec3& size);
	void updateVisualTerrain(int bx, int bz, int sizex, int sizez);
	void updateNodes();
	void flushNodes();
	void updateReflectCamera();
	void addObject(Object* object);
	void removeObject(Object* object);
	void addPlay(AnimationNode* node);
	void removePlay(AnimationNode* node);
	void finishInit() { inited = true; }
	bool isInited() { return inited; }
	void act(float dTime) { time = dTime * 0.025; }
	void setVelocity(float v) { velocity = v; }
	void addSound(SoundObject* sound) { sounds.push_back(sound); }
	void playSounds();
	void updateListenerPosition() { soundMgr->setListenerPosition(actCamera->position); }
	void flushStaticDatas() { needUpdateStatics = true; }
	void staticDatasFlushed() { needUpdateStatics = false; }
	bool staticDatasShouldFlush() { return needUpdateStatics; }
public: // Just for debugging
	void updateNodeAABB(Node* node);
	void clearAllAABB();
private: 
	void updateAABBMesh(AABB* aabb, const char* mat, const char* mesh);
	void updateAABBWater(AABB* aabb, const char* mat, const vec3& exTrans, const vec3& exScale);
private:
	std::list<AnimationObject*> animationPhyObjects;
	std::list<StaticObject*> dynamicPhyObjects;
private:
	void removeAnimationPhy(AnimationObject* object) { animationPhyObjects.remove(object); }
	void removeDynamicPhy(StaticObject* object) { dynamicPhyObjects.remove(object); }
public:
	void initAnimNodes();
	void updateAnimNodes();
	void updateDynamicNodes();
private:
	void synPhysics2Graphic(AnimationNode* node, AnimationObject* object);
	void synPhysics2Graphic(StaticObject* object);
};


#endif /* SCENE_H_ */

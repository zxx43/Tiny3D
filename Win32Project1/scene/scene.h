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

class Scene {
private:
	void initNodes();
public:
	bool inited;
	Camera* mainCamera;
	Camera* reflectCamera;
	Sky* skyBox;
	WaterNode* water;
	TerrainNode* terrainNode;
	Node* staticRoot;
	Node* billboardRoot;
	Node* animationRoot;
	StaticNode* textureNode; // Use it to draw texture for debugging
	std::vector<Node*> boundingNodes; // Used for debugging
public:
	Scene();
	~Scene();
	void updateNodes();
	void flushNodes();
	void updateReflectCamera();
public: // Just for debugging
	void createNodeAABB(Node* node);
	void clearAllAABB();
};


#endif /* SCENE_H_ */

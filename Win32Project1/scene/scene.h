/*
 * scene.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef SCENE_H_
#define SCENE_H_

#include "../node/terrainNode.h"
#include "../node/animationNode.h"
#include "../node/instanceNode.h"
#include "../camera/camera.h"
#include "../sky/sky.h"

class Scene {
private:
	void initNodes();
public:
	Camera* mainCamera;
	Sky* skyBox;
	TerrainNode* terrainNode;
	Node* staticRoot;
	Node* billboardRoot;
	Node* animationRoot;
	StaticNode* screenNode;
	std::vector<Node*> boundingNodes; // Used for debugging
public:
	Scene();
	~Scene();
	void updateNodes();
public: // Just for debugging
	void createNodeAABB(Node* node);
	void clearAllAABB();
};


#endif /* SCENE_H_ */

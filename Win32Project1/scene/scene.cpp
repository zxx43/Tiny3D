#include "scene.h"
#include "../constants/constants.h"
#include "../assets/assetManager.h"
#include "../mesh/box.h"
#include "../mesh/sphere.h"
#include "../mesh/board.h"
#include "../mesh/quad.h"
#include "../mesh/model.h"
#include "../mesh/terrain.h"
#include "../object/staticObject.h"
#include "../object/billboardObject.h"
using namespace std;

Scene::Scene() {
	mainCamera = new Camera(4.0);
	skyBox = NULL;
	terrainNode = NULL;
	screenNode = NULL;
	
	staticRoot = NULL;
	billboardRoot = NULL;
	animationRoot = NULL;
	initNodes();
	boundingNodes.clear();
}

Scene::~Scene() {
	if (mainCamera) delete mainCamera; mainCamera = NULL;
	if (terrainNode) delete terrainNode; terrainNode = NULL;
	if (skyBox) delete skyBox; skyBox = NULL;
	if (screenNode) delete screenNode; screenNode = NULL;

	if (staticRoot) delete staticRoot; staticRoot = NULL;
	if (billboardRoot) delete billboardRoot; billboardRoot = NULL;
	if (animationRoot) delete animationRoot; animationRoot = NULL;
	clearAllAABB();
}

void Scene::initNodes() {
	staticRoot = new StaticNode(VECTOR3D(0, 0, 0));
	billboardRoot = new StaticNode(VECTOR3D(0, 0, 0));
	animationRoot = new StaticNode(VECTOR3D(0, 0, 0));
}

void Scene::createNodeAABB(Node* node) {
	AABB* aabb = (AABB*)node->boundingBox;
	if(aabb) {
		StaticNode* aabbNode = new StaticNode(aabb->position);
		StaticObject* aabbObject = new StaticObject(assetManager->meshes.find("box")->second);
		aabbObject->bindMaterial(materials->find(BLACK_MAT));
		aabbObject->setSize(aabb->sizex, aabb->sizey, aabb->sizez);
		aabbNode->addObject(aabbObject);
		aabbNode->prepareDrawcall();
		boundingNodes.push_back(aabbNode);
	}
	for (uint i = 0; i < node->children.size(); i++)
		createNodeAABB(node->children[i]);
}

void Scene::clearAllAABB() {
	for (uint i = 0; i<boundingNodes.size(); i++)
		delete boundingNodes[i];
	boundingNodes.clear();
}


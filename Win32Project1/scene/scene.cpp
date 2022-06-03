#include "scene.h"
#include "../constants/constants.h"
#include "../assets/assetManager.h"
#include "../mesh/box.h"
#include "../mesh/sphere.h"
#include "../mesh/board.h"
#include "../mesh/quad.h"
#include "../mesh/model.h"
#include "../mesh/terrain.h"
#include "../mesh/water.h"
#include "../object/staticObject.h"
#include "../gather/meshBuffer.h"
using namespace std;

Scene::Scene() {
	time = 0.0, velocity = 0.0;
	inited = false;
	player = new Player();
	actCamera = new Camera(25.0);
	renderCamera = new Camera(25.0);

	//for (int i = 0; i < 120; ++i) actCamera->turnX(RIGHT);

	reflectCamera = NULL;
	skyBox = NULL;
	water = NULL;
	terrainNode = NULL;
	textureNode = NULL;
	noise3d = NULL;
	
	root = NULL;
	staticRoot = NULL;
	animationRoot = NULL;
	initNodes();
	boundingNodes.clear();
	animPlayers.clear();
	animationPhyObjects.clear();
	dynamicPhyObjects.clear();
	Node::nodesToUpdate.clear();
	Node::nodesToRemove.clear();

	collisionWorld = new DynamicWorld();
	soundMgr = new SoundManager();
	sounds.clear();

	meshMgr = new MeshManager();
	debugMeshMgr = new MeshManager();
	meshGather = NULL, meshBuffer = NULL;
	debugMeshGather = NULL, debugMeshBuffer = NULL;

	needUpdateStatics = false;
}

Scene::~Scene() {
	delete player;
	if (renderCamera && renderCamera != actCamera) delete renderCamera; renderCamera = NULL;
	if (actCamera) delete actCamera; actCamera = NULL;
	if (reflectCamera) delete reflectCamera; reflectCamera = NULL;
	if (skyBox) delete skyBox; skyBox = NULL;
	if (water) delete water; water = NULL;
	if (terrainNode) delete terrainNode; terrainNode = NULL;
	if (textureNode) delete textureNode; textureNode = NULL;
	if (noise3d) delete noise3d; noise3d = NULL;
	if (root) {
		delete root; 
		staticRoot = NULL, animationRoot = NULL;
		root = NULL;
	}
	clearAllAABB();
	animPlayers.clear();
	animationPhyObjects.clear();
	dynamicPhyObjects.clear();

	delete collisionWorld;
	for (uint i = 0; i < sounds.size(); ++i)
		delete sounds[i];
	sounds.clear();
	delete soundMgr;

	releaseMeshBuffer();
	releaseDebugBuffer();
	releaseMeshGather();
	releaseDebugGather();
	delete meshMgr;
	delete debugMeshMgr;
}

void Scene::releaseMeshGather() {
	if (meshGather) delete meshGather; meshGather = NULL;
}

void Scene::createMeshGather() {
	releaseMeshGather();
	meshGather = new MeshGather(meshMgr);
}

void Scene::releaseDebugGather() {
	if (debugMeshGather) delete debugMeshGather; debugMeshGather = NULL;
}

void Scene::createDebugGather() {
	releaseDebugGather();
	debugMeshGather = new MeshGather(debugMeshMgr);
}

void Scene::releaseMeshBuffer() {
	if (meshBuffer) delete meshBuffer; meshBuffer = NULL;
}

void Scene::createMeshBuffer() {
	releaseMeshBuffer();
	meshBuffer = new MeshBuffer(meshGather);
}

void Scene::releaseDebugBuffer() {
	if (debugMeshBuffer) delete debugMeshBuffer; debugMeshBuffer = NULL;
}

void Scene::createDebugBuffer() {
	releaseDebugBuffer();
	debugMeshBuffer = new MeshBuffer(debugMeshGather);
}

void Scene::initNodes() {
	staticRoot = new StaticNode(vec3(0, 0, 0));
	animationRoot = new StaticNode(vec3(0, 0, 0));
	root = new StaticNode(vec3(0, 0, 0));
	root->attachChild(this, staticRoot);
	root->attachChild(this, animationRoot);
}

void Scene::updateNodes() {
	uint size = Node::nodesToUpdate.size();
	if (size == 0) return;
	for (uint i = 0; i < size; i++)
		Node::nodesToUpdate[i]->updateNode(this);
	Node::nodesToUpdate.clear();
}

void Scene::flushNodes() {
	uint size = Node::nodesToRemove.size();
	if (size == 0) return;
	for (uint i = 0; i < size; i++)
		delete Node::nodesToRemove[i];
	Node::nodesToRemove.clear();
}

void Scene::updateReflectCamera() {
	if (water && reflectCamera) {
		static mat4 transMat = scaleY(-1) * translate(0, water->position.y, 0);
		reflectCamera->viewMatrix = actCamera->viewMatrix * transMat;
		reflectCamera->lookDir.x = actCamera->lookDir.x;
		reflectCamera->lookDir.y = -actCamera->lookDir.y;
		reflectCamera->lookDir.z = actCamera->lookDir.z;
		reflectCamera->forceRefresh();
		reflectCamera->updateFrustum();
	}
}

void Scene::createReflectCamera() {
	if (reflectCamera) delete reflectCamera;
	reflectCamera = new Camera(0.0);
}

void Scene::createSky(bool dyn) {
	if (skyBox) delete skyBox;
	skyBox = new Sky(this, dyn);
}

void Scene::createWater(const vec3& position, const vec3& size) {
	if (water) delete water;
	water = new WaterNode(vec3(0, 0, 0));
	water->setFullStatic(true);
	StaticObject* waterObject = new StaticObject(AssetManager::assetManager->meshes["water"]);
	waterObject->setPosition(position.x, position.y, position.z);
	waterObject->setSize(size.x, size.y, size.z);
	water->addObject(this, waterObject);
	water->updateNode(this);
	water->prepareDrawcall();
}

void Scene::createTerrain(const vec3& position, const vec3& size) {
	if (terrainNode) delete terrainNode;
	terrainNode = new TerrainNode(position);
	terrainNode->setFullStatic(true);
	StaticObject* terrainObject = new StaticObject(AssetManager::assetManager->meshes["terrain"]);
	terrainObject->bindMaterial(MaterialManager::materials->find("terrain_mat"));
	terrainObject->setSize(size.x, size.y, size.z);
	terrainNode->addObject(this, terrainObject);
	terrainNode->prepareCollisionData();
	terrainNode->updateNode(this);
	terrainNode->prepareDrawcall();
}

void Scene::updateVisualTerrain(int bx, int bz, int sizex, int sizez) {
	if (!terrainNode) return;
	terrainNode->cauculateBlockIndices(bx, bz, sizex, sizez);
}

void Scene::updateAABBMesh(AABB* aabb, const char* mat, const char* mesh) {
	if (!aabb->debugNode) {
		aabb->debugNode = new InstanceNode(aabb->position);
		StaticObject* aabbObject = new StaticObject(AssetManager::assetManager->meshes[mesh]);
		aabbObject->setPhysic(false);
		aabbObject->setDebug(true);
		aabbObject->bindMaterial(MaterialManager::materials->find(mat));
		aabbObject->setSize(aabb->sizex, aabb->sizey, aabb->sizez);
		aabb->debugNode->addObject(this, aabbObject);
		boundingNodes.push_back(aabb->debugNode);
	}
	aabb->debugNode->scaleNodeObject(this, 0, aabb->sizex, aabb->sizey, aabb->sizez);
	aabb->debugNode->translateNode(this, aabb->position.x, aabb->position.y, aabb->position.z);
}

void Scene::updateAABBWater(AABB* aabb, const char* mat, const vec3& exTrans, const vec3& exScale) {
	if (!aabb->debugNode) {
		aabb->debugNode = new InstanceNode(aabb->position);
		StaticObject* aabbObject = new StaticObject(AssetManager::assetManager->meshes["box"]);
		aabbObject->setPhysic(false);
		aabbObject->setDebug(true);
		aabbObject->bindMaterial(MaterialManager::materials->find(mat));
		aabbObject->setSize(aabb->sizex, aabb->sizey, aabb->sizez);
		aabb->debugNode->addObject(this, aabbObject);
		boundingNodes.push_back(aabb->debugNode);
	}
	aabb->debugNode->scaleNodeObject(this, 0, aabb->sizex * exScale.x, aabb->sizey * exScale.y, aabb->sizez * exScale.z);
	aabb->debugNode->translateNode(this, aabb->position.x + exTrans.x, aabb->position.y + exTrans.y, aabb->position.z + exTrans.z);
}

void Scene::updateNodeAABB(Node* node) {
	if (node->type == TYPE_TERRAIN) {
		Terrain* t = ((TerrainNode*)node)->getMesh();
		for (int i = 0; i < t->chunks.size(); ++i) {
			AABB* aabb = t->chunks[i]->bounding;
			updateAABBMesh(aabb, GREEN_MAT, "box");
		}
		return;
	} else if (node->type == TYPE_WATER) {
		//Water* w = ((WaterNode*)node)->getMesh();
		//for (int i = 0; i < w->chunks.size(); ++i) {
		//	AABB* aabb = w->chunks[i]->bounding;
		//	vec3 ext(0.0), exs(1.0);
		//	ext.x = actCamera->position.x;
		//	ext.z = actCamera->position.z;
		//	updateAABBWater(aabb, RED_MAT, ext, exs);
		//}
		return;
	} else {
		if (node->type == TYPE_ANIMATE) {
			AABB* aabb = (AABB*)node->boundingBox;
			if (aabb) updateAABBMesh(aabb, RED_MAT, "box");
		} else {
			AABB* aabb = (AABB*)node->boundingBox;
			if (aabb) updateAABBMesh(aabb, BLUE_MAT, "box");
		}

		for (uint i = 0; i < node->children.size(); i++) 
			updateNodeAABB(node->children[i]);

		if (node->children.size() == 0) {
			for (uint i = 0; i < node->objects.size(); i++) {
				AABB* aabb = (AABB*)node->objects[i]->bounding;
				if (aabb) updateAABBMesh(aabb, BLACK_MAT, "box");
			}
		}
	}
}

void Scene::clearAllAABB() {
	for (uint i = 0; i<boundingNodes.size(); i++)
		delete boundingNodes[i];
	boundingNodes.clear();
}

void Scene::addObject(Object* object) {
	if (!object->isDebug()) {
		if (object->type == OBJ_TYPE_ANIMAT) { // Animation object
			AnimationObject* animObj = (AnimationObject*)object;
			if (animObj && animObj->parent)
				animationPhyObjects.push_back(animObj);
		} else {
			StaticObject* obj = (StaticObject*)object;
			if (obj->parent && obj->isDynamic())
				dynamicPhyObjects.push_back(obj);
		}
	}
	
	if (object->isPhysic()) {
		object->caculateCollisionShape();
		CollisionObject* cob = object->initCollisionObject();
		collisionWorld->addObject(cob);
	}

	if (object->isDebug()) debugMeshMgr->addObject(object);
	else meshMgr->addObject(object);

	if (!object->isDebug() && object->type == OBJ_TYPE_STATIC && !object->isDynamic()) flushStaticDatas();
}

void Scene::removeObject(Object* object) {
	if (object->isPhysic()) {
		collisionWorld->removeObject(object->collisionObject);
		object->removeCollisionObject();
	}

	if (!object->isDebug()) {
		if (object->type == OBJ_TYPE_ANIMAT)
			removeAnimationPhy((AnimationObject*)object);
		else if (object->type == OBJ_TYPE_STATIC) {
			if (object->isDynamic()) removeDynamicPhy((StaticObject*)object);
		}
	}

	if (!object->isDebug() && object->type == OBJ_TYPE_STATIC && !object->isDynamic()) flushStaticDatas();
}

void Scene::addPlay(AnimationNode* node) {
	animPlayers.push_back(node);
}

void Scene::removePlay(AnimationNode* node) {
	std::vector<AnimationNode*>::iterator it = animPlayers.begin();
	while (it != animPlayers.end()) {
		if (*it == node) {
			animPlayers.erase(it);
			break;
		}
		++it;
	}
}

void Scene::initAnimNodes() {
	list<AnimationObject*>::iterator it;
	for (it = animationPhyObjects.begin(); it != animationPhyObjects.end(); ++it) {
		AnimationObject* object = *it;
		if (object->parent) {
			AnimationNode* animNode = (AnimationNode*)(object->parent);
			animNode->doUpdateNodeTransform(this, true, true, true);
		}
	}
}

// Read collision transform to render data
void Scene::synPhysics2Graphic(StaticObject* object) {
	vec3 gPosition = object->collisionObject->getTranslate();
	gPosition += object->collisionObject->getLinearVelocity();
	object->translateAtWorld(gPosition);

	vec4 gQuat = object->collisionObject->getRotate();
	object->rotateAtWorld(gQuat);

	object->collisionObject->resetVelocity();
}

void Scene::updateDynamicNodes() {
	list<StaticObject*>::iterator it;
	for (it = dynamicPhyObjects.begin(); it != dynamicPhyObjects.end(); ++it) {
		StaticObject* object = *it;
		if (!object->collisionObject || object->collisionObject->isStatic() || !object->parent) continue;
		synPhysics2Graphic(object); // Read back collision transform
		object->standOnGround(this); // Stand object on ground after collision (no terrain collision) & update object's bounding box
		object->updateObjectTransform(true, true); // Send render data for using
	}
}

// Read collision transform to render data
void Scene::synPhysics2Graphic(AnimationNode* node, AnimationObject* object) {
	vec3 gPosition = object->collisionObject->getTranslate();
	gPosition += object->collisionObject->getLinearVelocity();
	node->translateNodeAtWorld(this, gPosition.x, gPosition.y, gPosition.z);

	vec4 gQuat = object->collisionObject->getRotate();
	node->rotateNodeAtWorld(this, gQuat);

	object->collisionObject->resetVelocity();
}

// Update animation nodes' transform & aabb after collision
void Scene::updateAnimNodes() {
	list<AnimationObject*>::iterator it;
	for (it = animationPhyObjects.begin(); it != animationPhyObjects.end(); ++it) {
		AnimationObject* object = *it;
		if (object->parent) {
			AnimationNode* node = (AnimationNode*)(object->parent);
			if (!object || !object->collisionObject || object->collisionObject->isStatic()) continue;

			synPhysics2Graphic(node, object); // Read back collision transform
			terrainNode->standObjectsOnGround(this, node); // Stand animation nodes on ground after collision (no terrain collision)
			node->boundingBox->update(GetTranslate(node->nodeTransform)); // Update bounding box after terrain collision
			Node* superior = node->parent;
			while (superior) {
				superior->updateBounding();
				superior = superior->parent;
			}

			object->updateObjectTransform(true, true); // Send render data for using
			if (player->getNode() == node)
				player->setPosition(node->position);
		}
	}
}

void Scene::playSounds() {
	for (uint i = 0; i < sounds.size(); ++i)
		sounds[i]->play();
}
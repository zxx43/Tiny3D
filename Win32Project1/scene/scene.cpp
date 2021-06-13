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
	
	staticRoot = NULL;
	billboardRoot = NULL;
	animationRoot = NULL;
	initNodes();
	boundingNodes.clear();
	meshCount.clear();
	meshes.clear();
	animCount.clear();
	anims.clear();
	animPlayers.clear();
	animationNodes.clear();
	dynamicObjects.clear();
	Node::nodesToUpdate.clear();
	Node::nodesToRemove.clear();
	Instance::instanceTable.clear();

	collisionWorld = new DynamicWorld();
	soundMgr = new SoundManager();
	sounds.clear();
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
	if (staticRoot) delete staticRoot; staticRoot = NULL;
	if (billboardRoot) delete billboardRoot; billboardRoot = NULL;
	if (animationRoot) delete animationRoot; animationRoot = NULL;
	meshCount.clear();
	clearAllAABB();
	for (uint i = 0; i < meshes.size(); ++i)
		delete meshes[i];
	meshes.clear();
	anims.clear();
	animPlayers.clear();
	animCount.clear();
	animationNodes.clear();
	dynamicObjects.clear();

	delete collisionWorld;
	for (uint i = 0; i < sounds.size(); ++i)
		delete sounds[i];
	sounds.clear();
	delete soundMgr;
}

void Scene::initNodes() {
	staticRoot = new StaticNode(vec3(0, 0, 0));
	billboardRoot = new StaticNode(vec3(0, 0, 0));
	animationRoot = new StaticNode(vec3(0, 0, 0));
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

void Scene::createNodeAABB(Node* node) {
	TerrainNode* tn = dynamic_cast<TerrainNode*>(node);
	if (tn != NULL) {
		Terrain* t = tn->getMesh();
		for (int i = 0; i < t->chunks.size(); ++i) {
			AABB* aabb = t->chunks[i]->bounding;
			StaticNode* aabbNode = new StaticNode(aabb->position);
			StaticObject* aabbObject = new StaticObject(AssetManager::assetManager->meshes["box"]);
			aabbNode->setDynamicBatch(false);
			aabbObject->bindMaterial(MaterialManager::materials->find(BLACK_MAT));
			aabbObject->setSize(aabb->sizex, aabb->sizey, aabb->sizez);
			aabbNode->addObject(this, aabbObject);
			aabbNode->updateNode(this);
			aabbNode->prepareDrawcall();
			aabbNode->updateRenderData();
			aabbNode->updateDrawcall();
			boundingNodes.push_back(aabbNode);
		}
		return;
	}

	AABB* aabb = (AABB*)node->boundingBox;
	if(aabb) {
		StaticNode* aabbNode = new StaticNode(aabb->position);
		StaticObject* aabbObject = new StaticObject(AssetManager::assetManager->meshes["box"]);
		aabbNode->setDynamicBatch(false);
		aabbObject->bindMaterial(MaterialManager::materials->find(BLACK_MAT));
		aabbObject->setSize(aabb->sizex, aabb->sizey, aabb->sizez);
		aabbNode->addObject(this, aabbObject);
		aabbNode->updateNode(this);
		aabbNode->prepareDrawcall();
		aabbNode->updateRenderData();
		aabbNode->updateDrawcall();
		boundingNodes.push_back(aabbNode);
	}
	for (uint i = 0; i < node->children.size(); i++)
		createNodeAABB(node->children[i]);
	
	if (node->children.size() == 0) {
		for (uint i = 0; i < node->objects.size(); i++) {
			AABB* aabb = (AABB*)node->objects[i]->bounding;
			if (aabb) {
				StaticNode* aabbNode = new StaticNode(aabb->position);
				StaticObject* aabbObject = new StaticObject(AssetManager::assetManager->meshes.find("box")->second);
				aabbNode->setDynamicBatch(false);
				aabbObject->bindMaterial(MaterialManager::materials->find(BLACK_MAT));
				aabbObject->setSize(aabb->sizex, aabb->sizey, aabb->sizez);
				aabbNode->addObject(this, aabbObject);
				aabbNode->updateNode(this);
				aabbNode->prepareDrawcall();
				aabbNode->updateRenderData();
				aabbNode->updateDrawcall();
				boundingNodes.push_back(aabbNode);
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
	Mesh* cur = object->mesh;
	if (cur) {
		if (meshCount.find(cur) == meshCount.end()) {
			meshCount[cur] = 0;
			meshes.push_back(new MeshObject(cur, object));
		}
		meshCount[cur]++;
	}
	cur = object->meshMid;
	if (cur && cur != object->mesh) {
		if (meshCount.find(cur) == meshCount.end()) {
			meshCount[cur] = 0;
			meshes.push_back(new MeshObject(cur, object));
		}
		meshCount[cur]++;
	}
	cur = object->meshLow;
	if (cur && cur != object->meshMid && cur != object->mesh) {
		if (meshCount.find(cur) == meshCount.end()) {
			meshCount[cur] = 0;
			meshes.push_back(new MeshObject(cur, object));
		}
		meshCount[cur]++;
	}
	
	if (!object->mesh) { // Animation object
		AnimationObject* animObj = (AnimationObject*)object;
		if (animObj) {
			Animation* curAnim = animObj->animation;
			if (animCount.find(curAnim) == animCount.end()) {
				animCount[curAnim] = 0;
				anims.push_back(curAnim);
			}
			animCount[curAnim]++;
			if (animObj->parent)
				animationNodes.push_back((AnimationNode*)(animObj->parent));
		}
	} else { 
		StaticObject* obj = (StaticObject*)object;
		if (obj->parent && obj->isDynamic())
			dynamicObjects.push_back(obj);
	}
	
	object->caculateCollisionShape();
	CollisionObject* cob = object->initCollisionObject();
	collisionWorld->addObject(cob);
}

void Scene::addPlay(AnimationNode* node) {
	animPlayers.push_back(node);
}

uint Scene::queryMeshCount(Mesh* mesh) {
	if (meshCount.find(mesh) == meshCount.end())
		meshCount[mesh] = 0;
	return meshCount[mesh];
}

void Scene::initAnimNodes() {
	list<AnimationNode*>::iterator it;
	for (it = animationNodes.begin(); it != animationNodes.end(); ++it) 
		(*it)->doUpdateNodeTransform(this, true, true, true);
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
	for (it = dynamicObjects.begin(); it != dynamicObjects.end(); ++it) {
		StaticObject* object = *it;
		if (!object->collisionObject || object->collisionObject->isStatic()) continue;
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
	list<AnimationNode*>::iterator it;
	for (it = animationNodes.begin(); it != animationNodes.end(); ++it) {
		AnimationNode* node = *it;
		AnimationObject* object = node->getObject();
		if (!object->collisionObject || object->collisionObject->isStatic()) continue;

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

void Scene::playSounds() {
	for (uint i = 0; i < sounds.size(); ++i)
		sounds[i]->play();
}
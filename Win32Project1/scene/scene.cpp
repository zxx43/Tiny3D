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
	animNodeToUpdate.clear();
	animationNodes.clear();
	Node::nodesToUpdate.clear();
	Node::nodesToRemove.clear();
	Instance::instanceTable.clear();

	collisionWorld = new DynamicWorld();
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
	animNodeToUpdate.clear();
	animationNodes.clear();

	delete collisionWorld;
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
	// Animation object
	if (!object->mesh) {
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
	for (uint i = 0; i < animationNodes.size(); ++i) 
		animationNodes[i]->doUpdateNodeTransform(this, true, true, true);
}

// Read collision transform to render data
void Scene::synPhysics2Graphic(AnimationNode* node, AnimationObject* object) {
	vec3 gPosition = object->collisionObject->getTranslate();
	gPosition += object->collisionObject->getLinearVelocity();
	node->translateNodeAtWorld(this, gPosition.x, gPosition.y, gPosition.z);

	vec3 gAngle = object->collisionObject->getRotateAngle();
	// Todo gimbal lock to fix
	if (fabsf(gAngle.x) > 175.0 && fabsf(gAngle.z) > 175.0) 
		gAngle.y = -gAngle.y + 180.0;
	gAngle.x = 0.0;
	gAngle.z = 0.0;
	node->rotateNodeAtWorld(this, gAngle);

	object->collisionObject->resetVelocity();
}

// Update animation nodes' transform & aabb after collision
void Scene::updateAnimNodes() {
	for (uint i = 0; i < animationNodes.size(); ++i) {
		AnimationObject* object = animationNodes[i]->getObject();
		if (object->collisionObject->isStatic()) continue;
		animationNodes[i]->pushToUpdate(this);
	}

	for (uint i = 0; i < animNodeToUpdate.size(); ++i) {
		AnimationNode* node = animNodeToUpdate[i];
		AnimationObject* object = node->getObject();

		synPhysics2Graphic(node, object); // Read back collision transform
		terrainNode->standObjectsOnGround(this, node); // Stand animation nodes on ground after collision (no terrain collision)
		if(player->getNode() == node)
			player->setPosition(node->position);
		
		node->boundingBox->update(GetTranslate(node->nodeTransform)); // Update bounding box after terrain collision
		node->updateNodeObject(node->getObject(), true, true); // Send render data for using
		Node* superior = node->parent;
		while (superior) {
			superior->updateBounding();
			superior = superior->parent;
		}
		node->setUpdate(false);
	}
	animNodeToUpdate.clear();
}

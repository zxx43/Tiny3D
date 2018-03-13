#include "simpleApplication.h"
#include "mesh/model.h"
#include "mesh/terrain.h"
#include "object/staticObject.h"
#include "object/billboardObject.h"
#include "constants/constants.h"
#include <map>
#include <string>
using namespace std;

SimpleApplication::SimpleApplication() {
	screen = NULL;
	screenFilter = NULL;
}

SimpleApplication::~SimpleApplication() {
	delete screen; screen = NULL;
	delete screenFilter; screenFilter = NULL;
}

void SimpleApplication::resize(int width, int height) {
	if (!render) return;
	Application::resize(width, height);

	if (screen) delete screen;
	screen = new FrameBuffer(width, height, true, LOW_PRE);
	screen->addColorBuffer();
	screen->addColorBuffer();

	if (screenFilter) delete screenFilter;
	screenFilter = new Filter(0.8 * width, 0.8 * height, false, LOW_PRE);
}

void SimpleApplication::keyDown(int key) {
	Application::keyDown(key);
}

void SimpleApplication::keyUp(int key) {
	Application::keyUp(key);
}

void SimpleApplication::draw() {
	renderMgr->renderShadow(render, scene);
	render->setFrameBuffer(screen);
//	render->setFrameBuffer(NULL);
	renderMgr->renderScene(render, scene);
	screenFilter->draw(render, render->findShader("blur"), screen->colorBuffers);
}

void SimpleApplication::init() {
	Application::init();
	initScene();
	initScreen();
}

void SimpleApplication::initScreen() {
	scene->screenNode = new StaticNode(VECTOR3D(0, 0, 0));
	StaticObject* board = new StaticObject(AssetManager::assetManager->meshes["board"]);
	scene->screenNode->addObject(board);
}

void SimpleApplication::moveCamera() {
	Application::moveCamera();
	if (scene->terrainNode) {
		VECTOR3D cp = scene->mainCamera->position;
		cp.y = scene->terrainNode->cauculateY(cp.x, cp.z) + scene->mainCamera->getHeight();
		scene->mainCamera->moveTo(cp);
	}
}

void SimpleApplication::act(long startTime, long currentTime) {
	Application::act(startTime, currentTime);
	/*
		Node* node = scene->animationRoot->children[0];
		AnimationNode* animNode = (AnimationNode*)node->children[0];
		animNode->rotateNodeObject(0, 45, 0);
		animNode->translateNode(animNode->position.x + 0.02, animNode->position.y, animNode->position.z + 0.02);
		standObjectsOnGround(animNode, scene->terrainNode);
	//*/
	/*
	static int time = 1;
	if (currentTime - startTime > 10000 * time && scene->staticRoot->children.size() > 1) {
		scene->staticRoot->detachChild(scene->staticRoot->children[1]);
		time++;
	}
	*/
	scene->updateNodes();
}

void SimpleApplication::initScene() {
	scene->skyBox = new Sky();

	// Load meshes
	AssetManager::assetManager->meshes["tree"] = new Model("models/firC.obj", "models/firC.mtl", 2, true);
	AssetManager::assetManager->meshes["treeA"] = new Model("models/treeA.obj", "models/treeA.mtl", 2, true);
	AssetManager::assetManager->meshes["tank"] = new Model("models/tank.obj", "models/tank.mtl", 3, true);
	AssetManager::assetManager->meshes["m1a2"] = new Model("models/m1a2.obj", "models/m1a2.mtl", 2, true);
	AssetManager::assetManager->meshes["terrain"] = new Terrain("terrain/Terrain.raw");
	AssetManager::assetManager->animations["army"] = new Animation("models/ArmyPilot.dae");

	// Load textures
	ImageSet* textures = AssetManager::assetManager->textures;
	textures->addTexture("cube.bmp");
	textures->addTexture("ground.bmp");
	textures->addTexture("ground_r.bmp");
	textures->addTexture("sand.bmp");
	AssetManager::assetManager->initTextureArray();

	// Create materials
	Material* boxMat = new Material("box_mat");
	boxMat->texture.x = textures->findTexture("cube.bmp");
	MaterialManager::materials->add(boxMat);
	Material* grassMat = new Material("grass_mat");
	grassMat->texture.x = textures->findTexture("ground.bmp");
	MaterialManager::materials->add(grassMat);
	Material* sandMat = new Material("sand_mat");
	sandMat->texture.x = textures->findTexture("sand.bmp");
	MaterialManager::materials->add(sandMat);
	Material* terrainMat = new Material("terrain_mat");
	terrainMat->texture.x = textures->findTexture("ground.bmp");
	terrainMat->texture.y = textures->findTexture("ground_r.bmp");
	MaterialManager::materials->add(terrainMat);

	// Create Nodes
	map<string, Mesh*> meshes = AssetManager::assetManager->meshes;
	map<string, Animation*> animations = AssetManager::assetManager->animations;

	StaticObject* box = new StaticObject(meshes["box"]); 
	box->bindMaterial(MaterialManager::materials->find("box_mat"));
	StaticObject* sphere = new StaticObject(meshes["sphere"]); 
	sphere->bindMaterial(MaterialManager::materials->find("grass_mat"));
	StaticObject* board = new StaticObject(meshes["board"]);
	StaticObject* quad = new StaticObject(meshes["quad"]);
	StaticObject* model1 = new StaticObject(meshes["tree"]);
	StaticObject* model2 = new StaticObject(meshes["tank"]);
	StaticObject* model3 = new StaticObject(meshes["m1a2"]);
	StaticObject* model4 = new StaticObject(meshes["treeA"]);
	BillboardObject* billboard = new BillboardObject(meshes["board"]);

	scene->terrainNode = new TerrainNode(VECTOR3D(-1024, 0, -1024));
	scene->terrainNode->fullStatic = true;
	StaticObject* terrainObject = new StaticObject(meshes["terrain"]);
	terrainObject->bindMaterial(MaterialManager::materials->find("terrain_mat"));
	terrainObject->setPosition(0, 0, 0);
	terrainObject->setSize(2, 1, 2);
	scene->terrainNode->addObject(terrainObject);
	scene->terrainNode->prepareTriangles();


	StaticNode* node1 = new StaticNode(VECTOR3D(2, 2, 2));
	StaticObject* object11 = model2->clone();
	object11->setPosition(-5, -7, 5);
	object11->setRotation(0, 90, 0);
	object11->setSize(0.05, 0.05, 0.05);
	node1->addObject(object11);
	StaticObject* object12 = model2->clone();
	object12->setPosition(-1, -7, 5);
	object12->setRotation(0, 90, 0);
	object12->setSize(0.05, 0.05, 0.05);
	node1->addObject(object12);
	StaticObject* object13 = model3->clone();
	object13->setPosition(-2, -7, 0);
	object13->setSize(0.02, 0.02, 0.02);
	node1->addObject(object13);
	StaticObject* object14 = model3->clone();
	object14->setPosition(2, -7, 0);
	object14->setSize(0.02, 0.02, 0.02);
	node1->addObject(object14);

	StaticNode* node2 = new StaticNode(VECTOR3D(10, 2, 2));
	StaticObject* object6 = box->clone();
	object6->bindMaterial(MaterialManager::materials->find(DEFAULT_MAT));
	object6->setPosition(3, 3, 3);
	object6->setRotation(0, 30, 0);
	object6->setSize(1, 1, 1);
	node2->addObject(object6);
	StaticObject* object7 = box->clone();
	object7->setPosition(-1, 1, 2);
	object7->setRotation(0, 0, 30);
	object7->setSize(2, 2, 2);
	node2->addObject(object7);

	StaticNode* node3 = new StaticNode(VECTOR3D(5, 10, 0));
	StaticObject* objectSphere = sphere->clone();
	objectSphere->setSize(2, 2, 2);
	node3->addObject(objectSphere);

	Node* node = new StaticNode(VECTOR3D(-20, 0, 0));
	Node* modelNode = new StaticNode(VECTOR3D(0, 0, 0));
	modelNode->attachChild(node1);
	modelNode->attachChild(node2);
	node->attachChild(modelNode);
	node->attachChild(node3);
	scene->staticRoot->attachChild(node);

	StaticObject* objectTree = model4->clone();
	objectTree->setPosition(-10, 0, 0);
	objectTree->setSize(0.5, 0.5, 0.5);
	node1->addObject(objectTree);

	
	srand(100);
	InstanceNode* instanceNode1 = new InstanceNode(VECTOR3D(-920, 0, -920));
	instanceNode1->singleSide = true;
	for (uint i = 0; i < 10; i++) {
		for (uint j = 0; j < 10; j++) {
			StaticObject* tree = (rand() % 100) > 40 ? model4->clone() : model1->clone();
			//StaticObject* tree = model4->clone();
			float size = (rand() % 100 * 0.01) * 2 + 1;
			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 80 + 80 * (rand() % 100) * 0.01, 0, 200 + i * 40 + 40 * (rand() % 100) * 0.01);
			instanceNode1->addObject(tree);
		}
	}
	InstanceNode* instanceNode2 = new InstanceNode(VECTOR3D(20, 0, -20));
	instanceNode2->singleSide = true;
	for (uint i = 0; i < 10; i++) {
		for (uint j = 0; j < 10; j++) {
			StaticObject* tree = (rand() % 100) > 40 ? model4->clone() : model1->clone();
			//StaticObject* tree = model4->clone();
			float size = (rand() % 100 * 0.01) * 2 + 1;
			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 80 + 80 * (rand() % 100) * 0.01, 0, -(200 + i * 40 + 40 * (rand() % 100) * 0.01));
			instanceNode2->addObject(tree);
		}
	}
	InstanceNode* instanceNode3 = new InstanceNode(VECTOR3D(-20, 0, -20));
	instanceNode3->singleSide = true;
	for (uint i = 0; i < 10; i++) {
		for (uint j = 0; j < 10; j++) {
			StaticObject* tree = (rand() % 100) > 40 ? model4->clone() : model1->clone();
			//StaticObject* tree = model4->clone();
			float size = (rand() % 100 * 0.01) * 2 + 1;
			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(200 + j * 80 + 80 * (rand() % 100) * 0.01, 0, i * 80 + 80 * (rand() % 100) * 0.01);
			instanceNode3->addObject(tree);
		}
	}
	InstanceNode* instanceNode4 = new InstanceNode(VECTOR3D(-20, 0, 20));
	instanceNode4->singleSide = true;
	for (uint i = 0; i < 10; i++) {
		for (uint j = 0; j < 10; j++) {
			StaticObject* tree = (rand() % 100) > 40 ? model4->clone() : model1->clone();
			//StaticObject* tree = model4->clone();
			float size = (rand() % 100 * 0.01) * 2 + 1;
			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(-(200 + j * 80 + 80 * (rand() % 100) * 0.01), 0, i * 80 + 80 * (rand() % 100) * 0.01);
			instanceNode4->addObject(tree);
		}
	}
	scene->staticRoot->attachChild(instanceNode1);
	scene->staticRoot->attachChild(instanceNode2);
	scene->staticRoot->attachChild(instanceNode3);
	scene->staticRoot->attachChild(instanceNode4);
	
	AnimationNode* animNode1 = new AnimationNode(VECTOR3D(5, 10, 5));
	animNode1->setAnimation(animations.find("army")->second);
	animNode1->getObject()->setSize(0.05, 0.05, 0.05);
	animNode1->getObject()->setPosition(0, -5, -1);
	animNode1->translateNode(5, 0, 15);
	AnimationNode* animNode2 = new AnimationNode(VECTOR3D(5, 10, 5));
	animNode2->setAnimation(animations.find("army")->second);
	animNode2->getObject()->setSize(0.05, 0.05, 0.05);
	animNode2->getObject()->setPosition(0, -5, -1);
	animNode2->translateNode(40, 0, 40);
	animNode2->rotateNodeObject(0, 45, 0);
	AnimationNode* animNode3 = new AnimationNode(VECTOR3D(5, 10, 5));
	animNode3->setAnimation(animations.find("army")->second);
	animNode3->getObject()->setSize(0.05, 0.05, 0.05);
	animNode3->getObject()->setPosition(0, -5, -1);
	animNode3->translateNode(5, 0, 15);
	AnimationNode* animNode4 = new AnimationNode(VECTOR3D(5, 10, 5));
	animNode4->setAnimation(animations.find("army")->second);
	animNode4->getObject()->setSize(0.05, 0.05, 0.05);
	animNode4->getObject()->setPosition(0, -5, -1);
	animNode4->translateNode(40, 0, 40);
	animNode4->rotateNodeObject(0, 90, 0);

	Node* animNode = new StaticNode(VECTOR3D(0, 0, 0));
	animNode->attachChild(animNode1);
	animNode->attachChild(animNode2);
	scene->animationRoot->attachChild(animNode);
	Node* animNodeSub = new StaticNode(VECTOR3D(0, 0, 0));
	animNodeSub->attachChild(animNode3);
	animNodeSub->attachChild(animNode4);
	scene->animationRoot->attachChild(animNodeSub);

	animNode->translateNode(0, 10, 0);
	animNodeSub->translateNode(10, 0, 0);
	
	node1->translateNode(0, 0, 20);

	delete box;
	delete sphere;
	delete board;
	delete quad;
	delete model1;
	delete model2;
	delete model3;
	delete model4;
	delete billboard;

	standObjectsOnGround(scene->staticRoot, scene->terrainNode);
	standObjectsOnGround(scene->animationRoot, scene->terrainNode);
}


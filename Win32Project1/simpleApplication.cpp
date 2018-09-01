#include "simpleApplication.h"
#include "mesh/model.h"
#include "mesh/terrain.h"
#include "mesh/water.h"
#include "object/staticObject.h"
#include "constants/constants.h"
using namespace std;

SimpleApplication::SimpleApplication() {
	screen = NULL;
	screenFilter = NULL;
	aaFilter = NULL;
	blurFilter = NULL;
	dofFilter = NULL;
	dofInput.clear();
}

SimpleApplication::~SimpleApplication() {
	dofInput.clear();
	if (screen) delete screen; screen = NULL;
	if (screenFilter) delete screenFilter; screenFilter = NULL;
	if (blurFilter) delete blurFilter; blurFilter = NULL;
	if (dofFilter) delete dofFilter; dofFilter = NULL;
	if (aaFilter) delete aaFilter; aaFilter = NULL;
}

void SimpleApplication::resize(int width, int height) {
	if (!render) return;
	Application::resize(width, height);

	int precision = graphQuality > 4.0 ? HIGH_PRE : LOW_PRE;

	if (screen) delete screen;
	screen = new FrameBuffer(width, height, precision, 4);
	screen->addColorBuffer(precision, 3);
	screen->addColorBuffer(precision, 3);
	screen->attachDepthBuffer(precision);

	if (screenFilter) delete screenFilter;
	if (!useFxaa && !useDof)
		screenFilter = new Filter(width, height, false, precision, 4);
	else {
		screenFilter = new Filter(width, height, true, precision, 4);
		if (useFxaa) {
			if (aaFilter) delete aaFilter;
			aaFilter = new Filter(width, height, false, precision, 4);
		}
		if (useDof) {
			if (blurFilter) delete blurFilter;
			blurFilter = new Filter(width * 0.4, height * 0.4, true, precision, 4);
			if (dofFilter) delete dofFilter;
			dofFilter = new Filter(width, height, useFxaa, precision, 4);

			dofInput.clear();
			dofInput.push_back(blurFilter->getFrameBuffer()->getColorBuffer(0));
			dofInput.push_back(screenFilter->getFrameBuffer()->getColorBuffer(0));
		}
	}

	render->clearTextureSlots();
}

void SimpleApplication::keyDown(int key) {
	Application::keyDown(key);
	if (key == 'C')
		printf("pos: %f,%f\n", scene->mainCamera->position.x, scene->mainCamera->position.z);
}

void SimpleApplication::keyUp(int key) {
	Application::keyUp(key);
}

void SimpleApplication::draw() {
	if (!screenFilter) return;

	renderMgr->renderShadow(render, scene);
	render->setFrameBuffer(screen);
	renderMgr->renderScene(render, scene);
	renderMgr->drawDeferred(render, scene, screen, screenFilter);

	Filter* lastFilter = screenFilter;
	if (useDof) {
		renderMgr->drawScreenFilter(render, scene, "blur", screenFilter->getFrameBuffer(), blurFilter);
		renderMgr->drawScreenFilter(render, scene, "dof", dofInput, dofFilter);
		lastFilter = dofFilter;
	}
	if (useFxaa)
		renderMgr->drawScreenFilter(render, scene, "fxaa", lastFilter->getFrameBuffer(), aaFilter);

	render->finishDraw();
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

void SimpleApplication::moveKey() {
	Application::moveKey();
	if (scene->water)
		scene->water->moveWaterWithCamera(scene->mainCamera);
	if (scene->terrainNode) {
		VECTOR3D cp = scene->mainCamera->position;
		if (scene->terrainNode->cauculateY(cp.x, cp.z, cp.y)) {
			if (scene->water) {
				float waterHeight = scene->water->position.y;
				cp.y = cp.y < waterHeight ? waterHeight : cp.y;
			}
			cp.y += scene->mainCamera->getHeight();
		} else if(scene->water) {
			float waterHeight = scene->water->position.y;
			cp.y = waterHeight;
			cp.y += scene->mainCamera->getHeight();
		}
		scene->mainCamera->moveTo(cp);
	}
}

void SimpleApplication::act(long startTime, long currentTime) {
	Application::act(startTime, currentTime);
	/*
		Node* node = scene->animationRoot->children[0];
		AnimationNode* animNode = (AnimationNode*)node->children[0];
		animNode->rotateNodeObject(0, 45, 0);
		animNode->translateNode(animNode->position.x + 0.01, animNode->position.y, animNode->position.z + 0.01);
		standObjectsOnGround(animNode, scene->terrainNode);
	//*/
	/*
	static int time = 1;
	if (currentTime - startTime > 10000 * time && scene->staticRoot->children.size() > 1) {
		scene->staticRoot->detachChild(scene->staticRoot->children[1])->pushToRemove();
		time++;
	}
	//*/
	scene->updateNodes();
}

void SimpleApplication::initScene() {
	scene->skyBox = new Sky();

	AssetManager* assetMgr = AssetManager::assetManager;
	MaterialManager* mtlMgr = MaterialManager::materials;

	// Load meshes
	assetMgr->addMesh("tree", new Model("models/firC.obj", "models/firC.mtl", 2, true));
	assetMgr->addMesh("treeMid", new Model("models/firC_mid.obj", "models/firC_mid.mtl", 2, true));
	assetMgr->addMesh("treeLow", new Model("models/fir_mesh.obj", "models/fir_mesh.mtl", 3, true));
	assetMgr->addMesh("treeA", new Model("models/treeA.obj", "models/treeA.mtl", 2, true));
	assetMgr->addMesh("treeAMid", new Model("models/treeA_mid.obj", "models/treeA_mid.mtl", 2, true));
	assetMgr->addMesh("treeALow", new Model("models/treeA_low.obj", "models/treeA_low.mtl", 2, true));
	assetMgr->addMesh("tank", new Model("models/tank.obj", "models/tank.mtl", 3, true));
	assetMgr->addMesh("m1a2", new Model("models/m1a2.obj", "models/m1a2.mtl", 2, true));
	assetMgr->addMesh("house", new Model("models/house.obj", "models/house.mtl", 2, true));
	assetMgr->addMesh("oildrum", new Model("models/oildrum.obj", "models/oildrum.mtl", 3, true));
	assetMgr->addMesh("terrain", new Terrain("terrain/Terrain.raw"));
	assetMgr->addMesh("water", new Water(1024, 16));
	assetMgr->addAnimation("army", new Animation("models/ArmyPilot.dae"));

	// Load textures
	assetMgr->addTexture("cube.bmp");
	assetMgr->addTexture("ground.bmp");
	assetMgr->addTexture("ground_r.bmp");
	assetMgr->addTexture("sand.bmp");
	assetMgr->addTexture("tree2.bmp");
	assetMgr->initTextureArray();

	// Create materials
	Material* boxMat = new Material("box_mat");
	boxMat->texture.x = assetMgr->findTexture("cube.bmp");
	boxMat->ambient = VECTOR3D(0.4, 0.4, 0.4); boxMat->diffuse = VECTOR3D(0.6, 0.6, 0.6);
	mtlMgr->add(boxMat);
	Material* grassMat = new Material("grass_mat");
	grassMat->texture.x = assetMgr->findTexture("ground.bmp");
	mtlMgr->add(grassMat);
	Material* sandMat = new Material("sand_mat");
	sandMat->texture.x = assetMgr->findTexture("sand.bmp");
	mtlMgr->add(sandMat);
	Material* terrainMat = new Material("terrain_mat");
	terrainMat->texture.x = assetMgr->findTexture("ground.bmp");
	terrainMat->texture.y = assetMgr->findTexture("ground_r.bmp");
	mtlMgr->add(terrainMat);

	// Create Nodes
	map<string, Mesh*> meshes = assetMgr->meshes;
	map<string, Animation*> animations = assetMgr->animations;

	StaticObject* box = new StaticObject(meshes["box"]); 
	box->bindMaterial(mtlMgr->find("box_mat"));
	StaticObject* sphere = new StaticObject(meshes["sphere"]); 
	sphere->bindMaterial(mtlMgr->find("grass_mat"));
	StaticObject* board = new StaticObject(meshes["board"]);
	StaticObject* quad = new StaticObject(meshes["quad"]);

	StaticObject* model1 = new StaticObject(meshes["tree"], meshes["treeMid"], meshes["billboard"]);
	model1->setBillboard(5, 10, assetMgr->findTexture("tree2.bmp"));
	StaticObject* model2 = new StaticObject(meshes["tank"]);
	StaticObject* model3 = new StaticObject(meshes["m1a2"]);
	StaticObject* model4 = new StaticObject(meshes["treeA"], meshes["treeAMid"], meshes["treeALow"]);
	StaticObject* model5 = new StaticObject(meshes["house"]);
	StaticObject* model6 = new StaticObject(meshes["oildrum"]);
	

	WaterNode* waterNode = new WaterNode(VECTOR3D(0, 0, 0));
	waterNode->setFullStatic(true);
	StaticObject* waterObject = new StaticObject(meshes["water"]);
	waterObject->setPosition(-2048, 0, -2048);
	waterObject->setSize(6, 1, 6);
	waterNode->addObject(waterObject);
	waterNode->putCenter();
	scene->water = waterNode;

	//return;

	TerrainNode* terrainNode = new TerrainNode(VECTOR3D(-2048, 0, -2048));
	terrainNode->setFullStatic(true);
	StaticObject* terrainObject = new StaticObject(meshes["terrain"]);
	terrainObject->bindMaterial(mtlMgr->find("terrain_mat"));
	terrainObject->setPosition(0, -200, 0);
	terrainObject->setSize(6, 2, 6);
	terrainNode->addObject(terrainObject);
	terrainNode->prepareTriangles();
	scene->staticRoot->attachChild(terrainNode);
	scene->terrainNode = terrainNode;


	StaticNode* node1 = new StaticNode(VECTOR3D(2, 2, 2));
	node1->setDynamicBatch(false);
	StaticObject* object11 = model2->clone();
	object11->setPosition(-15, -7, 10);
	object11->setRotation(0, 90, 0);
	object11->setSize(0.3, 0.3, 0.3);
	node1->addObject(object11);
	StaticObject* object12 = model2->clone();
	object12->setPosition(15, -7, 10);
	object12->setRotation(0, 90, 0);
	object12->setSize(0.3, 0.3, 0.3);
	node1->addObject(object12);
	StaticObject* object13 = model3->clone();
	object13->setPosition(-30, -7, 70);
	object13->setSize(0.3, 0.3, 0.3);
	node1->addObject(object13);
	StaticObject* object14 = model3->clone();
	object14->setPosition(30, -7, 70);
	object14->setSize(0.3, 0.3, 0.3);
	node1->addObject(object14);

	StaticNode* node2 = new StaticNode(VECTOR3D(10, 2, 2));
	node2->setDynamicBatch(true);
	StaticObject* object6 = box->clone();
	//object6->bindMaterial(mtlMgr->find(DEFAULT_MAT));
	object6->setPosition(3, 3, 3);
	object6->setRotation(0, 30, 0);
	object6->setSize(1, 1, 1);
	node2->addObject(object6);
	StaticObject* object7 = box->clone();
	object7->setPosition(-1, 1, 2);
	object7->setRotation(0, 0, 30);
	object7->setSize(2, 2, 2);
	node2->addObject(object7);
	StaticObject* house = model5->clone();
	house->setPosition(60, 0, 80);
	house->setSize(5, 5, 5);
	node2->addObject(house);

	StaticNode* node3 = new StaticNode(VECTOR3D(5, 10, 0));
	node3->setDynamicBatch(true);
	StaticObject* objectSphere = sphere->clone();
	objectSphere->setSize(2, 2, 2);
	node3->addObject(objectSphere);

	Node* node = new StaticNode(VECTOR3D(1900, 0, 2400));
	Node* modelNode = new StaticNode(VECTOR3D(0, 0, 0));
	modelNode->attachChild(node1);
	modelNode->attachChild(node2);
	node->attachChild(modelNode);
	node->attachChild(node3);
	scene->staticRoot->attachChild(node);

	StaticObject* objectTree = model1->clone();
	objectTree->setPosition(-10, 0, 0);
	objectTree->setSize(3, 3, 3);
	node3->addObject(objectTree);

	
	srand(100);
	InstanceNode* instanceNode1 = new InstanceNode(VECTOR3D(-120, 0, -1520));
	instanceNode1->singleSide = true;
	for (uint i = 0; i < 25; i++) {
		for (uint j = 0; j < 25; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 80;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 1 : 2;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 100 + 100 * (rand() % 100) * 0.01, 0, 200 + i * 100 + 100 * (rand() % 100) * 0.01);
			instanceNode1->addObject(tree);
		}
	}
	InstanceNode* instanceNode2 = new InstanceNode(VECTOR3D(1200, 0, 2000));
	instanceNode2->singleSide = true;
	for (uint i = 0; i < 25; i++) {
		for (uint j = 0; j < 25; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 80;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 1 : 2;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 100 + 100 * (rand() % 100) * 0.01, 0, -(200 + i * 50 + 50 * (rand() % 100) * 0.01));
			instanceNode2->addObject(tree);
		}
	}
	InstanceNode* instanceNode3 = new InstanceNode(VECTOR3D(720, 0, -20));
	instanceNode3->singleSide = true;
	for (uint i = 0; i < 25; i++) {
		for (uint j = 0; j < 25; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 80;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 1 : 2;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(200 + j * 100 + 100 * (rand() % 100) * 0.01, 0, i * 100 + 100 * (rand() % 100) * 0.01);
			instanceNode3->addObject(tree);
		}
	}
	InstanceNode* instanceNode4 = new InstanceNode(VECTOR3D(1920, 0, 1220));
	instanceNode4->singleSide = true;
	for (uint i = 0; i < 25; i++) {
		for (uint j = 0; j < 25; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 80;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 1 : 2;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(-(200 + j * 100 + 100 * (rand() % 100) * 0.01), 0, i * 100 + 100 * (rand() % 100) * 0.01);
			instanceNode4->addObject(tree);
		}
	}
	InstanceNode* instanceNode5 = new InstanceNode(VECTOR3D(-1920, 0, 220));
	instanceNode5->singleSide = true;
	for (uint i = 0; i < 25; i++) {
		for (uint j = 0; j < 25; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 80;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 1 : 2;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 100 + 100 * (rand() % 100) * 0.01, 0, 200 + i * 100 + 100 * (rand() % 100) * 0.01);
			instanceNode5->addObject(tree);
		}
	}
	InstanceNode* instanceNode6 = new InstanceNode(VECTOR3D(-1320, 0, -620));
	instanceNode6->singleSide = true;
	for (uint i = 0; i < 25; i++) {
		for (uint j = 0; j < 25; j++) {
			StaticObject* tree = model1->clone();
			float baseSize = 2;
			//bool changeTree = (rand() % 100) > 80;
			//StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			//float baseSize = changeTree ? 1 : 2;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 120 + 120 * (rand() % 100) * 0.01, 0, 200 + i * 100 + 100 * (rand() % 100) * 0.01);
			instanceNode6->addObject(tree);
		}
	}


	InstanceNode* instanceNode7 = new InstanceNode(VECTOR3D(2000, 0, 2500));
	StaticObject* oil1 = model6->clone();
	oil1->setPosition(15, 0, 15);
	oil1->setSize(5, 5, 5);
	StaticObject* oil2 = model6->clone();
	oil2->setPosition(15, 0, 20);
	oil2->setSize(5, 5, 5);
	StaticObject* oil3 = model6->clone();
	oil3->setPosition(20, 0, 15);
	oil3->setSize(5, 5, 5);
	StaticObject* oil4 = model6->clone();
	oil4->setPosition(20, 0, 20);
	oil4->setSize(5, 5, 5);
	StaticObject* box1 = box->clone();
	box1->setPosition(0, 0, 15);
	box1->setSize(3, 3, 3);
	StaticObject* box2 = box->clone();
	box2->setPosition(0, 0, 20);
	box2->setSize(3, 3, 3);
	StaticObject* box3 = box->clone();
	box3->setPosition(-5, 0, 15);
	box3->setSize(3, 3, 3);
	StaticObject* box4 = box->clone();
	box4->setPosition(-5, 0, 20);
	box4->setSize(3, 3, 3);
	instanceNode7->addObject(oil1);
	instanceNode7->addObject(oil2);
	instanceNode7->addObject(oil3);
	instanceNode7->addObject(oil4);
	instanceNode7->addObject(box1);
	instanceNode7->addObject(box2);
	instanceNode7->addObject(box3);
	instanceNode7->addObject(box4);

	scene->staticRoot->attachChild(instanceNode1);
	scene->staticRoot->attachChild(instanceNode2);
	scene->staticRoot->attachChild(instanceNode3);
	scene->staticRoot->attachChild(instanceNode4);
	scene->staticRoot->attachChild(instanceNode5);
	scene->staticRoot->attachChild(instanceNode6);
	scene->staticRoot->attachChild(instanceNode7);
	
	AnimationNode* animNode1 = new AnimationNode(VECTOR3D(5, 10, 5));
	animNode1->setAnimation(animations["army"]);
	animNode1->getObject()->setSize(0.05, 0.05, 0.05);
	animNode1->getObject()->setPosition(0, -5, -1);
	animNode1->translateNode(5, 0, 15);
	AnimationNode* animNode2 = new AnimationNode(VECTOR3D(5, 10, 5));
	animNode2->setAnimation(animations["army"]);
	animNode2->getObject()->setSize(0.05, 0.05, 0.05);
	animNode2->getObject()->setPosition(0, -5, -1);
	animNode2->translateNode(40, 0, 40);
	animNode2->rotateNodeObject(0, 45, 0);
	AnimationNode* animNode3 = new AnimationNode(VECTOR3D(5, 10, 5));
	animNode3->setAnimation(animations["army"]);
	animNode3->getObject()->setSize(0.05, 0.05, 0.05);
	animNode3->getObject()->setPosition(0, -5, -1);
	animNode3->translateNode(5, 0, 15);
	AnimationNode* animNode4 = new AnimationNode(VECTOR3D(5, 10, 5));
	animNode4->setAnimation(animations["army"]);
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
	delete model5;
	delete model6;

	standObjectsOnGround(scene->staticRoot, terrainNode);
	standObjectsOnGround(scene->animationRoot, terrainNode);
}


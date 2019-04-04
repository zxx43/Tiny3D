#include "simpleApplication.h"
#include "mesh/model.h"
#include "mesh/board.h"
#include "mesh/terrain.h"
#include "mesh/water.h"
#include "object/staticObject.h"
#include "constants/constants.h"
using namespace std;

SimpleApplication::SimpleApplication() {
	screen = NULL;
	waterFrame = NULL;
	sceneFilter = NULL;
	combinedChain = NULL;
	aaFilter = NULL;
	dofBlurFilter = NULL;
	dofChain = NULL;
	ssrChain = NULL;
	ssrBlurFilter = NULL;
	rawScreenFilter = NULL;
}

SimpleApplication::~SimpleApplication() {
	if (screen) delete screen; screen = NULL;
	if (waterFrame) delete waterFrame; waterFrame = NULL;
	if (sceneFilter) delete sceneFilter; sceneFilter = NULL;
	if (combinedChain) delete combinedChain; combinedChain = NULL;
	if (dofBlurFilter) delete dofBlurFilter; dofBlurFilter = NULL;
	if (dofChain) delete dofChain; dofChain = NULL;
	if (aaFilter) delete aaFilter; aaFilter = NULL;
	if (ssrChain) delete ssrChain; ssrChain = NULL;
	if (ssrBlurFilter) delete ssrBlurFilter; ssrBlurFilter = NULL;
	if (rawScreenFilter) delete rawScreenFilter; rawScreenFilter = NULL;
}

void SimpleApplication::resize(int width, int height) {
	if (!render) return;
	Application::resize(width, height);

	int precision = graphQuality > 4.0 ? HIGH_PRE : LOW_PRE;
	int scrPre = (graphQuality > 4.0 || useSsr) ? HIGH_PRE : LOW_PRE;
	int hdrPre = graphQuality > 3.0 ? FLOAT_PRE : precision;

	if (screen) delete screen;
	screen = new FrameBuffer(width, height, hdrPre, 4, false);
	screen->addColorBuffer(precision, 4);
	screen->addColorBuffer(scrPre, 3);
	screen->attachDepthBuffer(scrPre);

	if (waterFrame) delete waterFrame;
	waterFrame = new FrameBuffer(width, height, hdrPre, 4, false);
	waterFrame->addColorBuffer(precision, 4);
	waterFrame->addColorBuffer(scrPre, 3);
	waterFrame->attachDepthBuffer(scrPre);

	if (sceneFilter) delete sceneFilter;
	sceneFilter = new Filter(width, height, true, precision, 4, false);

	if (combinedChain) delete combinedChain;
	if (!useFxaa && !useDof && !useSsr) 
		combinedChain = new FilterChain(width, height, false, precision, 4);
	else {
		combinedChain = new FilterChain(width, height, true, precision, 4);
		if (useFxaa) {
			if (aaFilter) delete aaFilter;
			aaFilter = new Filter(width, height, false, precision, 4);
		}
		if (useDof) {
			if (dofBlurFilter) delete dofBlurFilter;
			dofBlurFilter = new Filter(width * 0.5, height * 0.5, true, LOW_PRE, 4);
			if (dofChain) delete dofChain;
			dofChain = new FilterChain(width, height, useFxaa, precision, 4);
			dofChain->addInputTex(dofBlurFilter->getOutput(0));
			dofChain->addInputTex(combinedChain->getOutputTex(0));
		}
		if (useSsr) {
			if (ssrChain) delete ssrChain;
			ssrChain = new FilterChain(width, height, true, LOW_PRE, 4, false);
			ssrChain->addInputTex(combinedChain->getOutputTex(0));
			ssrChain->addInputTex(waterFrame->getColorBuffer(1));
			ssrChain->addInputTex(waterFrame->getColorBuffer(2));
			ssrChain->addInputTex(waterFrame->getDepthBuffer());

			if (!useFxaa && !useDof) {
				if (rawScreenFilter) delete rawScreenFilter;
				rawScreenFilter = new Filter(width, height, false, precision, 4);
			}

			if (ssrBlurFilter) delete ssrBlurFilter;
			ssrBlurFilter = new Filter(width * 0.5, height * 0.5, true, LOW_PRE, 4);
		}
	}

	if (combinedChain) {
		combinedChain->addInputTex(sceneFilter->getOutput(0));
		combinedChain->addInputTex(screen->getDepthBuffer());
		combinedChain->addInputTex(waterFrame->getColorBuffer(0));
		combinedChain->addInputTex(waterFrame->getDepthBuffer());
		combinedChain->addInputTex(waterFrame->getColorBuffer(1));
		combinedChain->addInputTex(waterFrame->getColorBuffer(2));
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
	if (!sceneFilter || !renderMgr || !AssetManager::assetManager) return;

	if (ssrChain) {
		AssetManager::assetManager->setReflectTexture(ssrBlurFilter->getOutput(0));
		//AssetManager::assetManager->setReflectTexture(ssrChain->getOutputTex(0));
	} else {
		if (renderMgr->reflectBuffer) {
			renderMgr->renderReflect(render, scene);
			AssetManager::assetManager->setReflectTexture(renderMgr->reflectBuffer->getColorBuffer(0));
		}
	}
	
	///*
	renderMgr->renderShadow(render, scene);
	render->setFrameBuffer(screen);
	renderMgr->renderScene(render, scene);
	renderMgr->drawDeferred(render, scene, screen, sceneFilter);

	render->setFrameBuffer(waterFrame);
	waterFrame->getDepthBuffer()->copyDataFrom(screen->getDepthBuffer());
	renderMgr->renderWater(render, scene);

	renderMgr->drawCombined(render, scene, combinedChain->input, combinedChain->output);

	if (ssrChain) {
		if (rawScreenFilter)
			renderMgr->drawScreenFilter(render, scene, "screen", combinedChain->getOutFrameBuffer(), rawScreenFilter);
		renderMgr->drawSSRFilter(render, scene, "ssr", ssrChain->input, ssrChain->output);
		renderMgr->drawScreenFilter(render, scene, "mean", ssrChain->getOutFrameBuffer(), ssrBlurFilter);
	}

	Filter* lastFilter = combinedChain->output;
	if (useDof) {
		renderMgr->drawScreenFilter(render, scene, "blur", combinedChain->getOutFrameBuffer(), dofBlurFilter);
		renderMgr->drawScreenFilter(render, scene, "dof", dofChain->input, dofChain->output);
		lastFilter = dofChain->output;
	}
	if (useFxaa)
		renderMgr->drawScreenFilter(render, scene, "fxaa", lastFilter->getFrameBuffer(), aaFilter);
	//*/
	//if (AssetManager::assetManager->texAlt)
	//	renderMgr->drawTexture2Screen(render, scene, AssetManager::assetManager->texAlt->texId);

	render->finishDraw();
}

void SimpleApplication::init() {
	Application::init();
	initScene();
}

void SimpleApplication::moveKey() {
	Application::moveKey();
	updateMovement();
}

void SimpleApplication::moveByDir(int dir) {
	Application::moveByDir(dir);
	updateMovement();
}

void SimpleApplication::updateMovement() {
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
		}
		else if (scene->water) {
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
		scene->terrainNode->standObjectsOnGround(animNode);
	//*/
	/*
	static int time = 1;
	if (currentTime - startTime > 10000 * time && scene->staticRoot->children.size() > 1) {
		scene->staticRoot->detachChild(scene->staticRoot->children[1])->pushToRemove();
		time++;
	}
	//*/
	scene->updateNodes();
	if (!useSsr)
		scene->updateReflectCamera();
}

void SimpleApplication::initScene() {
	AssetManager* assetMgr = AssetManager::assetManager;
	MaterialManager* mtlMgr = MaterialManager::materials;

	// Load meshes
	assetMgr->addMesh("tree", new Model("models/firC.obj", "models/firC.mtl", 2, true));
	assetMgr->addMesh("treeMid", new Model("models/firC_mid.obj", "models/firC_mid.mtl", 2, true));
	assetMgr->addMesh("treeLow", new Model("models/fir_mesh.obj", "models/fir_mesh.mtl", 3, true));
	assetMgr->addMesh("treeA", new Model("models/treeA.obj", "models/treeA.mtl", 2, true));
	assetMgr->addMesh("treeAMid", new Model("models/treeA_mid.obj", "models/treeA_mid.mtl", 2, true));
	assetMgr->addMesh("treeALow", new Model("models/treeA_low.obj", "models/treeA_low.mtl", 2, true));
	assetMgr->addMesh("grass1", new Model("models/grass1.obj", "models/grass1.mtl", 2, true));
	assetMgr->addMesh("grass2", new Model("models/grass2.obj", "models/grass2.mtl", 2, true));
	assetMgr->addMesh("tank", new Model("models/tank.obj", "models/tank.mtl", 3, true));
	assetMgr->addMesh("m1a2", new Model("models/m1a2.obj", "models/m1a2.mtl", 2, true));
	assetMgr->addMesh("house", new Model("models/house.obj", "models/house.mtl", 2, true));
	assetMgr->addMesh("oildrum", new Model("models/oildrum.obj", "models/oildrum.mtl", 3, true));
	assetMgr->addMesh("terrain", new Terrain("terrain/Terrain.raw"));
	assetMgr->addMesh("water", new Water(1024, 16));
	assetMgr->addAnimation("army", new Animation("models/ArmyPilot.dae"));

	// Load textures
	assetMgr->addTexture2Alt("cube.bmp");
	assetMgr->addTexture2Alt("ground.bmp");
	assetMgr->addTexture2Array("ground_g.bmp");
	assetMgr->addTexture2Array("ground_r.bmp");
	assetMgr->addTexture2Array("ground_s.bmp");
	assetMgr->addTexture2Alt("sand.bmp");
	assetMgr->addTexture2Alt("tree.bmp");
	assetMgr->addTexture2Alt("treeA.bmp");
	assetMgr->initTextureAtlas();
	assetMgr->initTextureArray();

	// Create materials
	Material* boxMat = new Material("box_mat");
	boxMat->tex1 = "cube.bmp";
	boxMat->ambient = VECTOR3D(0.4, 0.4, 0.4); 
	boxMat->diffuse = VECTOR3D(0.6, 0.6, 0.6);
	mtlMgr->add(boxMat);

	Material* grassMat = new Material("grass_mat");
	grassMat->tex1 = "ground.bmp";
	mtlMgr->add(grassMat);

	Material* sandMat = new Material("sand_mat");
	sandMat->tex1 = "sand.bmp";
	mtlMgr->add(sandMat);
	
	Material* terrainMat = new Material("terrain_mat");
	terrainMat->useArray = true;
	terrainMat->texOfs1.x = assetMgr->findTextureInArray("ground_g.bmp");
	terrainMat->texOfs1.y = assetMgr->findTextureInArray("ground_r.bmp");
	terrainMat->texOfs1.z = assetMgr->findTextureInArray("ground_s.bmp");
	mtlMgr->add(terrainMat);
	
	Material* billboardTreeMat = new Material("billboard_tree_mat");
	billboardTreeMat->tex1 = "tree.bmp";
	mtlMgr->add(billboardTreeMat);

	Material* billboardTreeAMat = new Material("billboard_treeA_mat");
	billboardTreeAMat->tex1 = "treeA.bmp";
	mtlMgr->add(billboardTreeAMat);

	assetMgr->createMaterialTextureAtlas(mtlMgr);

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
	model1->setBillboard(5, 10, mtlMgr->find("billboard_tree_mat"));
	StaticObject* model2 = new StaticObject(meshes["tank"]);
	StaticObject* model3 = new StaticObject(meshes["m1a2"]);
	StaticObject* model4 = new StaticObject(meshes["treeA"], meshes["treeAMid"], meshes["billboard"]);
	model4->setBillboard(13, 14, mtlMgr->find("billboard_treeA_mat"));
	StaticObject* model5 = new StaticObject(meshes["house"]);
	StaticObject* model6 = new StaticObject(meshes["oildrum"]);
	StaticObject* model7 = new StaticObject(meshes["grass1"], NULL, NULL);
	model7->detailLevel = 1;
	StaticObject* model8 = new StaticObject(meshes["grass2"], NULL, NULL);
	model8->detailLevel = 1;
	int grassShadowLevel = graphQuality > 4 ? 1 : 0;
	bool grassDynamic = true;

	//return;
	scene->createSky();
	scene->createWater(VECTOR3D(-2048, 0, -2048), VECTOR3D(6, 1, 6));
	scene->createTerrain(VECTOR3D(-2048, -200, -2048), VECTOR3D(6, 1.5, 6));

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

	Node* node = new StaticNode(VECTOR3D(-1200, 0, 3300));
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
	InstanceNode* instanceNode1 = new InstanceNode(VECTOR3D(900, 0, 600));
	instanceNode1->setSingle(true);
	instanceNode1->setSimple(true);
	instanceNode1->detailLevel = 4;
	for (int i = -12; i < 12; i++) {
		for (int j = -12; j < 12; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 95;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 200 + 200 * (rand() % 100) * 0.01, 0, i * 200 + 200 * (rand() % 100) * 0.01);
			instanceNode1->addObject(tree);
		}
	}
	InstanceNode* instanceNode2 = new InstanceNode(VECTOR3D(2746, 0, 2565));
	instanceNode2->setSingle(true);
	instanceNode2->setSimple(true);
	instanceNode2->detailLevel = 4;
	for (int i = -12; i < 12; i++) {
		for (int j = -12; j < 12; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 95;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 100 + 100 * (rand() % 100) * 0.01, 0, i * 100 + 100 * (rand() % 100) * 0.01);
			instanceNode2->addObject(tree);
		}
	}
	InstanceNode* instanceNode3 = new InstanceNode(VECTOR3D(-700, 0, 1320));
	instanceNode3->setSingle(true);
	instanceNode3->setSimple(true);
	instanceNode3->detailLevel = 4;
	for (int i = -12; i < 12; i++) {
		for (int j = -12; j < 12; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 95;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 100 + 100 * (rand() % 100) * 0.01, 0, i * 100 + 100 * (rand() % 100) * 0.01);
			instanceNode3->addObject(tree);
		}
	}
	InstanceNode* instanceNode4 = new InstanceNode(VECTOR3D(-750, 0, -500));
	instanceNode4->setSingle(true);
	instanceNode4->setSimple(true);
	instanceNode4->detailLevel = 4;
	for (int i = -12; i < 12; i++) {
		for (int j = -12; j < 12; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 95;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 100 + 100 * (rand() % 100) * 0.01, 0, i * 100 + 100 * (rand() % 100) * 0.01);
			instanceNode4->addObject(tree);
		}
	}
	InstanceNode* instanceNode5 = new InstanceNode(VECTOR3D(2100, 0, -600));
	instanceNode5->setSingle(true);
	instanceNode5->setSimple(true);
	instanceNode5->detailLevel = 4;
	for (int i = -12; i < 12; i++) {
		for (int j = -12; j < 12; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > 95;
			StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 110 + 110 * (rand() % 100) * 0.01, 0, i * 110 + 110 * (rand() % 100) * 0.01);
			instanceNode5->addObject(tree);
		}
	}
	InstanceNode* instanceNode6 = new InstanceNode(VECTOR3D(800, 0, 2000));
	instanceNode6->setSingle(true);
	instanceNode6->setSimple(true);
	instanceNode6->detailLevel = 4;
	for (int i = -12; i < 12; i++) {
		for (int j = -12; j < 12; j++) {
			StaticObject* tree = model1->clone();
			float baseSize = 5;
			//bool changeTree = (rand() % 100) > 90;
			//StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			//float baseSize = changeTree ? 2 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * 150 + 150 * (rand() % 100) * 0.01, 0, i * 150 + 150 * (rand() % 100) * 0.01);
			instanceNode6->addObject(tree);
		}
	}

	InstanceNode* instanceNode8 = new InstanceNode(VECTOR3D(2500, 0, 860));
	instanceNode8->setSingle(true);
	instanceNode8->shadowLevel = grassShadowLevel;
	instanceNode8->dynamic = grassDynamic;
	instanceNode8->setGroup(true);
	instanceNode8->setSimple(true);
	instanceNode8->setGrass(true);
	int grassSize = 50;
	for (int i = -grassSize; i < grassSize; i++) {
		for (int j = -grassSize; j < grassSize; j++) {
			StaticObject* grass = model7->clone();
			float baseSize = 5;
			float size = (rand() % 100 * 0.01) * 4 + baseSize;
			float spread = 30;

			grass->setSize(size, size, size);
			grass->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			grass->setPosition(j * spread + spread * (rand() % 100) * 0.01, 0, i * spread + spread * (rand() % 100) * 0.01);
			instanceNode8->addObject(grass);
		}
	}
	InstanceNode* instanceNode9 = new InstanceNode(VECTOR3D(1300, 0, 2400));
	instanceNode9->setSingle(true);
	instanceNode9->shadowLevel = grassShadowLevel;
	instanceNode9->dynamic = grassDynamic;
	instanceNode9->setGroup(true);
	instanceNode9->setSimple(true);
	instanceNode9->setGrass(true);
	for (int i = -grassSize; i < grassSize; i++) {
		for (int j = -grassSize; j < grassSize; j++) {
			StaticObject* grass = model7->clone();
			float baseSize = 5;
			float size = (rand() % 100 * 0.01) * 4 + baseSize;
			float spread = 30;

			grass->setSize(size, size, size);
			grass->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			grass->setPosition(j * spread + spread * (rand() % 100) * 0.01, 0, i * spread + spread * (rand() % 100) * 0.01);
			instanceNode9->addObject(grass);
		}
	}
	InstanceNode* instanceNode10 = new InstanceNode(VECTOR3D(500, 0, -300));
	instanceNode10->setSingle(true);
	instanceNode10->shadowLevel = grassShadowLevel;
	instanceNode10->dynamic = grassDynamic;
	instanceNode10->setGroup(true);
	instanceNode10->setSimple(true);
	instanceNode10->setGrass(true);
	for (int i = -grassSize; i < grassSize; i++) {
		for (int j = -grassSize; j < grassSize; j++) {
			StaticObject* grass = model7->clone();
			float baseSize = 5;
			float size = (rand() % 100 * 0.01) * 4 + baseSize;
			float spread = 30;

			grass->setSize(size, size, size);
			grass->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			grass->setPosition(j * spread + spread * (rand() % 100) * 0.01, 0, i * spread + spread * (rand() % 100) * 0.01);
			instanceNode10->addObject(grass);
		}
	}
	InstanceNode* instanceNode11 = new InstanceNode(VECTOR3D(1300, 0, 1360));
	instanceNode11->setSingle(true);
	instanceNode11->shadowLevel = grassShadowLevel;
	instanceNode11->dynamic = grassDynamic;
	instanceNode11->setGroup(true);
	instanceNode11->setSimple(true);
	instanceNode11->setGrass(true);
	for (int i = -grassSize; i < grassSize; i++) {
		for (int j = -grassSize; j < grassSize; j++) {
			StaticObject* grass = model7->clone();
			float baseSize = 5;
			float size = (rand() % 100 * 0.01) * 4 + baseSize;
			float spread = 30;

			grass->setSize(size, size, size);
			grass->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			grass->setPosition(j * spread + spread * (rand() % 100) * 0.01, 0, i * spread + spread * (rand() % 100) * 0.01);
			instanceNode11->addObject(grass);
		}
	}
	InstanceNode* instanceNode12 = new InstanceNode(VECTOR3D(-500, 0, 2500));
	instanceNode12->setSingle(true);
	instanceNode12->shadowLevel = grassShadowLevel;
	instanceNode12->dynamic = grassDynamic;
	instanceNode12->setGroup(true);
	instanceNode12->setSimple(true);
	instanceNode12->setGrass(true);
	for (int i = -grassSize; i < grassSize; i++) {
		for (int j = -grassSize; j < grassSize; j++) {
			StaticObject* grass = model7->clone();
			float baseSize = 5;
			float size = (rand() % 100 * 0.01) * 4 + baseSize;
			float spread = 30;

			grass->setSize(size, size, size);
			grass->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			grass->setPosition(j * spread + spread * (rand() % 100) * 0.01, 0, i * spread + spread * (rand() % 100) * 0.01);
			instanceNode12->addObject(grass);
		}
	}
	InstanceNode* instanceNode13 = new InstanceNode(VECTOR3D(2500, 0, 2500));
	instanceNode13->setSingle(true);
	instanceNode13->shadowLevel = grassShadowLevel;
	instanceNode13->dynamic = grassDynamic;
	instanceNode13->setGroup(true);
	instanceNode13->setSimple(true);
	instanceNode13->setGrass(true);
	for (int i = -grassSize; i < grassSize; i++) {
		for (int j = -grassSize; j < grassSize; j++) {
			StaticObject* grass = model7->clone();
			float baseSize = 5;
			float size = (rand() % 100 * 0.01) * 4 + baseSize;
			float spread = 30;

			grass->setSize(size, size, size);
			grass->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			grass->setPosition(j * spread + spread * (rand() % 100) * 0.01, 0, i * spread + spread * (rand() % 100) * 0.01);
			instanceNode13->addObject(grass);
		}
	}
	InstanceNode* instanceNode14 = new InstanceNode(VECTOR3D(2500, 0, -450));
	instanceNode14->setSingle(true);
	instanceNode14->shadowLevel = grassShadowLevel;
	instanceNode14->dynamic = grassDynamic;
	instanceNode14->setGroup(true);
	instanceNode14->setSimple(true);
	instanceNode14->setGrass(true);
	for (int i = -grassSize; i < grassSize; i++) {
		for (int j = -grassSize; j < grassSize; j++) {
			StaticObject* grass = model7->clone();
			float baseSize = 5;
			float size = (rand() % 100 * 0.01) * 4 + baseSize;
			float spread = 30;

			grass->setSize(size, size, size);
			grass->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			grass->setPosition(j * spread + spread * (rand() % 100) * 0.01, 0, i * spread + spread * (rand() % 100) * 0.01);
			instanceNode14->addObject(grass);
		}
	}
	InstanceNode* instanceNode15 = new InstanceNode(VECTOR3D(-520, 0, -530));
	instanceNode15->setSingle(true);
	instanceNode15->shadowLevel = grassShadowLevel;
	instanceNode15->dynamic = grassDynamic;
	instanceNode15->setGroup(true);
	instanceNode15->setSimple(true);
	instanceNode15->setGrass(true);
	for (int i = -grassSize; i < grassSize; i++) {
		for (int j = -grassSize; j < grassSize; j++) {
			StaticObject* grass = model7->clone();
			float baseSize = 5;
			float size = (rand() % 100 * 0.01) * 4 + baseSize;
			float spread = 30;

			grass->setSize(size, size, size);
			grass->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			grass->setPosition(j * spread + spread * (rand() % 100) * 0.01, 0, i * spread + spread * (rand() % 100) * 0.01);
			instanceNode15->addObject(grass);
		}
	}


	InstanceNode* instanceNode7 = new InstanceNode(VECTOR3D(3500, 0, 200));
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
	scene->staticRoot->attachChild(instanceNode8);
	scene->staticRoot->attachChild(instanceNode9);
	scene->staticRoot->attachChild(instanceNode10);
	scene->staticRoot->attachChild(instanceNode11);
	scene->staticRoot->attachChild(instanceNode12);
	scene->staticRoot->attachChild(instanceNode13);
	scene->staticRoot->attachChild(instanceNode14);
	scene->staticRoot->attachChild(instanceNode15);
	
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
	delete model7;
	delete model8;

	scene->terrainNode->standObjectsOnGround(scene->staticRoot);
	scene->terrainNode->standObjectsOnGround(scene->animationRoot);
	scene->inited = true;
}


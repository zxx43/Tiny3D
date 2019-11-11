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
	ssgChain = NULL;
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
	if (ssgChain) delete ssgChain; ssgChain = NULL;
}

void SimpleApplication::resize(int width, int height) {
	if (!render) return;

	int precision = graphQuality > 4.0 ? HIGH_PRE : LOW_PRE;
	int scrPre = (graphQuality > 4.0 || useSsr) ? HIGH_PRE : LOW_PRE;
	int hdrPre = graphQuality > 3.0 ? FLOAT_PRE : precision;
	int depthPre = LOW_PRE;

	Application::resize(width, height);

	if (screen) delete screen;
	screen = new FrameBuffer(width, height, hdrPre, 4, false); // texBuffer
	screen->addColorBuffer(precision, 4); // matBuffer
	screen->addColorBuffer(scrPre, 4); // normal-grassBuffer
	screen->addColorBuffer(scrPre, 3); // rough-metalBuffer
	screen->attachDepthBuffer(depthPre); // depthBuffer

	if (waterFrame) delete waterFrame;
	waterFrame = new FrameBuffer(width, height, hdrPre, 4, false);
	waterFrame->addColorBuffer(precision, 4);
	waterFrame->addColorBuffer(scrPre, 3);
	waterFrame->attachDepthBuffer(depthPre);

	if (sceneFilter) delete sceneFilter;
	sceneFilter = new Filter(width, height, true, precision, 4, false);

	/*
	if (ssgChain) delete ssgChain;
	ssgChain = new FilterChain(width, height, true, hdrPre, 4, false);
	ssgChain->addInputTex("colorBuffer", sceneFilter->getOutput(0));
	ssgChain->addInputTex("normalGrassBuffer", screen->getColorBuffer(2));
	ssgChain->addInputTex("depthBuffer", screen->getDepthBuffer());
	*/

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
		if (ssgChain)
			combinedChain->addInputTex(ssgChain->getOutputTex(0));
		else
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
	if (ssgChain)
		renderMgr->drawSSGFilter(render, scene, "ssg", ssgChain->input, ssgChain->output);

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

	render->finishDraw();
}

void SimpleApplication::init() {
	Application::init();
	initScene();
	printf("Init ok!\n");
	render->getError();
}

void SimpleApplication::moveKey(float velocity) {
	Application::moveKey(velocity);
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
		vec3 cp = scene->mainCamera->position;
		int bx, bz;
		scene->terrainNode->caculateBlock(cp.x, cp.z, bx, bz);
		scene->updateVisualTerrain(bx, bz, 40, 40);
		if (scene->terrainNode->cauculateY(bx, bz, cp.x, cp.z, cp.y)) {
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
	if (!useSsr) scene->updateReflectCamera();
}

void SimpleApplication::initScene() {
	AssetManager* assetMgr = AssetManager::assetManager;
	MaterialManager* mtlMgr = MaterialManager::materials;

	// Load meshes
	assetMgr->addMesh("tree", new Model("models/firC.obj", "models/firC.mtl", 2));
	assetMgr->addMesh("treeMid", new Model("models/firC_mid.obj", "models/firC_mid.mtl", 2));
	assetMgr->addMesh("treeLow", new Model("models/fir_mesh.obj", "models/fir_mesh.mtl", 3));
	assetMgr->addMesh("treeA", new Model("models/treeA.obj", "models/treeA.mtl", 2));
	assetMgr->addMesh("treeAMid", new Model("models/treeA_mid.obj", "models/treeA_mid.mtl", 2));
	assetMgr->addMesh("treeALow", new Model("models/treeA_low.obj", "models/treeA_low.mtl", 2));
	assetMgr->addMesh("tank", new Model("models/tank.obj", "models/tank.mtl", 3));
	assetMgr->addMesh("m1a2", new Model("models/m1a2.obj", "models/m1a2.mtl", 2));
	assetMgr->addMesh("house", new Model("models/house.obj", "models/house.mtl", 2));
	assetMgr->addMesh("oildrum", new Model("models/oildrum.obj", "models/oildrum.mtl", 3));
	assetMgr->addMesh("rock", new Model("models/sharprockfree.obj", "models/sharprockfree.mtl", 2));
	assetMgr->addMesh("terrain", new Terrain("terrain/Terrain.raw"));
	assetMgr->addMesh("water", new Water(1024, 16));
	assetMgr->addAnimation("army", new Animation("models/ArmyPilot.dae"));

	// Load textures
	assetMgr->addTextureBindless("cube.bmp", true);
	assetMgr->addTextureBindless("ground_n.bmp", false);
	assetMgr->addTextureBindless("ground.bmp", true);
	assetMgr->addTextureBindless("ground_norm.bmp", false);
	assetMgr->addTextureBindless("ground_g.bmp", true);
	assetMgr->addTextureBindless("ground_r.bmp", true);
	assetMgr->addTextureBindless("ground_s.bmp", true);
	assetMgr->addTextureBindless("rnormal.bmp", false);
	assetMgr->addTextureBindless("sand.bmp", true);
	assetMgr->addTextureBindless("tree.bmp", true);
	assetMgr->addTextureBindless("treeA.bmp", true);
	assetMgr->addTextureBindless("mixedmoss-albedo2.bmp", true);
	assetMgr->addTextureBindless("mixedmoss-normal2.bmp", false);
	assetMgr->addTextureBindless("mixedmoss-roughness.bmp", false);
	assetMgr->addTextureBindless("mixedmoss-metalness.bmp", false);
	assetMgr->addTextureBindless("rustediron2_basecolor.bmp", true);
	assetMgr->addTextureBindless("rustediron2_normal.bmp", false);
	assetMgr->addTextureBindless("rustediron2_roughness.bmp", false);
	assetMgr->addTextureBindless("rustediron2_metallic.bmp", false);
	assetMgr->addTextureBindless("grass1-albedo3.bmp", true);
	assetMgr->addTextureBindless("grass1-normal1-dx.bmp", false);
	//assetMgr->addTextureBindless("grass1-rough.bmp", false);
	//assetMgr->addTextureBindless("grass1-metalness.bmp", false);
	assetMgr->addDistortionTex("distortion.bmp");

	// Create materials
	Material* boxMat = new Material("box_mat");
	boxMat->tex1 = "cube.bmp";
	boxMat->tex2 = "ground_n.bmp";
	boxMat->ambient = vec3(0.4, 0.4, 0.4); 
	boxMat->diffuse = vec3(0.6, 0.6, 0.6);
	mtlMgr->add(boxMat);

	Material* grassMat = new Material("grass_mat");
	grassMat->tex1 = "ground.bmp";
	grassMat->tex2 = "ground_norm.bmp";
	mtlMgr->add(grassMat);

	Material* sandMat = new Material("sand_mat");
	sandMat->tex1 = "sand.bmp";
	mtlMgr->add(sandMat);

	Material* ironMat = new Material("iron_mat");
	ironMat->tex1 = "rustediron2_basecolor.bmp";
	ironMat->tex2 = "rustediron2_normal.bmp";
	ironMat->tex3 = "rustediron2_roughness.bmp";
	ironMat->tex4 = "rustediron2_metallic.bmp";
	mtlMgr->add(ironMat);
	
	Material* terrainMat = new Material("terrain_mat");
	terrainMat->prepared = true;
	terrainMat->texids.x = assetMgr->findTextureBindless("grass1-albedo3.bmp");
	terrainMat->texids.y = assetMgr->findTextureBindless("ground_r.bmp");
	terrainMat->texids.z = assetMgr->findTextureBindless("ground_s.bmp");
	terrainMat->texids.w = assetMgr->findTextureBindless("grass1-normal1-dx.bmp");
	//terrainMat->exTexids.x = assetMgr->findTextureBindless("grass1-rough.bmp");
	//terrainMat->exTexids.y = assetMgr->findTextureBindless("grass1-metalness.bmp");
	mtlMgr->add(terrainMat);
	
	Material* billboardTreeMat = new Material("billboard_tree_mat");
	billboardTreeMat->tex1 = "tree.bmp";
	mtlMgr->add(billboardTreeMat);

	Material* billboardTreeAMat = new Material("billboard_treeA_mat");
	billboardTreeAMat->tex1 = "treeA.bmp";
	mtlMgr->add(billboardTreeAMat);

	assetMgr->initTextureBindless(mtlMgr);
	render->setTextureBindless2Shaders(assetMgr->texBld);

	// Create Nodes
	map<string, Mesh*> meshes = assetMgr->meshes;
	map<string, Animation*> animations = assetMgr->animations;

	StaticObject box(meshes["box"]); 
	box.bindMaterial(mtlMgr->find("box_mat"));
	StaticObject sphere(meshes["sphere"]); 
	sphere.bindMaterial(mtlMgr->find("iron_mat"));
	StaticObject board(meshes["board"]);
	StaticObject quad(meshes["quad"]);

	StaticObject model1(meshes["tree"], meshes["treeMid"], meshes["billboard"]);
	model1.detailLevel = 4;
	model1.setBillboard(5, 10, mtlMgr->find("billboard_tree_mat"));
	StaticObject model2(meshes["tank"]);
	StaticObject model3(meshes["m1a2"]);
	StaticObject model4(meshes["treeA"], meshes["treeAMid"], meshes["billboard"]);
	model4.detailLevel = 4;
	model4.setBillboard(13, 14, mtlMgr->find("billboard_treeA_mat"));
	StaticObject model5(meshes["house"]);
	StaticObject model6(meshes["oildrum"]);
	StaticObject model9(meshes["rock"], meshes["rock"], NULL);
	bool treeSimple = false;

	//return;
	scene->createSky();
	scene->createWater(vec3(-2048, 0, -2048), vec3(6, 1, 6));
	scene->createTerrain(vec3(-2048, -200, -2048), vec3(6, 1.8, 6));

	StaticNode* node1 = new StaticNode(vec3(2, 2, 2));
	node1->setDynamicBatch(false);
	StaticObject* object11 = model2.clone();
	object11->setPosition(-15, -7, 10);
	object11->setRotation(0, 90, 0);
	object11->setSize(0.3, 0.3, 0.3);
	node1->addObject(scene, object11);
	StaticObject* object12 = model2.clone();
	object12->setPosition(15, -7, 10);
	object12->setRotation(0, 90, 0);
	object12->setSize(0.3, 0.3, 0.3);
	node1->addObject(scene, object12);
	StaticObject* object13 = model3.clone();
	object13->setPosition(-30, -7, 70);
	object13->setSize(0.3, 0.3, 0.3);
	node1->addObject(scene, object13);
	StaticObject* object14 = model3.clone();
	object14->setPosition(30, -7, 70);
	object14->setSize(0.3, 0.3, 0.3);
	node1->addObject(scene, object14);

	StaticNode* node2 = new StaticNode(vec3(10, 2, 2));
	node2->setDynamicBatch(true);
	StaticObject* object6 = box.clone();
	//object6->bindMaterial(mtlMgr->find(DEFAULT_MAT));
	object6->setPosition(0, 0, 0);
	object6->setRotation(0, 30, 0);
	object6->setSize(4, 4, 4);
	node2->addObject(scene, object6);
	StaticObject* object7 = box.clone();
	object7->setPosition(-5, 5, 6);
	object7->setRotation(0, 0, 30);
	object7->setSize(5, 5, 5);
	node2->addObject(scene, object7);
	StaticObject* house = model5.clone();
	house->setPosition(60, 0, 80);
	house->setSize(5, 5, 5);
	node2->addObject(scene, house);

	StaticNode* node3 = new StaticNode(vec3(25, 10, 0));
	node3->setDynamicBatch(false);
	StaticObject* objectSphere = sphere.clone();
	objectSphere->setSize(10, 10, 10);
	node3->addObject(scene, objectSphere);

	Node* node = new StaticNode(vec3(-1200, 0, 3300));
	Node* modelNode = new StaticNode(vec3(0, 0, 0));
	modelNode->attachChild(node1);
	modelNode->attachChild(node2);
	node->attachChild(modelNode);
	node->attachChild(node3);
	scene->staticRoot->attachChild(node);

	StaticObject* objectRock = model9.clone();
	objectRock->setPosition(-60, 0, 0);
	objectRock->setSize(0.1, 0.1, 0.1);
	node3->addObject(scene, objectRock);

	int treeScale = 12;
	int treeSpace = 180;
	int treePerc = 80;
	
	srand(100);
	InstanceNode* instanceNode1 = new InstanceNode(vec3(900, 0, 600));
	instanceNode1->setSimple(treeSimple);
	instanceNode1->detailLevel = 4;
	for (int i = -treeScale; i < treeScale; i++) {
		for (int j = -treeScale; j < treeScale; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > treePerc;
			StaticObject* tree = changeTree ? model4.clone() : model1.clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode1->addObject(scene, tree);
		}
	}
	treeSpace = 100;
	InstanceNode* instanceNode2 = new InstanceNode(vec3(2746, 0, 2565));
	instanceNode2->setSimple(treeSimple);
	instanceNode2->detailLevel = 4;
	for (int i = -treeScale; i < treeScale; i++) {
		for (int j = -treeScale; j < treeScale; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > treePerc;
			StaticObject* tree = changeTree ? model4.clone() : model1.clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode2->addObject(scene, tree);
		}
	}
	InstanceNode* instanceNode3 = new InstanceNode(vec3(-700, 0, 1320));
	instanceNode3->setSimple(treeSimple);
	instanceNode3->detailLevel = 4;
	for (int i = -treeScale; i < treeScale; i++) {
		for (int j = -treeScale; j < treeScale; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > treePerc;
			StaticObject* tree = changeTree ? model4.clone() : model1.clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode3->addObject(scene, tree);
		}
	}
	InstanceNode* instanceNode4 = new InstanceNode(vec3(-750, 0, -500));
	instanceNode4->setSimple(treeSimple);
	instanceNode4->detailLevel = 4;
	for (int i = -treeScale; i < treeScale; i++) {
		for (int j = -treeScale; j < treeScale; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > treePerc;
			StaticObject* tree = changeTree ? model4.clone() : model1.clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode4->addObject(scene, tree);
		}
	}
	InstanceNode* instanceNode5 = new InstanceNode(vec3(2100, 0, -600));
	instanceNode5->setSimple(treeSimple);
	instanceNode5->detailLevel = 4;
	for (int i = -treeScale; i < treeScale; i++) {
		for (int j = -treeScale; j < treeScale; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > treePerc;
			StaticObject* tree = changeTree ? model4.clone() : model1.clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode5->addObject(scene, tree);
		}
	}
	treeSpace = 150;
	InstanceNode* instanceNode6 = new InstanceNode(vec3(800, 0, 2000));
	instanceNode6->setSimple(treeSimple);
	instanceNode6->detailLevel = 4;
	for (int i = -treeScale; i < treeScale; i++) {
		for (int j = -treeScale; j < treeScale; j++) {
			StaticObject* tree = model1.clone();
			float baseSize = 5;
			//bool changeTree = (rand() % 100) > 90;
			//StaticObject* tree = changeTree ? model4->clone() : model1->clone();
			//float baseSize = changeTree ? 2 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode6->addObject(scene, tree);
		}
	}

	InstanceNode* instanceNode7 = new InstanceNode(vec3(3500, 0, 200));
	StaticObject* oil1 = model6.clone();
	oil1->setPosition(30, 0, 30);
	oil1->setSize(10, 10, 10);
	StaticObject* oil2 = model6.clone();
	oil2->setPosition(30, 0, 40);
	oil2->setSize(10, 10, 10);
	StaticObject* oil3 = model6.clone();
	oil3->setPosition(40, 0, 30);
	oil3->setSize(10, 10, 10);
	StaticObject* oil4 = model6.clone();
	oil4->setPosition(40, 0, 40);
	oil4->setSize(10, 10, 10);
	StaticObject* box1 = box.clone();
	box1->setPosition(0, 0, 30);
	box1->setSize(6, 6, 6);
	StaticObject* box2 = box.clone();
	box2->setPosition(0, 0, 40);
	box2->setSize(6, 6, 6);
	StaticObject* box3 = box.clone();
	box3->setPosition(-10, 0, 30);
	box3->setSize(6, 6, 6);
	StaticObject* box4 = box.clone();
	box4->setPosition(-10, 0, 40);
	box4->setSize(6, 6, 6);
	instanceNode7->addObject(scene, oil1);
	instanceNode7->addObject(scene, oil2);
	instanceNode7->addObject(scene, oil3);
	instanceNode7->addObject(scene, oil4);
	instanceNode7->addObject(scene, box1);
	instanceNode7->addObject(scene, box2);
	instanceNode7->addObject(scene, box3);
	instanceNode7->addObject(scene, box4);

	scene->staticRoot->attachChild(instanceNode1);
	scene->staticRoot->attachChild(instanceNode2);
	scene->staticRoot->attachChild(instanceNode3);
	scene->staticRoot->attachChild(instanceNode4);
	scene->staticRoot->attachChild(instanceNode5);
	scene->staticRoot->attachChild(instanceNode6);
	scene->staticRoot->attachChild(instanceNode7);

	AnimationNode* animNode1 = new AnimationNode(vec3(5, 10, 5));
	animNode1->setAnimation(scene, animations["army"]);
	animNode1->getObject()->setSize(0.05, 0.05, 0.05);
	animNode1->getObject()->setPosition(0, -5, -1);
	animNode1->translateNode(5, 0, 15);
	AnimationNode* animNode2 = new AnimationNode(vec3(5, 10, 5));
	animNode2->setAnimation(scene, animations["army"]);
	animNode2->getObject()->setSize(0.05, 0.05, 0.05);
	animNode2->getObject()->setPosition(0, -5, -1);
	animNode2->translateNode(40, 0, 40);
	animNode2->rotateNodeObject(0, 45, 0);
	AnimationNode* animNode3 = new AnimationNode(vec3(5, 10, 5));
	animNode3->setAnimation(scene, animations["army"]);
	animNode3->getObject()->setSize(0.05, 0.05, 0.05);
	animNode3->getObject()->setPosition(0, -5, -1);
	animNode3->translateNode(5, 0, 15);
	AnimationNode* animNode4 = new AnimationNode(vec3(5, 10, 5));
	animNode4->setAnimation(scene, animations["army"]);
	animNode4->getObject()->setSize(0.05, 0.05, 0.05);
	animNode4->getObject()->setPosition(0, -5, -1);
	animNode4->translateNode(40, 0, 40);
	animNode4->rotateNodeObject(0, 90, 0);

	Node* animNode = new StaticNode(vec3(0, 0, 0));
	animNode->attachChild(animNode1);
	animNode->attachChild(animNode2);
	scene->animationRoot->attachChild(animNode);
	Node* animNodeSub = new StaticNode(vec3(0, 0, 0));
	animNodeSub->attachChild(animNode3);
	animNodeSub->attachChild(animNode4);
	scene->animationRoot->attachChild(animNodeSub);

	animNode->translateNode(0, 10, 0);
	animNodeSub->translateNode(10, 0, 0);
	
	node1->translateNode(0, 0, 20);

	scene->terrainNode->standObjectsOnGround(scene->staticRoot);
	scene->terrainNode->standObjectsOnGround(scene->animationRoot);
	
	Application::initScene();
}


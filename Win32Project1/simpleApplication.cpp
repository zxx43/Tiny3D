#include "simpleApplication.h"
#include "mesh/model.h"
#include "mesh/board.h"
#include "mesh/terrain.h"
#include "mesh/water.h"
#include "object/staticObject.h"
#include "constants/constants.h"
using namespace std;

SimpleApplication::SimpleApplication() : Application() {
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
	bloomChain = NULL;
	aaInput.clear();
	noiseBuf = NULL;
	firstFrame = true;
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
	if (bloomChain) delete bloomChain; bloomChain = NULL;
	aaInput.clear();
	if (noiseBuf) delete noiseBuf; noiseBuf = NULL;
}

void SimpleApplication::resize(int width, int height) {
	if (!render) return;

	const int precision = cfgs->graphQuality > 4 ? HIGH_PRE : LOW_PRE;
	const int scrPre = (cfgs->graphQuality > 4 || cfgs->ssr) ? HIGH_PRE : LOW_PRE;
	const int hdrPre = cfgs->graphQuality > 3 ? FLOAT_PRE : precision;
	const int matPre = LOW_PRE, waterPre = LOW_PRE;
	const int aaPre = LOW_PRE, dofPre = precision, rawPre = LOW_PRE;
	const float bloomScale = 0.75, ssrScale = 0.75, dofScale = 0.5;

	Application::resize(width, height);

	if (screen) delete screen;
	screen = new FrameBuffer(width, height, hdrPre, 4, false); // texBuffer
	screen->addColorBuffer(matPre, 4);                         // matBuffer
	screen->addColorBuffer(matPre, 3);                         // normal-grassBuffer
	screen->addColorBuffer(matPre, 2);                         // rough-metalBuffer
	screen->attachDepthBuffer(renderMgr->getDepthPre());       // depthBuffer

	if (waterFrame) delete waterFrame;
	waterFrame = new FrameBuffer(width, height, hdrPre, 4, false);
	waterFrame->addColorBuffer(waterPre, 4); // FragMat
	waterFrame->addColorBuffer(waterPre, 3); // FragNormal
	waterFrame->attachDepthBuffer(renderMgr->getDepthPre());

	if (sceneFilter) delete sceneFilter;
	sceneFilter = new Filter(width, height, true, precision, 4, false);
	sceneFilter->addOutput(hdrPre, 3); // FragBright

	if (combinedChain) delete combinedChain;
	if (!cfgs->fxaa && !cfgs->dof && !cfgs->ssr)
		combinedChain = new FilterChain(width, height, false, precision, 4);
	else {
		combinedChain = new FilterChain(width, height, true, precision, 4);
		if (cfgs->fxaa) {
			if (aaFilter) delete aaFilter;
			aaFilter = new Filter(width, height, false, aaPre, 3);
			aaInput.clear();
		}
		if (cfgs->dof) {
			if (dofBlurFilter) delete dofBlurFilter;
			dofBlurFilter = new Filter(width * dofScale, height * dofScale, true, dofPre, 4);
			if (dofChain) delete dofChain;
			dofChain = new FilterChain(width, height, cfgs->fxaa, dofPre, 4);
			dofChain->addInputTex(dofBlurFilter->getOutput(0));
			dofChain->addInputTex(combinedChain->getOutputTex(0));
		}
		if (cfgs->ssr) {
			if (ssrChain) delete ssrChain;
			ssrChain = new FilterChain(width * ssrScale, height * ssrScale, true, LOW_PRE, 4, false);
			ssrChain->addInputTex(combinedChain->getOutputTex(0)); // lightBuffer
			ssrChain->addInputTex(waterFrame->getColorBuffer(1));  // matBuffer
			ssrChain->addInputTex(waterFrame->getColorBuffer(2));  // normalBuffer
			ssrChain->addInputTex(waterFrame->getDepthBuffer());   // depthBuffer

			if (!cfgs->fxaa && !cfgs->dof) {
				if (rawScreenFilter) delete rawScreenFilter;
				rawScreenFilter = new Filter(width, height, false, rawPre, 4);
			}

			//if (ssrBlurFilter) delete ssrBlurFilter;
			//ssrBlurFilter = new Filter(width * 0.5, height * 0.5, true, LOW_PRE, 4);
		}
	}

	if (bloomChain) delete bloomChain;
	bloomChain = new DualFilter(width * bloomScale, height * bloomScale, true, hdrPre, 3, false);
	bloomChain->addInputTex(sceneFilter->getOutput(1));

	if (combinedChain) {
		combinedChain->output->addOutput(precision, 4);            // NormalWaterFlag
		if (ssgChain)
			combinedChain->addInputTex(ssgChain->getOutputTex(0)); // sceneBuffer
		else
			combinedChain->addInputTex(sceneFilter->getOutput(0)); // sceneBuffer
		combinedChain->addInputTex(screen->getColorBuffer(2));     // sceneNormalBuffer
		combinedChain->addInputTex(screen->getDepthBuffer());      // sceneDepthBuffer
		combinedChain->addInputTex(waterFrame->getColorBuffer(0)); // waterBuffer
		combinedChain->addInputTex(waterFrame->getColorBuffer(1)); // waterMatBuffer
		combinedChain->addInputTex(waterFrame->getColorBuffer(2)); // waterNormalBuffer
		combinedChain->addInputTex(waterFrame->getDepthBuffer());  // waterDepthBuffer
		combinedChain->addInputTex(bloomChain->getOutputTex());    // bloomBuffer
	}

	render->clearTextureSlots();
}

void SimpleApplication::keyDown(int key) {
	Application::keyDown(key);
	scene->player->keyDown(input, scene);
}

void SimpleApplication::keyUp(int key) {
	Application::keyUp(key);
	scene->player->keyUp(input);
}

void SimpleApplication::moveMouse(const float mx, const float my, const float cx, const float cy) {
	Application::moveMouse(mx, my, cx, cy);
	scene->player->mouseAct(scene, mx, my, cx, cy);
}

void SimpleApplication::mouseKey(bool press, bool isMain) {
	Application::mouseKey(press, isMain);
	scene->player->mousePress(press, isMain);
}

void SimpleApplication::preDraw() {
	if (!firstFrame) return;
	printf("first draw\n");
	firstFrame = false;
}

void SimpleApplication::draw() {
	if (!sceneFilter || !renderMgr || !AssetManager::assetManager) return;
	else preDraw();

	if (ssrChain) {
		//AssetManager::assetManager->setReflectTexture(ssrBlurFilter->getOutput(0));
		AssetManager::assetManager->setReflectTexture(ssrChain->getOutputTex(0));
	} else {
		if (renderMgr->reflectBuffer) {
			renderMgr->renderReflect(render, scene);
			AssetManager::assetManager->setReflectTexture(renderMgr->reflectBuffer->getColorBuffer(0));
		}
	}
	
	///*

	renderMgr->renderShadow(render, scene);
	renderMgr->renderSkyTex(render, scene);
	render->setFrameBuffer(screen);
	renderMgr->renderScene(render, scene);

	renderMgr->drawDeferred(render, scene, screen, sceneFilter);
	if (ssgChain)
		renderMgr->drawSSGFilter(render, scene, "ssg", ssgChain->input, ssgChain->output);

	render->setFrameBuffer(waterFrame);
	if (cfgs->graphQuality <= 3)
		waterFrame->getDepthBuffer()->copyDataFrom(screen->getDepthBuffer());
	if (renderMgr->isWaterShow(scene)) 
		renderMgr->renderWater(render, scene);

	if (cfgs->bloom)
		renderMgr->drawDualFilter(render, scene, "gaussv", "gaussh", bloomChain);

	renderMgr->drawCombined(render, scene, combinedChain->input, combinedChain->output);

	if (ssrChain) {
		if (rawScreenFilter)
			renderMgr->drawScreenFilter(render, scene, "screen", combinedChain->getOutFrameBuffer(), rawScreenFilter);
		renderMgr->drawSSRFilter(render, scene, "ssr", ssrChain->input, ssrChain->output);
		//renderMgr->drawScreenFilter(render, scene, "mean", ssrChain->getOutFrameBuffer(), ssrBlurFilter);
	}

	Filter* lastFilter = combinedChain->output;
	if (cfgs->dof) {
		renderMgr->drawScreenFilter(render, scene, "blur", combinedChain->getOutputTex(0), dofBlurFilter);
		renderMgr->drawScreenFilter(render, scene, "dof", dofChain->input, dofChain->output);
		lastFilter = dofChain->output;
	}
	if (cfgs->fxaa) {
		if (aaInput.size() == 0) {
			aaInput.push_back(lastFilter->getFrameBuffer()->getColorBuffer(0)); // colorBuffer
			aaInput.push_back(combinedChain->getOutputTex(1));					// normalWaterBuffer
		}
		renderMgr->drawScreenFilter(render, scene, "fxaa", aaInput, aaFilter);
	}
	//*/

	render->finishDraw();
}

void SimpleApplication::init() {
	Application::init();
	initScene();
	printf("Init ok!\n");
	render->getError();
}

void SimpleApplication::updateMovement() {
	if (scene->water)
		scene->water->moveWaterWithCamera(scene, scene->actCamera);
	if (scene->terrainNode) {
		vec3 cp = scene->actCamera->position;
		int bx, bz;
		scene->terrainNode->caculateBlock(cp.x, cp.z, bx, bz);
		int visualSize = cfgs->graphQuality >= 8 ? 60 : 40;
		scene->updateVisualTerrain(bx, bz, visualSize, visualSize);
		if (input->getControl() < 0) {
			if (scene->terrainNode->cauculateY(bx, bz, cp.x, cp.z, cp.y)) {
				if (scene->water) {
					float waterHeight = scene->water->position.y;
					cp.y = cp.y < waterHeight ? waterHeight : cp.y;
				}
				cp.y += scene->actCamera->getHeight();
			}
			else if (scene->water) {
				float waterHeight = scene->water->position.y;
				cp.y = waterHeight;
				cp.y += scene->actCamera->getHeight();
			}
			scene->actCamera->moveTo(cp);
		}
	}
}

void SimpleApplication::act(long startTime, long currentTime, float dTime, float velocity) {
	wheelAct();
	if (wheelDir != MNONE) {
		scene->player->wheelAct(wheelDir == MNEAR ? -1.0 : 1.0);
		wheelDir = MNONE;
	}
	keyAct(velocity);
	scene->player->controlAct(input, scene, velocity * 0.05);

	Application::act(startTime, currentTime, dTime, velocity);

	static float dd = 1.0, dr = 1.0;
	static bool needRotate = true;
	Node* node = scene->animationRoot->children[0];
	AnimationNode* animNode = (AnimationNode*)node->children[0];
	if (needRotate) {
		animNode->rotateNodeObject(scene, 0, 135 + 90 * dr, 0);
		needRotate = false;
	}
	animNode->translateNode(scene, animNode->position.x - 0.025 * dd, animNode->position.y, animNode->position.z - 0.025 * dd);
	static float distance = 0.0;
	distance++;
	if (distance > 5000.0) {
		dd *= -1.0;
		dr *= -1.0;
		distance = 0.0;
		needRotate = true;
	}

	node = scene->animationRoot->children[1];
	animNode = (AnimationNode*)node->children[3];
	animNode->rotateNodeObject(scene, animNode->getObject()->anglex, animNode->getObject()->angley, animNode->getObject()->anglez + 0.1);
	float radian = angleToRadian(animNode->getObject()->anglez);
	float rcos = cosf(radian);
	float rsin = sinf(radian);
	animNode->translateNode(scene, animNode->position.x + 0.04 * rsin, animNode->position.y, animNode->position.z + 0.04 * rcos);

	scene->updateNodes();

	scene->collisionWorld->act(dTime);
	scene->updateDynamicNodes();
	scene->updateAnimNodes();
	scene->player->updateCamera();
	updateMovement();

	animNode = (AnimationNode*)node->children[2];
	animNode->getObject()->playEffect("bark");

	scene->updateListenerPosition();
	if (!cfgs->ssr) scene->updateReflectCamera();
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
	assetMgr->addMesh("rock_low", new Model("models/sharprockfree_low.obj", "models/sharprockfree_low.mtl", 2));
	assetMgr->addMesh("terrain", new Terrain("terrain/Terrain.raw"));
	assetMgr->addMesh("water", new Water(1024, 16));

	assetMgr->meshes["treeA"]->setBoundScale(vec3(0.2, 1.0, 0.2));
	assetMgr->meshes["m1a2"]->setBoundScale(vec3(0.6, 1.0, 1.0));
	assetMgr->meshes["tank"]->setBoundScale(vec3(1.0, 1.0, 0.7));
	assetMgr->meshes["rock"]->setBoundScale(vec3(0.9, 1.0, 0.9));

	// Load animations
	assetMgr->addAnimation("ninja", new AssAnim("models/ninja.mesh"));
	assetMgr->addAnimation("army", new AssAnim("models/ArmyPilot.dae"));
	assetMgr->addAnimation("dog", new FBXLoader("models/Pes.fbx"));
	assetMgr->addAnimation("male", new FBXLoader("models/male.fbx"));
	assetMgr->initFrames();

	// Load textures
	assetMgr->addTextureBindless("cube.bmp", true);
	assetMgr->addTextureBindless("ground_n.bmp", false);
	assetMgr->addTextureBindless("ground.bmp", true);
	assetMgr->addTextureBindless("ground_norm.bmp", false);
	assetMgr->addTextureBindless("ground_g.bmp", true);
	assetMgr->addTextureBindless("ground_r.bmp", true);
	assetMgr->addTextureBindless("ground_s.bmp", true);
	assetMgr->addTextureBindless("ground_s2.bmp", true);
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
	assetMgr->addDistortionTex("distortion.bmp");
	assetMgr->addNoiseTex("noise.bmp");
	assetMgr->addRoadTex("road.bmp");
	assetMgr->createHeightTex();

	// Create materials
	Material* boxMat = new Material("box_mat");
	boxMat->tex1 = "cube.bmp";
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
	terrainMat->texids.z = assetMgr->findTextureBindless("ground_s2.bmp");
	terrainMat->texids.w = assetMgr->findTextureBindless("grass1-normal1-dx.bmp");
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
	box.setSound("push", "sounds/box.wav");
	StaticObject sphere(meshes["sphere"]); 
	sphere.bindMaterial(mtlMgr->find("iron_mat"));
	StaticObject board(meshes["board"]);
	StaticObject quad(meshes["quad"]);

	//StaticObject model1(meshes["billboard"], meshes["billboard"], meshes["billboard"]);
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
	model6.setSound("push", "sounds/box.wav");
	StaticObject model9(meshes["rock"], meshes["rock_low"], NULL);

	//return;
	scene->createSky(cfgs->dynsky);
	scene->createWater(vec3(-2048, 0, -2048), vec3(6, 1, 6));
	scene->createTerrain(vec3(-2048, -200, -2048), vec3(6, 2.0, 6));

	InstanceNode* node1 = new InstanceNode(vec3(2, 2, 2));
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

	InstanceNode* node2 = new InstanceNode(vec3(10, 2, 2));
	StaticObject* object6 = box.clone();
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
	house->setRotation(0, 180, 0);
	node2->addObject(scene, house);

	InstanceNode* node3 = new InstanceNode(vec3(25, 10, 0));
	StaticObject* objectSphere = sphere.clone();
	objectSphere->setSize(10, 10, 10);
	node3->addObject(scene, objectSphere);

	Node* node = new StaticNode(vec3(800, 0, -604));
	Node* modelNode = new StaticNode(vec3(0, 0, 0));
	modelNode->attachChild(scene, node1);
	modelNode->attachChild(scene, node2);
	node->attachChild(scene, modelNode);
	node->attachChild(scene, node3);
	scene->staticRoot->attachChild(scene, node);

	StaticObject* objectRock = model9.clone();
	objectRock->setPosition(-60, 0, 0);
	objectRock->setSize(0.1, 0.1, 0.1);
	node3->addObject(scene, objectRock);

	int treeScale = 13;
	int treeSpace = 180;
	int treePerc = 80;
	
	srand(100);
	InstanceNode* instanceNode1 = new InstanceNode(vec3(900, 0, 600));
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

	InstanceNode* stoneNode = new InstanceNode(vec3(0, 0, 0));
	stoneNode->detailLevel = 3;
	int stoneSize = 10, stoneSpace = 200;
	for (int i = -stoneSize; i < stoneSize; i++) {
		for (int j = -stoneSize; j < stoneSize; j++) {
			StaticObject* stone = model9.clone();
			float baseSize = 0.05;
			float size = (rand() % 100 * 0.01) * 0.2 + baseSize;
			if (size < 0.1) {
				stone->setDynamic(true);
				stone->setSound("push", "sounds/stone.wav");
			}

			stone->setSize(size, size, size);
			stone->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			stone->setPosition(j * stoneSpace + stoneSpace * (rand() % 100) * 0.01, 0, i * stoneSpace + stoneSpace * (rand() % 100) * 0.01);
			stoneNode->addObject(scene, stone);
		}
	}

	InstanceNode* instanceNode7 = new InstanceNode(vec3(903, 0, -608));
	StaticObject* oil1 = model6.clone();
	oil1->setPosition(30, 0, 30);
	oil1->setSize(10, 10, 10);
	oil1->setDynamic(true);
	StaticObject* oil2 = model6.clone();
	oil2->setPosition(30, 0, 40);
	oil2->setSize(10, 10, 10);
	oil2->setDynamic(true);
	StaticObject* oil3 = model6.clone();
	oil3->setPosition(40, 0, 30);
	oil3->setSize(10, 10, 10);
	oil3->setDynamic(true);
	StaticObject* oil4 = model6.clone();
	oil4->setPosition(40, 0, 40);
	oil4->setSize(10, 10, 10);
	oil4->setDynamic(true);
	StaticObject* box1 = box.clone();
	box1->setPosition(0, 0, 30);
	box1->setSize(6, 6, 6);
	box1->setDynamic(true);
	StaticObject* box2 = box.clone();
	box2->setPosition(0, 0, 40);
	box2->setRotation(0, 45, 0);
	box2->setSize(6, 6, 6);
	box2->setDynamic(true);
	StaticObject* box3 = box.clone();
	box3->setPosition(-10, 0, 30);
	box3->setSize(6, 6, 6);
	box3->setDynamic(true);
	StaticObject* box4 = box.clone();
	box4->setPosition(-10, 0, 40);
	box4->setSize(6, 6, 6);
	box4->setDynamic(true);
	instanceNode7->addObject(scene, oil1);
	instanceNode7->addObject(scene, oil2);
	instanceNode7->addObject(scene, oil3);
	instanceNode7->addObject(scene, oil4);
	instanceNode7->addObject(scene, box1);
	instanceNode7->addObject(scene, box2);
	instanceNode7->addObject(scene, box3);
	instanceNode7->addObject(scene, box4);

	scene->staticRoot->attachChild(scene, instanceNode1);
	scene->staticRoot->attachChild(scene, instanceNode2);
	scene->staticRoot->attachChild(scene, instanceNode3);
	scene->staticRoot->attachChild(scene, instanceNode4);
	scene->staticRoot->attachChild(scene, instanceNode5);
	scene->staticRoot->attachChild(scene, instanceNode6);
	scene->staticRoot->attachChild(scene, instanceNode7);
	scene->staticRoot->attachChild(scene, stoneNode);

	AnimationNode* animNode1 = new AnimationNode(vec3(5, 10, 5));
	animNode1->setAnimation(scene, animations["army"]);
	animNode1->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode1->getObject()->setPosition(0, -5, -1);
	animNode1->translateNode(scene, 5, 0, 15);
	animNode1->getObject()->setCurAnim(0, false);
	animNode1->getObject()->setLoop(true);
	AnimationNode* animNode2 = new AnimationNode(vec3(5, 10, 5));
	animNode2->setAnimation(scene, animations["ninja"]);
	animNode2->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode2->getObject()->setPosition(0, -5, -1);
	animNode2->translateNode(scene, 40, 0, 40);
	animNode2->rotateNodeObject(scene, 0, 45, 0);
	animNode2->getObject()->setDefaultAnim(10);
	AnimationNode* animNode3 = new AnimationNode(vec3(5, 10, 5));
	animNode3->setAnimation(scene, animations["ninja"]);
	animNode3->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode3->getObject()->setPosition(0, -5, -1);
	animNode3->translateNode(scene, 5, 0, 15);
	animNode3->getObject()->setDefaultAnim(11);
	AnimationNode* animNode4 = new AnimationNode(vec3(5, 10, 5));
	animNode4->setAnimation(scene, animations["ninja"]);
	animNode4->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode4->getObject()->setPosition(0, -5, -1);
	animNode4->translateNode(scene, 40, 0, 40);
	animNode4->getObject()->setDefaultAnim(12);
	AnimationNode* animNode5 = new AnimationNode(vec3(7.5, 7.5, 10.5));
	animNode5->setAnimation(scene, animations["dog"]);
	animNode5->scaleNodeObject(scene, 0.075, 0.075, 0.075);
	animNode5->getObject()->setPosition(0, -2.6, -1.5);
	animNode5->translateNode(scene, 30, 0, 20);
	animNode5->getObject()->setSound("bark", "sounds/dog.wav");
	AnimationNode* animNode6 = new AnimationNode(vec3(5.0, 10.0, 5.0));
	animNode6->setAnimation(scene, animations["male"]);
	animNode6->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode6->getObject()->setPosition(0.0, -4.0, 0.0);
	animNode6->translateNode(scene, 40, 0, 0);

	Node* animNode = new StaticNode(vec3(0.0));
	animNode->attachChild(scene, animNode1);
	animNode->attachChild(scene, animNode2);
	scene->animationRoot->attachChild(scene, animNode);
	Node* animNodeSub = new StaticNode(vec3(0.0));
	animNodeSub->attachChild(scene, animNode3);
	animNodeSub->attachChild(scene, animNode4);
	animNodeSub->attachChild(scene, animNode5);
	animNodeSub->attachChild(scene, animNode6);
	scene->animationRoot->attachChild(scene, animNodeSub);

	animNode->translateNode(scene, 0, 10, 0);
	animNodeSub->translateNode(scene, 10, 0, 0);
	
	node1->translateNode(scene, 0, 0, 20);

	scene->terrainNode->standObjectsOnGround(scene, scene->staticRoot);
	scene->updateNodes();
	scene->initAnimNodes();

	SoundObject* ambSound = new SoundObject("sounds/amb.wav");
	ambSound->setLoop(true);
	ambSound->setGain(50.0);
	ambSound->setPosition(vec3(0, 50, 0));
	scene->addSound(ambSound);
	scene->playSounds();
	
	Application::initScene();
}


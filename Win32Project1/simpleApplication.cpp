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
	edgeFilter = NULL;
	finFilter = NULL;
	dofBlurFilter = NULL;
	dofFilter = NULL;
	ssrChain = NULL;
	ssrBlurFilter = NULL;
	rawScreenFilter = NULL;
	ssgChain = NULL;
	bloomChain = NULL;
	edgeInput.clear();
	finInput.clear();
	dofInput.clear();
	noiseBuf = NULL;
	firstFrame = true;
	drawDepth = false;
	drawNormal = false;
	drawBounding = false;
	depthLevel = 0;
	timer = 0;
}

SimpleApplication::~SimpleApplication() {
	if (screen) delete screen; screen = NULL;
	if (waterFrame) delete waterFrame; waterFrame = NULL;
	if (sceneFilter) delete sceneFilter; sceneFilter = NULL;
	if (combinedChain) delete combinedChain; combinedChain = NULL;
	if (dofBlurFilter) delete dofBlurFilter; dofBlurFilter = NULL;
	if (dofFilter) delete dofFilter; dofFilter = NULL;
	if (edgeFilter) delete edgeFilter; edgeFilter = NULL;
	if (finFilter) delete finFilter; finFilter = NULL;
	if (ssrChain) delete ssrChain; ssrChain = NULL;
	if (ssrBlurFilter) delete ssrBlurFilter; ssrBlurFilter = NULL;
	if (rawScreenFilter) delete rawScreenFilter; rawScreenFilter = NULL;
	if (ssgChain) delete ssgChain; ssgChain = NULL;
	if (bloomChain) delete bloomChain; bloomChain = NULL;
	edgeInput.clear();
	finInput.clear();
	dofInput.clear();
	if (noiseBuf) delete noiseBuf; noiseBuf = NULL;
}

void SimpleApplication::resize(int width, int height) {
	if (!render) return;

	const int precision = cfgs->graphQuality > 4 ? HIGH_PRE : LOW_PRE;
	const int scrPre = (cfgs->graphQuality > 4 || cfgs->ssr) ? HIGH_PRE : LOW_PRE;
	//const int hdrPre = cfgs->graphQuality > 7 ? FLOAT_PRE : precision;
	const int hdrPre = precision;
	const int matPre = LOW_PRE, waterPre = LOW_PRE;
	const int finPre = LOW_PRE, edgePre = precision, dofPre = precision, rawPre = LOW_PRE;
	const float bloomScale = 0.75, ssrScale = 0.75, dofScale = 0.5;

	Application::resize(width, height);

	if (screen) delete screen;
	screen = new FrameBuffer(width, height, hdrPre, 4, WRAP_REPEAT, NEAREST); // texBuffer
	screen->addColorBuffer(matPre, 4);									// matBuffer
	screen->addColorBuffer(matPre, 4);									// roughMetalNormalBuffer
	screen->attachDepthBuffer(renderMgr->getDepthPre(), false);			// depthBuffer

	if (waterFrame) delete waterFrame;
	waterFrame = new FrameBuffer(width, height, hdrPre, 4, WRAP_REPEAT);
	waterFrame->addColorBuffer(waterPre, 4); // FragNormal
	waterFrame->attachDepthBuffer(renderMgr->getDepthPre(), false);

	if (sceneFilter) delete sceneFilter;
	sceneFilter = new Filter(width, height, true, precision, 4, LINEAR, WRAP_REPEAT);
	sceneFilter->addOutput(matPre, 3, LINEAR); // FragNormal
	sceneFilter->addOutput(hdrPre, 3, LINEAR); // FragBright

	renderMgr->prepareTransparent(screen, true, hdrPre);

	if (combinedChain) delete combinedChain;
	{
		combinedChain = new FilterChain(width, height, true, precision, 4);
		if (cfgs->cartoon) {
			if (edgeFilter) delete edgeFilter;
			edgeFilter = new Filter(width, height, true, edgePre, 4, NEAREST, WRAP_REPEAT);
			edgeInput.clear();
		}

		if (finFilter) delete finFilter;
		finFilter = new Filter(width, height, false, finPre, 3, NEAREST, WRAP_REPEAT);
		finInput.clear();

		if (cfgs->dof) {
			if (dofBlurFilter) delete dofBlurFilter;
			dofBlurFilter = new Filter(width * dofScale, height * dofScale, true, dofPre, 3, NEAREST, WRAP_REPEAT);
			if (dofFilter) delete dofFilter;
			dofFilter = new Filter(width, height, true, dofPre, 3, LINEAR, WRAP_CLAMP_TO_BORDER);
			dofInput.clear();
		}
		if (cfgs->ssr) {
			if (ssrChain) delete ssrChain;
			ssrChain = new FilterChain(width * ssrScale, height * ssrScale, true, LOW_PRE, 4, WRAP_REPEAT);
			ssrChain->addInputTex(combinedChain->getOutputTex(0)); // lightBuffer
			ssrChain->addInputTex(waterFrame->getColorBuffer(1));  // normalBuffer
			ssrChain->addInputTex(waterFrame->getDepthBuffer());   // depthBuffer

			if (ssrBlurFilter) delete ssrBlurFilter;
			ssrBlurFilter = new Filter(width * 0.5, height * 0.5, true, LOW_PRE, 4, LINEAR, WRAP_REPEAT);
		}
	}

	if (bloomChain) delete bloomChain;
	bloomChain = new DualFilter(width * bloomScale, height * bloomScale, true, hdrPre, 3, WRAP_REPEAT);
	bloomChain->addInputTex(sceneFilter->getOutput(2)); // Bright

	if (combinedChain) {
		combinedChain->output->addOutput(precision, 4, NEAREST);            // NormalWaterFlag
		if (ssgChain)
			combinedChain->addInputTex(ssgChain->getOutputTex(0)); // sceneBuffer
		else
			combinedChain->addInputTex(sceneFilter->getOutput(0)); // sceneBuffer
		combinedChain->addInputTex(sceneFilter->getOutput(1));     // sceneNormalBuffer
		combinedChain->addInputTex(screen->getDepthBuffer());      // sceneDepthBuffer
		combinedChain->addInputTex(waterFrame->getColorBuffer(0)); // waterBuffer
		combinedChain->addInputTex(waterFrame->getColorBuffer(1)); // waterNormalBuffer
		combinedChain->addInputTex(waterFrame->getDepthBuffer());  // waterDepthBuffer
		combinedChain->addInputTex(bloomChain->getOutputTex());    // bloomBuffer
	}

	render->clearTextureSlots();
}

void SimpleApplication::keyDown(int key) {
	Application::keyDown(key);
	scene->player->keyDown(input, scene);
	
	if(key == 67) printf("pos: %f, %f\n", scene->actCamera->position.x, scene->actCamera->position.z);
	if (key == 187) depthLevel++;
	if (key == 189) depthLevel = depthLevel >= 1 ? depthLevel - 1 : 0;
	//printf("key: %d\n", key);
}

void SimpleApplication::keyUp(int key) {
	Application::keyUp(key);
	scene->player->keyUp(input);

	if (key == 66) drawDepth = !drawDepth;
	if (key == 78) drawNormal = !drawNormal;
	if (key == 77) render->setDebugTerrain(!render->getDebugTerrain());
	if (key == 79) render->setFog(!render->getFog());
	if (key == 73) {
		cfgs->dof = !cfgs->dof;
		resize(windowWidth, windowHeight);
	}
	if (key == 80) {
		drawBounding = !drawBounding;
		cfgs->debug = drawBounding ? true : false;
		render->setDebug(cfgs->debug);
	}
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
		AssetManager::assetManager->setReflectTexture(ssrBlurFilter->getOutput(0));
		//AssetManager::assetManager->setReflectTexture(ssrChain->getOutputTex(0));
	} else {
		if (renderMgr->reflectBuffer) {
			renderMgr->renderReflect(render, scene);
			AssetManager::assetManager->setReflectTexture(renderMgr->reflectBuffer->getColorBuffer(0));
		}
	}
	AssetManager::assetManager->setSceneTexture(sceneFilter->getOutput(0));
	AssetManager::assetManager->setSceneDepth(screen->getDepthBuffer());

	renderMgr->clearGatherDatas(scene);
	renderMgr->clearRenderDatas(scene);
	renderMgr->resetRenderDatas(scene);
	
	///*

	renderMgr->renderShadow(render, scene);
	renderMgr->renderSkyTex(render, scene);
	render->setFrameBuffer(screen);
	renderMgr->renderScene(render, scene);
	renderMgr->renderTransparent(render, scene);

	renderMgr->drawDeferred(render, scene, screen, sceneFilter);
	if (ssgChain)
		renderMgr->drawSSGFilter(render, scene, "ssg", ssgChain->input, ssgChain->output);

	render->setFrameBuffer(waterFrame);
	if (cfgs->graphQuality <= 3)
		waterFrame->getDepthBuffer()->copyDataFrom(screen->getDepthBuffer());
	if (renderMgr->isWaterShow(render, scene)) 
		renderMgr->renderWater(render, scene);

	if (cfgs->bloom)
		renderMgr->drawDualFilter(render, scene, "gaussv", "gaussh", bloomChain);

	renderMgr->drawCombined(render, scene, combinedChain->input, combinedChain->output);

	if (ssrChain) {
		if (rawScreenFilter)
			renderMgr->drawScreenFilter(render, scene, "screen", combinedChain->getOutFrameBuffer(), rawScreenFilter);
		renderMgr->drawSSRFilter(render, scene, "ssr", ssrChain->input, ssrChain->output);
		renderMgr->drawScreenFilter(render, scene, "mean", ssrChain->getOutFrameBuffer(), ssrBlurFilter);
	}

	Filter* lastFilter = combinedChain->output;

	if (cfgs->cartoon) {
		if (edgeInput.size() == 0) {
			edgeInput.push_back(lastFilter->getFrameBuffer()->getColorBuffer(0)); // colorBuffer
			edgeInput.push_back(combinedChain->getOutputTex(1));				  // normalWaterBuffer
			edgeInput.push_back(screen->getColorBuffer(1));						  // matBuffer
		}
		renderMgr->drawScreenFilter(render, scene, render->getFog() ? "edge" : "edge_nfg", edgeInput, edgeFilter);
		lastFilter = edgeFilter;
	}

	// blend oit
	renderMgr->blendTransparent(render, scene, lastFilter->getFrameBuffer()->getColorBuffer(0));
	lastFilter = renderMgr->oit->blendFilter;

	if (cfgs->dof) {
		renderMgr->drawScreenFilter(render, scene, "blur", lastFilter->getOutput(0), dofBlurFilter);
		if (dofInput.size() == 0) {
			dofInput.push_back(dofBlurFilter->getOutput(0));
			dofInput.push_back(lastFilter->getOutput(0));
		}
		renderMgr->drawScreenFilter(render, scene, "dof", dofInput, dofFilter);
		lastFilter = dofFilter;
	}
	
	if (finInput.size() == 0) finInput.push_back(lastFilter->getFrameBuffer()->getColorBuffer(0));
	renderMgr->drawScreenFilter(render, scene, "fin", finInput, finFilter);

	renderMgr->retrievePrev(scene);
	renderMgr->genHiz(render, scene, screen->getDepthBuffer());
	if (drawDepth) renderMgr->drawHiz2Screen(render, scene, depthLevel);
	if (drawNormal) renderMgr->drawTexture2Screen(render, scene, sceneFilter->getOutput(1)->hnd);
	//renderMgr->drawTexture2Screen(render, scene, renderMgr->ibl->getBrdf()->hnd);
	//renderMgr->drawTexture2Screen(render, scene, sceneFilter->getOutput(1)->hnd);
	//renderMgr->drawTexture2Screen(render, scene, screen->getColorBuffer(2)->hnd);
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
		vec4 quat = Euler2Quat(vec3(0, 180, 0));
		quat = MulQuat(quat, animNode->getObject()->rotateQuat);
		animNode->rotateNodeObject(scene, quat);
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

	static AnimationNode* man = (AnimationNode*)node->children[2];
	if (man) {
		vec4 quat = Euler2Quat(vec3(0, 0.1, 0));
		quat = MulQuat(quat, man->getObject()->rotateQuat);
		man->rotateNodeObject(scene, quat);

		vec3 dir(-man->getObject()->rotateMat[4], -man->getObject()->rotateMat[5], -man->getObject()->rotateMat[6]);
		man->translateNode(scene, man->position.x + dir.x * 0.04, man->position.y + dir.y * 0.04, man->position.z + dir.z * 0.04);
	}

	static Node* dynObjs = scene->staticRoot->children[7];
	/*
	if (dynObjs) {
		for (int i = 0; i < dynObjs->objects.size(); ++i) {
			if (dynObjs->objects[i]->isDynamic()) {
				vec4 quat = Euler2Quat(vec3(0.1, 0.1, 0.1));
				quat = MulQuat(quat, dynObjs->objects[i]->rotateQuat);
				dynObjs->rotateNodeObject(scene, i, quat);
				dynObjs->translateNodeObject(scene, i, dynObjs->objects[i]->position.x + 0.01, dynObjs->objects[i]->position.y + 10.0, dynObjs->objects[i]->position.z + 0.01);
			}
		}
		scene->terrainNode->standObjectsOnGround(scene, dynObjs);
	}
	//*/
	static AnimationNode* ninja = (AnimationNode*)node->children[1];
	static AnimationNode* barker = (AnimationNode*)node->children[3];
	static Node* target1 = scene->staticRoot->children[1];
	static Node* target2 = scene->staticRoot->children[2];
	static Node* target3 = scene->staticRoot->children[3];
	static Node* target4 = scene->staticRoot->children[4];
	static Node* target5 = scene->staticRoot->children[5];
	static Node* target6 = scene->staticRoot->children[6];
	/*
	if (timer > 5000) {
		if (barker) {
			barker->pushToRemove();
			barker = NULL;
			printf("dog killed\n");
		}
	}
	if (timer > 10000) {
		if (ninja) {
			ninja->pushToRemove();
			ninja = NULL;
			scene->player->setNode(NULL, NULL);
			printf("ninja killed\n");
		}
	}
	if (timer > 15000) {
		if (man) {
			man->pushToRemove();
			man = NULL;
			printf("man killed\n");
		}
	}
	static bool halfKilled = false;
	if (timer > 10000) {
		if (!halfKilled) {
			for (int i = 0; i < target1->objects.size() * 0.5; ++i)
				delete target1->removeObject(target1->objects[i]);
			for (int i = 0; i < target2->objects.size() * 0.5; ++i)
				delete target2->removeObject(target2->objects[i]);
			for (int i = 0; i < target3->objects.size() * 0.5; ++i)
				delete target3->removeObject(target3->objects[i]);
			for (int i = 0; i < target4->objects.size() * 0.5; ++i)
				delete target4->removeObject(target4->objects[i]);
			for (int i = 0; i < target5->objects.size() * 0.5; ++i)
				delete target5->removeObject(target5->objects[i]);
			for (int i = 0; i < target6->objects.size() * 0.5; ++i)
				delete target6->removeObject(target6->objects[i]);
			halfKilled = true;
			printf("half tree killed\n");
		}
	}
	if (timer > 14500) {
		if (target2) {
			target2->pushToRemove();
			target2 = NULL;
		}
	}
	if (timer > 15500) {
		if (target3) {
			target3->pushToRemove();
			target3 = NULL;
		}
	}
	if (timer > 16500) {
		if (target4) {
			target4->pushToRemove();
			target4 = NULL;
		}
	}
	if (timer > 17500) {
		if (target5) {
			target5->pushToRemove();
			target5 = NULL;
		}
	}
	if (timer > 18500) {
		if (target6) {
			target6->pushToRemove();
			target6 = NULL;
		}
	}
	if (timer > 20000) {
		if (target1) {
			target1->pushToRemove();
			target1 = NULL;
			printf("all tree killed\n");
		}
	}
	//*/
	/*
	static bool created1 = false;
	if (timer > 5000) {
		if (!created1) {
			StaticObject* sphere = new StaticObject(AssetManager::assetManager->meshes["sphere"]);
			sphere->bindMaterial(MaterialManager::materials->find("iron_mat"));
			sphere->setSize(5, 5, 5);
			sphere->setPosition(0, 0, 0);
			sphere->setDynamic(true);
			dynObjs->addObject(scene, sphere);

			StaticObject* house = new StaticObject(AssetManager::assetManager->meshes["house"]);
			house->setSize(2, 2, 2);
			house->setPosition(5, 0, 5);
			dynObjs->addObject(scene, house);

			printf("added1!!\n");
			created1 = true;

			scene->terrainNode->standObjectsOnGround(scene, dynObjs);
		}
	}

	static bool created2 = false;
	if (timer > 5500) {
		if (!created2) {
			StaticObject* sphere = new StaticObject(AssetManager::assetManager->meshes["sphere"]);
			sphere->bindMaterial(MaterialManager::materials->find("gold_mat"));
			sphere->setSize(5, 5, 5);
			sphere->setPosition(4, 0, 1);
			sphere->setDynamic(true);
			dynObjs->addObject(scene, sphere);

			printf("added2!!\n");
			created2 = true;

			scene->terrainNode->standObjectsOnGround(scene, dynObjs);
		}
	}

	static bool created3 = false;
	if (timer > 6000) {
		if (!created3) {
			StaticObject* sphere = new StaticObject(AssetManager::assetManager->meshes["sphere"]);
			sphere->bindMaterial(MaterialManager::materials->find("streaky_mat"));
			sphere->setSize(5, 5, 5);
			sphere->setPosition(3, 0, 5);
			sphere->setDynamic(true);
			dynObjs->addObject(scene, sphere);

			printf("added3!!\n");
			created3 = true;

			scene->terrainNode->standObjectsOnGround(scene, dynObjs);
		}
	}

	//static bool deleted = false;
	//if (timer > 12000) {
	//	if (!deleted) {
	//		while (dynObjs->objects.size() > 0) {
	//			delete dynObjs->removeObject(dynObjs->objects[dynObjs->objects.size() - 1]);
	//		}
	//		deleted = true;
	//	}
	//}

	//static bool created4 = false;
	//if (timer > 16000) {
	//	if (!created4) {
	//		StaticObject* house = new StaticObject(AssetManager::assetManager->meshes["house"]);
	//		house->setSize(4, 4, 4);
	//		house->setPosition(5, 0, 5);
	//		house->setDynamic(true);
	//		dynObjs->addObject(scene, house);

	//		created4 = true;

	//		scene->terrainNode->standObjectsOnGround(scene, dynObjs);
	//	}
	//}
	//*/
	/*
	static bool animAdded1 = false;
	if (timer > 3500) {
		if (!animAdded1) {
			AnimationNode* ninja = new AnimationNode(vec3(5, 10, 5));
			ninja->setAnimation(scene, AssetManager::assetManager->animations["ninja"]);
			ninja->scaleNodeObject(scene, 0.05, 0.05, 0.05);
			ninja->getObject()->setPosition(0, -5, -1);
			ninja->translateNode(scene, 20, 0, 20);
			ninja->rotateNodeObject(scene, 0, 15, 0);
			ninja->getObject()->setDefaultAnim("idle2");
			ninja->getObject()->bindMaterial(MaterialManager::materials->find("cloth_red"));
			scene->animationRoot->attachChild(scene, ninja);

			animAdded1 = true;
			printf("anim added1\n");
		}
	}
	static bool animAdded2 = false;
	if (timer > 5500) {
		if (!animAdded2) {
			AnimationNode* ninja = new AnimationNode(vec3(5, 10, 5));
			ninja->setAnimation(scene, AssetManager::assetManager->animations["ninja"]);
			ninja->scaleNodeObject(scene, 0.05, 0.05, 0.05);
			ninja->getObject()->setPosition(0, -5, -1);
			ninja->translateNode(scene, 50, 0, 50);
			ninja->rotateNodeObject(scene, 0, 135, 0);
			ninja->getObject()->setDefaultAnim("idle3");
			scene->animationRoot->attachChild(scene, ninja);

			animAdded2 = true;
			printf("anim added2\n");
		}
	}
	//*/
	scene->updateNodes();

	scene->collisionWorld->act(dTime);
	scene->updateDynamicNodes();
	scene->updateAnimNodes();
	scene->player->updateCamera();
	updateMovement();

	renderMgr->updateDebugData(scene);

	//if (ninja) ninja->getObject()->playEffect("wang");
	if (barker) barker->getObject()->playEffect("bark");

	scene->updateListenerPosition();
	if (!cfgs->ssr) scene->updateReflectCamera();

	scene->flushNodes();
	if (!cfgs->dualthread) timer += 2;
	if (cfgs->debug) timer += 5;
	else timer++;
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
	assetMgr->addMesh("birch", new Model("models/birchB.obj", "models/birchB.mtl", 2));
	assetMgr->addMesh("bigtree", new Model("models/bigtreeC.obj", "models/bigtreeC.mtl", 3));
	assetMgr->addMesh("tank", new Model("models/tank.obj", "models/tank.mtl", 3));
	assetMgr->addMesh("m1a2", new Model("models/m1a2.obj", "models/m1a2.mtl", 2));
	assetMgr->addMesh("house", new Model("models/house.obj", "models/house.mtl", 2));
	assetMgr->addMesh("oildrum", new Model("models/oildrum.obj", "models/oildrum.mtl", 3));
	assetMgr->addMesh("rock", new Model("models/sharprockfree.obj", "models/sharprockfree.mtl", 2));
	assetMgr->addMesh("rock_low", new Model("models/sharprockfree_low.obj", "models/sharprockfree_low.mtl", 2));
	assetMgr->addMesh("cottage", new Model("models/cottage_obj.obj", "models/cottage_obj.mtl", 2));
	assetMgr->addMesh("terrain", new Terrain("terrain/Terrain.raw"));
	assetMgr->addMesh("water", new Water(WATER_SIZE, 16));

	assetMgr->meshes["treeA"]->setBoundScale(vec3(0.2, 1.0, 0.2));
	assetMgr->meshes["birch"]->setBoundScale(vec3(0.3, 1.0, 0.3));
	assetMgr->meshes["bigtree"]->setBoundScale(vec3(0.3, 1.0, 0.3));
	assetMgr->meshes["m1a2"]->setBoundScale(vec3(0.6, 1.0, 1.0));
	assetMgr->meshes["tank"]->setBoundScale(vec3(1.0, 1.0, 0.7));
	assetMgr->meshes["rock"]->setBoundScale(vec3(0.9, 1.0, 0.9));
	assetMgr->meshes["cottage"]->setBoundScale(vec3(0.9, 1.0, 0.8));

	// Load animation mesh & export animation data
	Animation* player = assetMgr->exportAnimation("ninja", new AssAnim("models/ninja.mesh"));
	Animation* army = assetMgr->exportAnimation("army", new AssAnim("models/ArmyPilot.dae"));
	Animation* dog = assetMgr->exportAnimation("dog", new FBXLoader("models/Pes.fbx"));
	Animation* male = assetMgr->exportAnimation("male", new FBXLoader("models/male.fbx"));

	// Load animation data & bind to animation mesh
	assetMgr->addAnimationData("army_run", "animation/army_combinedAnim_0.t3a", army);
	assetMgr->addAnimationData("male_run", "animation/male_Gun_Walk.t3a", male);
	assetMgr->addAnimationData("dog_idle", "animation/dog_idle2_CINEMA_4D_Main.t3a", dog);
	assetMgr->addAnimationData("idle1", "animation/ninja_Idle1.t3a", player);
	assetMgr->addAnimationData("idle2", "animation/ninja_Idle2.t3a", player);
	assetMgr->addAnimationData("idle3", "animation/ninja_Idle3.t3a", player);
	assetMgr->addAnimationData("attack1", "animation/ninja_Attack1.t3a", player);
	assetMgr->addAnimationData("attack2", "animation/ninja_Attack2.t3a", player);
	assetMgr->addAnimationData("attack3", "animation/ninja_Attack3.t3a", player);
	assetMgr->addAnimationData("block", "animation/ninja_Block.t3a", player);
	assetMgr->addAnimationData("kick", "animation/ninja_Kick.t3a", player);
	assetMgr->addAnimationData("walk", "animation/ninja_Walk.t3a", player);
	assetMgr->addAnimationData("jump", "animation/ninja_Backflip.t3a", player);

	// Create animation textures for vtf
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
	assetMgr->addTextureBindless("rustediron2_basecolor.png", true);
	assetMgr->addTextureBindless("rustediron2_normal.png", false);
	assetMgr->addTextureBindless("rustediron2_roughness.png", false);
	assetMgr->addTextureBindless("rustediron2_metallic.png", false);
	assetMgr->addTextureBindless("rustediron2_metallic_roughness.png", false);
	assetMgr->addTextureBindless("streaky-metal1_albedo.png", true);
	assetMgr->addTextureBindless("streaky-metal1_normal-ogl.png", false);
	assetMgr->addTextureBindless("streaky-metal1_roughness.png", false);
	assetMgr->addTextureBindless("streaky-metal1_metallic.png", false);
	assetMgr->addTextureBindless("streaky-metal1_metallic_roughness.png", false);
	assetMgr->addTextureBindless("lightgold_albedo.png", true);
	assetMgr->addTextureBindless("lightgold_normal-ogl.png", false);
	assetMgr->addTextureBindless("lightgold_roughness.png", false);
	assetMgr->addTextureBindless("lightgold_metallic.png", false);
	assetMgr->addTextureBindless("lightgold_metallic_roughness.png", false);
	assetMgr->addTextureBindless("grass1-albedo3.bmp", true);
	assetMgr->addTextureBindless("grass1-normal1-dx.bmp", false);
	assetMgr->addTextureBindless("ninja_red.bmp", true);
	assetMgr->addTextureBindless("ninja_blue.bmp", true);
	assetMgr->addDistortionTex("distortion.bmp");
	assetMgr->addNoiseTex("noise.bmp");
	assetMgr->addRoadTex("road.bmp");
	assetMgr->createHeightTex();

	// Create materials
	Material* blackMat = new Material(BLACK_MAT);
	blackMat->tex1 = "black.bmp";
	mtlMgr->add(blackMat);

	Material* whiteMat = new Material(WHITE_MAT);
	whiteMat->tex1 = "white.bmp";
	mtlMgr->add(whiteMat);

	Material* greenMat = new Material(GREEN_MAT);
	greenMat->tex1 = "green.bmp";
	mtlMgr->add(greenMat);

	Material* blueMat = new Material(BLUE_MAT);
	blueMat->tex1 = "blue.bmp";
	mtlMgr->add(blueMat);

	Material* redMat = new Material(RED_MAT);
	redMat->tex1 = "red.bmp";
	mtlMgr->add(redMat);

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
	ironMat->tex1 = "rustediron2_basecolor.png";
	ironMat->tex2 = "rustediron2_normal.png";
	ironMat->tex3 = "rustediron2_metallic_roughness.png";
	ironMat->tex4 = "rustediron2_metallic.png";
	mtlMgr->add(ironMat);

	Material* streakyMat = new Material("streaky_mat");
	streakyMat->tex1 = "streaky-metal1_albedo.png";
	streakyMat->tex2 = "streaky-metal1_normal-ogl.png";
	streakyMat->tex3 = "streaky-metal1_metallic_roughness.png";
	streakyMat->tex4 = "streaky-metal1_metallic.png";
	mtlMgr->add(streakyMat);

	Material* goldMat = new Material("gold_mat");
	goldMat->tex1 = "lightgold_albedo.png";
	goldMat->tex2 = "lightgold_normal-ogl.png";
	goldMat->tex3 = "lightgold_metallic_roughness.png";
	goldMat->tex4 = "lightgold_metallic.png";
	mtlMgr->add(goldMat);

	Material* clothRedMat = new Material("cloth_red");
	clothRedMat->tex1 = "ninja_red.bmp";
	mtlMgr->add(clothRedMat);

	Material* clothBlueMat = new Material("cloth_blue");
	clothBlueMat->tex1 = "ninja_blue.bmp";
	mtlMgr->add(clothBlueMat);
	
	Material* terrainMat = new Material("terrain_mat");
	terrainMat->prepared = true;
	terrainMat->texids.x = assetMgr->findTextureBindless("grass1-albedo3.bmp");
	terrainMat->texids.y = assetMgr->findTextureBindless("ground_r.bmp");
	terrainMat->texids.z = assetMgr->findTextureBindless("ground_s2.bmp");
	if (!cfgs->cartoon) terrainMat->texids.w = assetMgr->findTextureBindless("grass1-normal1-dx.bmp");
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

	StaticObject boxTransp(meshes["box_trans"]);
	boxTransp.bindMaterial(mtlMgr->find("box_mat"));
	boxTransp.setSound("push", "sounds/box.wav");

	StaticObject sphere(meshes["sphere"]);
	StaticObject board(meshes["board"]);
	StaticObject quad(meshes["quad"]);

	//StaticObject model1(meshes["tree"], meshes["billboard"], meshes["billboard"]);
	StaticObject model1(meshes["tree"], meshes["treeMid"], meshes["billboard"]);
	model1.detailLevel = 4;
	model1.setBillboard(5, 10, mtlMgr->find("billboard_tree_mat"));
	StaticObject model2(meshes["tank"]);
	StaticObject model3(meshes["m1a2"]);
	//StaticObject model4(meshes["treeA"], meshes["billboard"], meshes["billboard"]);
	StaticObject model4(meshes["treeA"], meshes["treeAMid"], meshes["billboard"]);
	model4.detailLevel = 4;
	model4.setBillboard(13, 14, mtlMgr->find("billboard_treeA_mat"));
	StaticObject model5(meshes["house"]);
	StaticObject model6(meshes["oildrum"]);
	model6.setSound("push", "sounds/box.wav");
	StaticObject model9(meshes["rock"], meshes["rock_low"], meshes["rock_low"]);
	StaticObject model10(meshes["cottage"]);
	StaticObject model11(meshes["birch"], meshes["birch"], meshes["billboard"]);
	model11.detailLevel = 4;
	model11.setBillboard(13, 14, mtlMgr->find("billboard_treeA_mat"));
	StaticObject model12(meshes["bigtree"], meshes["bigtree"], meshes["billboard"]);
	model12.detailLevel = 4;
	model12.setBillboard(13, 14, mtlMgr->find("billboard_treeA_mat"));

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
	StaticObject* object7 = boxTransp.clone();
	object7->setPosition(-5, 5, 6);
	object7->setRotation(30, 0, 30);
	object7->setSize(5, 5, 5);
	node2->addObject(scene, object7);
	StaticObject* house = model5.clone();
	house->setPosition(60, 0, 80);
	house->setSize(5, 5, 5);
	house->setRotation(0, 180, 0);
	node2->addObject(scene, house);

	InstanceNode* node3 = new InstanceNode(vec3(25, 10, 0));
	StaticObject* objIronSphere = sphere.clone();
	objIronSphere->bindMaterial(mtlMgr->find("iron_mat"));
	objIronSphere->setSize(10, 10, 10);
	objIronSphere->setPosition(0, 0, 0);
	node3->addObject(scene, objIronSphere);
	StaticObject* objGoldSphere = sphere.clone();
	objGoldSphere->bindMaterial(mtlMgr->find("gold_mat"));
	objGoldSphere->setSize(10, 10, 10);
	objGoldSphere->setPosition(20, 0, 0);
	node3->addObject(scene, objGoldSphere);
	StaticObject* objStreakySphere = sphere.clone();
	objStreakySphere->bindMaterial(mtlMgr->find("streaky_mat"));
	objStreakySphere->setSize(10, 10, 10);
	objStreakySphere->setPosition(40, 0, 0);
	node3->addObject(scene, objStreakySphere);

	InstanceNode* node4 = new InstanceNode(vec3(2553, 0, 1450));
	StaticObject* objectCottage = model10.clone();
	objectCottage->setPosition(-20, 0, -20);
	objectCottage->setSize(3, 3, 3);
	objectCottage->setRotation(0, 90, 0);
	node4->addObject(scene, objectCottage);
	StaticObject* boxObj1 = box.clone();
	boxObj1->setPosition(0, 0, 50);
	boxObj1->setSize(6, 6, 6);
	boxObj1->setDynamic(true);
	StaticObject* boxObj2 = box.clone();
	boxObj2->setPosition(0, 0, 60);
	boxObj2->setRotation(0, 30, 0);
	boxObj2->setSize(6, 6, 6);
	boxObj2->setDynamic(true);
	StaticObject* boxObj3 = box.clone();
	boxObj3->setPosition(-10, 0, 50);
	boxObj3->setSize(6, 6, 6);
	boxObj3->setDynamic(true);
	StaticObject* boxObj4 = box.clone();
	boxObj4->setPosition(-10, 0, 70);
	boxObj4->setSize(6, 6, 6);
	boxObj4->setDynamic(true);
	StaticObject* oilObj1 = model6.clone();
	oilObj1->setPosition(30, 0, 50);
	oilObj1->setSize(10, 10, 10);
	oilObj1->setDynamic(true);
	StaticObject* oilObj2 = model6.clone();
	oilObj2->setPosition(35, 0, 60);
	oilObj2->setSize(10, 10, 10);
	oilObj2->setDynamic(true);
	StaticObject* oilObj3 = model6.clone();
	oilObj3->setPosition(40, 0, 50);
	oilObj3->setSize(10, 10, 10);
	oilObj3->setDynamic(true);
	node4->addObject(scene, boxObj1);
	node4->addObject(scene, boxObj2);
	node4->addObject(scene, boxObj3);
	node4->addObject(scene, boxObj4);
	node4->addObject(scene, oilObj1);
	node4->addObject(scene, oilObj2);
	node4->addObject(scene, oilObj3);

	Node* node = new StaticNode(vec3(800, 0, -604));
	Node* modelNode = new StaticNode(vec3(0, 0, 0));
	modelNode->attachChild(scene, node1);
	modelNode->attachChild(scene, node2);
	node->attachChild(scene, modelNode);
	node->attachChild(scene, node3);
	node->attachChild(scene, node4);
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

			//tree->setDynamic(true);
			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode1->addObject(scene, tree);

			//instanceNode1->scaleNodeObject(scene, instanceNode1->objects.size() - 1, size, size, size);
			//instanceNode1->rotateNodeObject(scene, instanceNode1->objects.size() - 1, 0, 360 * (rand() % 100) * 0.01, 0);
			//instanceNode1->translateNodeObject(scene, instanceNode1->objects.size() - 1, j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
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
			StaticObject* tree = changeTree ? model11.clone() : model1.clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode2->addObject(scene, tree);

			//instanceNode2->translateNodeObject(scene, instanceNode2->objects.size() - 1, j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			//instanceNode2->rotateNodeObject(scene, instanceNode2->objects.size() - 1, 0, 360 * (rand() % 100) * 0.01, 0);
			//instanceNode2->scaleNodeObject(scene, instanceNode2->objects.size() - 1, size, size, size);
		}
	}
	InstanceNode* instanceNode3 = new InstanceNode(vec3(-700, 0, 1320));
	instanceNode3->detailLevel = 4;
	for (int i = -treeScale; i < treeScale; i++) {
		for (int j = -treeScale; j < treeScale; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > treePerc;
			StaticObject* tree = changeTree ? model12.clone() : model1.clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode3->addObject(scene, tree);

			//instanceNode3->translateNodeObject(scene, instanceNode3->objects.size() - 1, j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			//instanceNode3->rotateNodeObject(scene, instanceNode3->objects.size() - 1, 0, 360 * (rand() % 100) * 0.01, 0);
			//instanceNode3->scaleNodeObject(scene, instanceNode3->objects.size() - 1, size, size, size);
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

			//instanceNode4->translateNodeObject(scene, instanceNode4->objects.size() - 1, j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			//instanceNode4->rotateNodeObject(scene, instanceNode4->objects.size() - 1, 0, 360 * (rand() % 100) * 0.01, 0);
			//instanceNode4->scaleNodeObject(scene, instanceNode4->objects.size() - 1, size, size, size);
		}
	}
	InstanceNode* instanceNode5 = new InstanceNode(vec3(2100, 0, -600));
	instanceNode5->detailLevel = 4;
	for (int i = -treeScale; i < treeScale; i++) {
		for (int j = -treeScale; j < treeScale; j++) {
			//StaticObject* tree = model1->clone();
			//float baseSize = 2;
			bool changeTree = (rand() % 100) > treePerc;
			StaticObject* tree = changeTree ? model12.clone() : model1.clone();
			float baseSize = changeTree ? 3 : 5;
			float size = (rand() % 100 * 0.01) * 2 + baseSize;

			tree->setSize(size, size, size);
			tree->setRotation(0, 360 * (rand() % 100) * 0.01, 0);
			tree->setPosition(j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			instanceNode5->addObject(scene, tree);

			//instanceNode5->translateNodeObject(scene, instanceNode5->objects.size() - 1, j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			//instanceNode5->rotateNodeObject(scene, instanceNode5->objects.size() - 1, 0, 360 * (rand() % 100) * 0.01, 0);
			//instanceNode5->scaleNodeObject(scene, instanceNode5->objects.size() - 1, size, size, size);
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

			//instanceNode6->translateNodeObject(scene, instanceNode6->objects.size() - 1, j * treeSpace + treeSpace * (rand() % 100) * 0.01, 0, i * treeSpace + treeSpace * (rand() % 100) * 0.01);
			//instanceNode6->rotateNodeObject(scene, instanceNode6->objects.size() - 1, 0, 360 * (rand() % 100) * 0.01, 0);
			//instanceNode6->scaleNodeObject(scene, instanceNode6->objects.size() - 1, size, size, size);
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

			//stoneNode->scaleNodeObject(scene, stoneNode->objects.size() - 1, size, size, size);
			//stoneNode->rotateNodeObject(scene, stoneNode->objects.size() - 1, 0, 360 * (rand() % 100) * 0.01, 0);
			//stoneNode->translateNodeObject(scene, stoneNode->objects.size() - 1, j * stoneSpace + stoneSpace * (rand() % 100) * 0.01, 0, i* stoneSpace + stoneSpace * (rand() % 100) * 0.01);
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
	StaticObject* box1 = boxTransp.clone();
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
	animNode1->rotateNodeObject(scene, 0, 45, 0);
	animNode1->getObject()->setDefaultAnim("army_run");
	animNode1->getObject()->setLoop(true);
	AnimationNode* animNode2 = new AnimationNode(vec3(5, 10, 5));
	animNode2->setAnimation(scene, animations["ninja"]);
	animNode2->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode2->getObject()->setPosition(0, -5, -1);
	animNode2->translateNode(scene, 40, 0, 40);
	animNode2->rotateNodeObject(scene, 0, 45, 0);
	animNode2->getObject()->setDefaultAnim("idle1");
	animNode2->getObject()->bindMaterial(mtlMgr->find("cloth_red"));
	AnimationNode* animNode3 = new AnimationNode(vec3(5, 10, 5));
	animNode3->setAnimation(scene, animations["ninja"]);
	animNode3->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode3->getObject()->setPosition(0, -5, -1);
	animNode3->translateNode(scene, 5, 0, 15);
	animNode3->getObject()->setDefaultAnim("idle2");
	animNode3->getObject()->bindMaterial(mtlMgr->find("cloth_blue"));
	AnimationNode* animNode4 = new AnimationNode(vec3(5, 10, 5));
	animNode4->setAnimation(scene, animations["ninja"]);
	animNode4->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode4->getObject()->setPosition(0, -5, -1);
	animNode4->translateNode(scene, 40, 0, 40);
	animNode4->getObject()->setDefaultAnim("idle3");
	animNode4->getObject()->setSound("wang", "sounds/dog.wav");
	AnimationNode* animNode5 = new AnimationNode(vec3(5.0, 10.0, 5.0));
	animNode5->setAnimation(scene, animations["male"]);
	animNode5->scaleNodeObject(scene, 0.05, 0.05, 0.05);
	animNode5->getObject()->setPosition(0.0, -4.0, 0.0);
	animNode5->translateNode(scene, 40, 0, 0);
	animNode5->getObject()->setDefaultAnim("male_run");
	AnimationNode* animNode6 = new AnimationNode(vec3(7.5, 7.5, 10.5));
	animNode6->setAnimation(scene, animations["dog"]);
	animNode6->scaleNodeObject(scene, 0.075, 0.075, 0.075);
	animNode6->getObject()->setPosition(0, -2.6, -1.5);
	animNode6->translateNode(scene, 30, 0, 20);
	animNode6->getObject()->setDefaultAnim("dog_idle");
	animNode6->getObject()->setSound("bark", "sounds/dog.wav");

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


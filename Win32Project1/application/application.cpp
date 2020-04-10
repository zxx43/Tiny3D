#include "application.h"
#include "../constants/constants.h"
#include "../util/util.h"

Application::Application() {
	config = new Config("config/config.txt");
	cfgs = (ConfigArg*)malloc(sizeof(ConfigArg));
	memset(cfgs, 0, sizeof(ConfigArg));

	config->getInt("width", cfgs->width);
	config->getInt("height", cfgs->height);
	config->getBool("fullscreen", cfgs->fullscreen);
	config->getBool("dualthread", cfgs->dualthread);
	config->getBool("dualqueue", cfgs->dualqueue);
	config->getInt("smoothframe", cfgs->smoothframe);
	config->getInt("quality", cfgs->graphQuality);
	config->getInt("shadow", cfgs->shadowQuality);
	config->getBool("dof", cfgs->dof);
	config->getBool("fxaa", cfgs->fxaa);
	config->getBool("ssr", cfgs->ssr);
	config->getBool("bloom", cfgs->bloom);
	config->getBool("dynsky", cfgs->dynsky);
	config->getBool("cartoon", cfgs->cartoon);
	config->getBool("debug", cfgs->debug);

	windowWidth = cfgs->width;
	windowHeight = cfgs->height;
	fps = 0.0;

	willExit = false;
	pressed = false;
	showMouse();
	scene = NULL;
	render = NULL;
	input = NULL;
	renderMgr = NULL;
}

void Application::init() {
	printf("Init app\n");
	render = new Render();
	render->initShaders();
	AssetManager::Init();
	MaterialManager::Init();
	scene = new Scene();
	input = new Input();

	float lowDist = cfgs->graphQuality > 4 ? 800 : 200;
	float farDist = cfgs->graphQuality > 4 ? 1500 : 800;
	renderMgr = new RenderManager(cfgs, scene->mainCamera, lowDist, farDist, vec3(-1, -1, -1));

	if (!cfgs->ssr)
		scene->createReflectCamera();

	render->setDebug(cfgs->debug);
}

void Application::initScene() {
	scene->finishInit();
	printf("Scene inited!\n");
}

Application::~Application() {
	MaterialManager::Release();
	AssetManager::Release();
	delete scene; scene = NULL;
	delete render; render = NULL;
	delete input; input = NULL;
	delete renderMgr; renderMgr = NULL;
	delete config;
	free(cfgs);
}

void Application::act(long startTime, long currentTime, float velocity) {
	if (renderMgr) {
		input->updateExtra(renderMgr);
		scene->act(currentTime - startTime);
		scene->setVelocity(velocity);
	}
}

void Application::keyAct(float velocity) {
	scene->mainCamera->velocity = velocity;
	input->updateCameraByKey(scene->mainCamera);
}

void Application::wheelAct(int dir) {
	input->moveCamera(scene->mainCamera, dir);
}

void Application::moveMouse(const float mx, const float my, const float cx, const float cy) {
	input->updateCameraByMouse(scene->mainCamera, mx, my, cx, cy);
}

void Application::mouseKey(bool press, bool isMain) {
	pressed = press;
}

void Application::prepare(bool swapQueue) {
	scene->mainCamera->updateFrustum(); // Update main camera's frustum for cull
	renderMgr->updateMainLight(); // Update shadow cameras' frustum for cull
	renderMgr->swapRenderQueues(scene, swapQueue); // Caculate cull result
}

void Application::animate(float velocity) {
	renderMgr->animateQueues(velocity);
}

void Application::resize(int width, int height) {
	windowWidth = width; windowHeight = height;
	render->resize(width, height, scene->mainCamera, scene->reflectCamera);
	renderMgr->resize(width, height);
	renderMgr->updateShadowCamera(scene->mainCamera);
}

void Application::keyDown(int key) {
	input->keyDown(key);
	if (key == KEY_ESC)
		willExit = true;
}

void Application::keyUp(int key) {
	input->keyUp(key);
}

void Application::setFps(float fv) {
	fps = fv; 
}

#include "application.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include "../render/shaderscontainer.h"

Application::Application() {
	config = new Config("config/config.txt");

	float width = SCR_WIDTH, height = SCR_HEIGHT;
	config->get("width", width);
	config->get("height", height);
	windowWidth = width;
	windowHeight = height;
	fps = 0.0;

	willExit = false;
	scene = NULL;
	render = NULL;
	input = NULL;
	renderMgr = NULL;
}

void Application::init() {
	printf("Init app\n");
	render = new Render();
	SetupShaders(render);
	AssetManager::Init();
	MaterialManager::Init();
	scene = new Scene();
	input = new Input();

	config->get("quality", graphQuality);
	float lowDist = graphQuality > 4.0 ? 600 : 200;
	float farDist = graphQuality > 4.0 ? 1500 : 800;
	float dual = 0.0;
	config->get("dualthread", dual);
	dualThread = dual > 0.1 ? true : false;
	renderMgr = new RenderManager(graphQuality, scene->mainCamera, lowDist, farDist, dual, VECTOR3D(-1, -1, -1));
	if (graphQuality > 1)
		renderMgr->showShadow(true);
	else
		renderMgr->showShadow(false);

	config->get("dof", useDof);
	config->get("fxaa", useFxaa);
	config->get("ssr", useSsr);

	if (useSsr > 0.5) 
		renderMgr->enableSsr = true;
	else {
		scene->createReflectCamera();
		renderMgr->enableSsr = false;
	}


	float debug = 0.0;
	config->get("debug", debug);
	if (debug > 0.5)
		renderMgr->showBounding(true);
	else
		renderMgr->showBounding(false);
}

Application::~Application() {
	MaterialManager::Release();
	AssetManager::Release();
	delete scene; scene = NULL;
	delete render; render = NULL;
	delete input; input = NULL;
	delete renderMgr; renderMgr = NULL;
	delete config;
}

void Application::act(long startTime, long currentTime) {
	if (renderMgr) {
		input->updateExtra(renderMgr);
		renderMgr->act(currentTime - startTime);
	}
}

void Application::moveKey(float velocity) {
	scene->mainCamera->velocity = velocity;
	input->updateCameraByKey(scene->mainCamera);
}

void Application::moveByDir(int dir) {
	input->moveCamera(scene->mainCamera, dir);
}

void Application::moveMouse(const float mx, const float my, const float cx, const float cy) {
	input->updateCameraByMouse(scene->mainCamera, mx, my, cx, cy);
}

void Application::prepare(bool swapQueue) {
	scene->mainCamera->updateFrustum(); // Update main camera's frustum for cull
	renderMgr->updateMainLight(); // Update shadow cameras' frustum for cull
	renderMgr->swapRenderQueues(scene, swapQueue); // Caculate cull result
}

void Application::animate(long startTime, long currentTime) {
	renderMgr->animateQueues(startTime, currentTime);
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

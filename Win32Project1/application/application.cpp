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

	willExit = false;
	scene = NULL;
	render = NULL;
	input = NULL;
	renderMgr = NULL;
}

void Application::init() {
	render = new Render();
	SetupShaders(render);
	AssetManager::Init();
	MaterialManager::Init();
	scene = new Scene();
	input = new Input();

	float quality = 0; 
	config->get("quality", quality);
	renderMgr = new RenderManager(quality, scene->mainCamera, 200, 800, VECTOR3D(-1, -1, -1));
	if (quality > 0)
		renderMgr->enableShadow(render);
	else
		renderMgr->disableShadow(render);
	renderMgr->hideBounding();
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
		renderMgr->act();
	}
}

void Application::moveKey() {
	input->updateCameraByKey(scene->mainCamera);
}

void Application::moveMouse(const float mx, const float my, const float cx, const float cy) {
	input->updateCameraByMouse(scene->mainCamera, mx, my, cx, cy);
}

void Application::prepare() {
	scene->mainCamera->updateFrustum(); // Update main camera's frustum for cull
	renderMgr->updateMainLight(); // Update shadow cameras' frustum for cull
	renderMgr->swapRenderQueues(scene); // Caculate cull result
}

void Application::animate(long startTime, long currentTime) {
	renderMgr->animateQueues(startTime, currentTime);
}

void Application::resize(int width, int height) {
	windowWidth = width; windowHeight = height;
	render->resize(width, height, scene->mainCamera);
	renderMgr->updateShadowCamera();
}

void Application::keyDown(int key) {
	input->keyDown(key);
	if (key == KEY_ESC)
		willExit = true;
}

void Application::keyUp(int key) {
	input->keyUp(key);
}

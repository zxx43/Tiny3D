#include "application.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include "../render/shaderscontainer.h"

Application::Application() {
	windowWidth = SCR_WIDTH, windowHeight = SCR_HEIGHT;
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
	renderMgr = new RenderManager(scene->mainCamera, 100, 900, VECTOR3D(-1, -1, -1));
	renderMgr->enableShadow(render);
	renderMgr->hideBounding();
}

Application::~Application() {
	MaterialManager::Release();
	AssetManager::Release();
	delete scene; scene = NULL;
	delete render; render = NULL;
	delete input; input = NULL;
	delete renderMgr; renderMgr = NULL;
}

void Application::act(long startTime, long currentTime) {
	input->update(renderMgr);
}

void Application::moveCamera() {
	input->updateCamera(scene->mainCamera);
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

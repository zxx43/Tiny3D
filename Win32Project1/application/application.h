#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "../config/config.h"
#include "../input/input.h"
#include "../render/renderManager.h"
#include "../material/materialManager.h"
#include "../assets/assetManager.h"

class Application {
public:
	Config* config;
	Scene* scene;
	Render* render;
	Input* input;
	RenderManager* renderMgr;
public:
	int windowWidth, windowHeight;
	float fps;
	bool willExit;
	bool pressed;
	ConfigArg* cfgs;
public:
	Application();
	virtual ~Application();
public:
	virtual void init();
	virtual void initScene();
	virtual void draw() = 0;
	virtual void act(long startTime, long currentTime, float velocity);
	virtual void keyAct(float velocity);
	virtual void wheelAct(int dir);
	virtual void moveMouse(const float mx, const float my, const float cx, const float cy);
	virtual void mouseKey(bool press, bool isMain);
	void prepare(bool swapQueue);
	void animate(float velocity);
	virtual void resize(int width, int height);
	virtual void keyDown(int key);
	virtual void keyUp(int key);
	void setFps(float fv) { fps = fv; }
	float getFps() { return fps; }
};

#endif
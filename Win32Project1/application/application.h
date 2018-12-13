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
	bool willExit;
	float graphQuality, useDof, useFxaa, useSsr;
public:
	Application();
	virtual ~Application();
public:
	virtual void init();
	virtual void draw() = 0;
	virtual void act(long startTime, long currentTime);
	virtual void moveKey();
	virtual void moveByDir(int dir);
	void moveMouse(const float mx, const float my, const float cx, const float cy);
	void prepare();
	void animate(long startTime, long currentTime);
	virtual void resize(int width, int height);
	virtual void keyDown(int key);
	virtual void keyUp(int key);
};

#endif
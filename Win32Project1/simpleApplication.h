#ifndef SIMPLE_APPLICATION_H_
#define SIMPLE_APPLICATION_H_

#include "application/application.h"
#include "filter/filter.h"

class SimpleApplication : public Application {
private:
	FrameBuffer* screen;
	FrameBuffer* waterFrame;
	Filter* sceneFilter;
	FilterChain* ssgChain;
	FilterChain* combinedChain;
	std::vector<Texture*> edgeInput;
	Filter* edgeFilter;
	std::vector<Texture*> finInput;
	Filter* finFilter;
	Filter* dofBlurFilter;
	std::vector<Texture*> dofInput;
	Filter* dofFilter;
	FilterChain* ssrChain;
	Filter* ssrBlurFilter;
	Filter* rawScreenFilter;
	DualFilter* bloomChain;
	FrameBuffer* noiseBuf;
	bool firstFrame;
	bool drawDepth;
	bool drawNormal;
	bool drawBounding;
	int depthLevel;
	uint timer;
public:
	SimpleApplication();
	virtual ~SimpleApplication();
public:
	virtual void init();
	virtual void draw();
	virtual void act(long startTime, long currentTime, float dTime, float velocity);
	virtual void moveMouse(const float mx, const float my, const float cx, const float cy);
	virtual void mouseKey(bool press, bool isMain);
	virtual void resize(int width, int height);
	virtual void keyDown(int key);
	virtual void keyUp(int key);
private:
	virtual void initScene();
	void updateMovement();
	void preDraw();
};

#endif
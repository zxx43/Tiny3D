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
	std::vector<Texture2D*> aaInput;
	Filter* aaFilter;
	Filter* dofBlurFilter;
	FilterChain* dofChain;
	FilterChain* ssrChain;
	Filter* ssrBlurFilter;
	Filter* rawScreenFilter;
	DualFilter* bloomChain;
public:
	SimpleApplication();
	virtual ~SimpleApplication();
public:
	virtual void init();
	virtual void draw();
	virtual void act(long startTime, long currentTime);
	virtual void moveKey(float velocity);
	virtual void moveByDir(int dir);
	virtual void resize(int width, int height);
	virtual void keyDown(int key);
	virtual void keyUp(int key);
private:
	virtual void initScene();
	void updateMovement();
};

#endif
#ifndef SIMPLE_APPLICATION_H_
#define SIMPLE_APPLICATION_H_

#include "application/application.h"
#include "filter/filter.h"

class SimpleApplication : public Application {
private:
	FrameBuffer* screen;
	Filter* screenFilter;
	Filter* aaFilter;
	Filter* dofBlurFilter;
	Filter* dofFilter;
	Filter* ssrFilter;
	Filter* ssrBlurFilter;
	Filter* rawScreenFilter;
	std::vector<Texture2D*> dofInput;
	std::vector<Texture2D*> ssrInput;
public:
	SimpleApplication();
	virtual ~SimpleApplication();
public:
	virtual void init();
	virtual void draw();
	virtual void act(long startTime, long currentTime);
	virtual void moveKey();
	virtual void moveByDir(int dir);
	virtual void resize(int width, int height);
	virtual void keyDown(int key);
	virtual void keyUp(int key);
private:
	void initScene();
	void updateMovement();
};

#endif
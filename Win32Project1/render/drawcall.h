/*
 * drawcall.h
 *
 *  Created on: 2017-6-22
 *      Author: a
 */

#ifndef DRAWCALL_H_
#define DRAWCALL_H_

#define NULL_DC 0
#define STATIC_DC 1
#define INSTANCE_DC 2
#define ANIMATE_DC 3

#define NEAR_SHADOW_PASS 1
#define MID_SHADOW_PASS 2
#define FAR_SHADOW_PASS 3
#define COLOR_PASS 4
#define DEFERRED_PASS 5
#define POST_PASS 6

#include "../maths/Maths.h"
#include "renderState.h"
#include "renderBuffer.h"

class Drawcall {
private:
	bool singleSide;
	int type;
	bool fullStatic;
	bool billboardDC;
public:
	float* uModelMatrix;
	float* uNormalMatrix;
	int objectCount;
	RenderBuffer* dataBuffer;

	Drawcall();
	virtual ~Drawcall();

	virtual void draw(Shader* shader,int pass)=0;
	void setSide(bool single);
	bool isSingleSide();
	void setType(int typ);
	int getType();
	void setFullStatic(bool stat);
	bool isFullStatic();
	void setBillboard(bool billboard);
	bool isBillboard();
};

#endif /* DRAWCALL_H_ */

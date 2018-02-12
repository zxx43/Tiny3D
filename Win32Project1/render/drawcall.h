/*
 * drawcall.h
 *
 *  Created on: 2017-6-22
 *      Author: a
 */

#ifndef DRAWCALL_H_
#define DRAWCALL_H_

#define VERTEX_VBO 0
#define NORMAL_VBO 1
#define TEXCOORD_VBO 2
#define COLOR_VBO 3
#define OBJECTID_VBO 4
#define BONEID_VBO 4
#define WEIGHT_VBO 5
#define MODEL_MATRIX_VBO 4

#define VERTEX_LOCATION 0
#define NORMAL_LOCATION 1
#define TEXCOORD_LOCATION 2
#define COLOR_LOCATION 3
#define OBJECTID_LOCATION 4
#define BONEIDS_LOCATION 4
#define WEIGHTS_LOCATION 5
#define MODEL_MATRIX_LOCATION 4

#define NULL_DC 0
#define STATIC_DC 1
#define INSTANCE_DC 2
#define ANIMATE_DC 3

#include "../maths/Maths.h"
#include "renderState.h"
#include "renderBuffer.h"

class Drawcall {
private:
	bool singleSide;
	int type;
	bool fullStatic;
public:
	float* uModelMatrix;
	float* uNormalMatrix;
	int objectCount;
	RenderBuffer* dataBuffer;
	RenderBuffer* simpleBuffer;

	Drawcall();
	virtual ~Drawcall();

	virtual void createSimple() = 0;
	virtual void releaseSimple() = 0;

	virtual void draw(Shader* shader,bool simple)=0;
	void setSide(bool single);
	bool isSingleSide();
	void setType(int typ);
	int getType();
	void setFullStatic(bool stat);
	bool isFullStatic();
};

#endif /* DRAWCALL_H_ */

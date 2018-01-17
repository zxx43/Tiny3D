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
#define TEXTUREID_VBO 4
#define BONEID_VBO 5
#define WEIGHT_VBO 6
#define MODEL_MATRIX_VBO 5
#define NORMAL_MATRIX_VBO 6
#define INDEX_VBO 7

#define ATTRIB_VERTEX "vertex"
#define ATTRIB_NORMAL "normal"
#define ATTRIB_TEXCOORD "texcoord"
#define ATTRIB_AMBIENT "ambient"
#define ATTRIB_DIFFUSE "diffuse"
#define ATTRIB_SPECULAR "specular"
#define ATTRIB_TEXTURE "texid"
#define ATTRIB_BONEIDS "boneids"
#define ATTRIB_WEIGHTS "weights"
#define ATTRIB_MODEL_MATRIX "modelMatrix"
#define ATTRIB_NORMAL_MATRIX "normalMatrix"

#define VERTEX_LOCATION 0
#define NORMAL_LOCATION 1
#define TEXCOORD_LOCATION 2
#define COLOR_LOCATION 3
#define TEXTURE_LOCATION 4
#define BONEIDS_LOCATION 5
#define WEIGHTS_LOCATION 6
#define MODEL_MATRIX_LOCATION 5
#define NORMAL_MATRIX_LOCATION 8

#include "../shader/shader.h"
#include "../maths/Maths.h"
#include "../shadow/shadow.h"

struct RenderState {
	bool enableCull;
	int cullMode;
	bool drawLine;
	bool enableDepthTest;
	int depthTestMode;
	bool lightEffect; 
	bool shadowPass;
	Shadow* shadow;
	VECTOR3D light;
	Shader* shader;
	RenderState() {
		enableCull = true;
		cullMode = CULL_BACK;
		drawLine = false;
		enableDepthTest = true;
		depthTestMode = DEPTH_LEQUAL;
		lightEffect = true;
		shadowPass = false;
		shadow = NULL;
		light = VECTOR3D(0, 0, 0);
		shader = NULL;
	}
	void reset() {
		enableCull = true;
		cullMode = CULL_BACK;
		drawLine = false;
		enableDepthTest = true;
		depthTestMode = DEPTH_LEQUAL;
		lightEffect = true;
		shadowPass = false;
		shadow = NULL;
		light = VECTOR3D(0, 0, 0);
		shader = NULL;
	}
	void copyFrom(RenderState* src) {
		enableCull = src->enableCull;
		cullMode = src->cullMode;
		drawLine = src->drawLine;
		enableDepthTest = src->enableDepthTest;
		depthTestMode = src->depthTestMode;
		lightEffect = src->lightEffect;
		shadowPass = src->shadowPass;
		shadow = src->shadow;
		light = src->light;
		shader = src->shader;
	}
};

class Drawcall {
private:
	bool singleSide;
public:
	float* uModelMatrix;
	float* uNormalMatrix;
	GLuint* vbos;
	GLuint vao;
	GLuint* vboSimple;
	GLuint vaoSimple;

	Drawcall();
	virtual ~Drawcall();

	virtual void createSimple() = 0;
	virtual void releaseSimple() = 0;

	virtual void draw(Shader* shader,bool simple)=0;
	void setSide(bool single);
	bool isSingleSide();
};

#endif /* DRAWCALL_H_ */

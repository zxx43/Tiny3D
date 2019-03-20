#ifndef RENDER_STATE_H_
#define RENDER_STATE_H_

#include "../shader/shader.h"
#include "../shadow/shadow.h"

struct RenderState {
	bool enableCull;
	int cullMode;
	bool drawLine;
	bool enableDepthTest;
	int depthTestMode;
	bool enableAlphaTest;
	int alphaTestMode;
	float alphaThreshold;
	bool lightEffect;
	bool skyPass;
	bool waterPass;
	bool ssrPass;
	bool enableSsr;
	bool blend;
	bool simpleIns;
	bool grass;
	int pass;
	float time;
	float quality;
	Shadow* shadow;
	VECTOR3D light;
	VECTOR3D* eyePos;
	Shader* shader;
	Shader* shaderIns;
	Shader* shaderGrass;
	Shader* shaderBillboard;
	Shader* shaderSimple;

	RenderState() {
		reset();
		light = VECTOR3D(0, 0, 0);
		eyePos = NULL;
	}
	RenderState(const RenderState& rhs) {
		copy(&rhs);
	}
	void copy(const RenderState* rhs) {
		enableCull = rhs->enableCull;
		cullMode = rhs->cullMode;
		drawLine = rhs->drawLine;
		enableDepthTest = rhs->enableDepthTest;
		depthTestMode = rhs->depthTestMode;
		enableAlphaTest = rhs->enableAlphaTest;
		alphaTestMode = rhs->alphaTestMode;
		alphaThreshold = rhs->alphaThreshold;
		lightEffect = rhs->lightEffect;
		skyPass = rhs->skyPass;
		waterPass = rhs->waterPass;
		ssrPass = rhs->ssrPass;
		enableSsr = rhs->enableSsr;
		blend = rhs->blend;
		simpleIns = rhs->simpleIns;
		grass = rhs->grass;
		pass = rhs->pass;
		time = rhs->time;
		quality = rhs->quality;
		shadow = rhs->shadow;
		light = rhs->light;
		eyePos = rhs->eyePos;
		shader = rhs->shader;
		shaderIns = rhs->shaderIns;
		shaderGrass = rhs->shaderGrass;
		shaderBillboard = rhs->shaderBillboard;
		shaderSimple = rhs->shaderSimple;
	}
	void reset() {
		enableCull = true;
		cullMode = CULL_BACK;
		drawLine = false;
		enableDepthTest = true;
		depthTestMode = LEQUAL;
		enableAlphaTest = false;
		alphaTestMode = GREATER;
		alphaThreshold = 0;
		lightEffect = true;
		skyPass = false;
		waterPass = false;
		ssrPass = false;
		enableSsr = false;
		blend = false;
		simpleIns = false;
		grass = false;
		pass = COLOR_PASS;
		time = 0.0;
		quality = 1.0;
		shadow = NULL;
		shader = NULL;
		shaderIns = NULL;
		shaderGrass = NULL;
		shaderBillboard = NULL;
		shaderSimple = NULL;
	}
};

#endif
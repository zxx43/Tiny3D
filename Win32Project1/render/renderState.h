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
	bool grassPass;
	bool ssrPass;
	bool ssgPass;
	bool enableSsr;
	bool blend;
	bool tess;
	int pass;
	float time;
	float quality;
	Shadow* shadow;
	vec3 light;
	vec3* eyePos;
	Shader* shader;
	Shader* shaderIns;
	Shader* shaderBillboard;
	Shader* shaderCompute;

	RenderState() {
		reset();
		light = vec3(0, 0, 0);
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
		grassPass = rhs->grassPass;
		ssrPass = rhs->ssrPass;
		ssgPass = rhs->ssgPass;
		enableSsr = rhs->enableSsr;
		blend = rhs->blend;
		tess = rhs->tess;
		pass = rhs->pass;
		time = rhs->time;
		quality = rhs->quality;
		shadow = rhs->shadow;
		light = rhs->light;
		eyePos = rhs->eyePos;
		shader = rhs->shader;
		shaderIns = rhs->shaderIns;
		shaderBillboard = rhs->shaderBillboard;
		shaderCompute = rhs->shaderCompute;
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
		grassPass = false;
		ssrPass = false;
		ssgPass = false;
		enableSsr = false;
		blend = false;
		tess = false;
		pass = COLOR_PASS;
		time = 0.0;
		quality = 1.0;
		shadow = NULL;
		shader = NULL;
		shaderIns = NULL;
		shaderBillboard = NULL;
		shaderCompute = NULL;
	}
};

#endif
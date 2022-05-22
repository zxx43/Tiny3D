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
	bool atmoPass;
	bool cloudPass;
	bool iblPass;
	bool dynSky;
	bool waterPass;
	bool grassPass;
	bool ssrPass;
	bool ssgPass;
	bool enableSsr;
	bool blend;
	bool tess;
	int pass;
	bool dynPass;
	float time;
	float quality;
	int delay;
	Shadow* shadow;
	vec3 light;
	float udotl;
	vec3* eyePos;
	Shader* shader;
	Shader* shaderIns;
	Shader* shaderBill;
	Shader* shaderCompute;

	vec3 mapTrans, mapScl;
	vec4 mapInfo;

	RenderState() {
		reset();
		light = vec3(0, 0, 0);
		udotl = 0.0;
		eyePos = NULL;

		mapTrans = vec3(0, 0, 0);
		mapScl = vec3(0, 0, 0);
		mapInfo = vec4(0, 0, 0, 0);
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
		atmoPass = rhs->atmoPass;
		cloudPass = rhs->cloudPass;
		iblPass = rhs->iblPass;
		dynSky = rhs->dynSky;
		waterPass = rhs->waterPass;
		grassPass = rhs->grassPass;
		ssrPass = rhs->ssrPass;
		ssgPass = rhs->ssgPass;
		enableSsr = rhs->enableSsr;
		blend = rhs->blend;
		tess = rhs->tess;
		pass = rhs->pass;
		dynPass = rhs->pass;
		time = rhs->time;
		quality = rhs->quality;
		delay = rhs->delay;
		shadow = rhs->shadow;
		light = rhs->light;
		udotl = rhs->udotl;
		eyePos = rhs->eyePos;
		shader = rhs->shader;
		shaderIns = rhs->shaderIns;
		shaderBill = rhs->shaderBill;
		shaderCompute = rhs->shaderCompute;
		mapTrans = rhs->mapTrans;
		mapScl = rhs->mapScl;
		mapInfo = rhs->mapInfo;
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
		atmoPass = false;
		cloudPass = false;
		iblPass = false;
		dynSky = false;
		waterPass = false;
		grassPass = false;
		ssrPass = false;
		ssgPass = false;
		enableSsr = false;
		blend = false;
		tess = false;
		pass = COLOR_PASS;
		dynPass = false;
		time = 0.0;
		quality = 1.0;
		delay = DELAY_FRAME;
		shadow = NULL;
		shader = NULL;
		shaderIns = NULL;
		shaderBill = NULL;
		shaderCompute = NULL;
	}
	bool isShadowPass() const {
		return pass < COLOR_PASS;
	}
};

#endif
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
	int pass;
	float time;
	Shadow* shadow;
	VECTOR3D light;
	Shader* shader;
	Shader* shaderIns;
	Shader* shaderBillboard;

	RenderState() {
		reset();
		light = VECTOR3D(0, 0, 0);
	}
	RenderState(const RenderState& rhs) {
		enableCull = rhs.enableCull;
		cullMode = rhs.cullMode;
		drawLine = rhs.drawLine;
		enableDepthTest = rhs.enableDepthTest;
		depthTestMode = rhs.depthTestMode;
		enableAlphaTest = rhs.enableAlphaTest;
		alphaTestMode = rhs.alphaTestMode;
		alphaThreshold = rhs.alphaThreshold;
		lightEffect = rhs.lightEffect;
		skyPass = rhs.skyPass;
		waterPass = rhs.waterPass;
		ssrPass = rhs.ssrPass;
		enableSsr = rhs.enableSsr;
		blend = rhs.blend;
		pass = rhs.pass;
		time = rhs.time;
		shadow = rhs.shadow;
		light = rhs.light;
		shader = rhs.shader;
		shaderIns = rhs.shaderIns;
		shaderBillboard = rhs.shaderBillboard;
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
		pass = COLOR_PASS;
		time = 0.0;
		shadow = NULL;
		shader = NULL;
		shaderIns = NULL;
		shaderBillboard = NULL;
	}
};

#endif
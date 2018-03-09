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
	bool deferred;
	int pass;
	Shadow* shadow;
	VECTOR3D light;
	Shader* shader;
	Shader* shaderIns;

	RenderState() {
		reset();
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
		deferred = rhs.deferred;
		pass = rhs.pass;
		shadow = rhs.shadow;
		light = rhs.light;
		shader = rhs.shader;
		shaderIns = rhs.shaderIns;
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
		deferred = false;
		pass = 4;
		shadow = NULL;
		light = VECTOR3D(0, 0, 0);
		shader = NULL;
		shaderIns = NULL;
	}
};

#endif
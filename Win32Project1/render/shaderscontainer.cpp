#include "shaderscontainer.h"

#define SHADOW_TEX_FRAG "shader/shadow_tex.frag"
#define SHADOW_NONTEX_FRAG "shader/shadow_nontex.frag"
#define PHONG_VERT "shader/phong.vert"
#define PHONG_FRAG "shader/phong.frag"
#define GRASS_VERT "shader/grass.vert"
#define INSTANCE_VERT "shader/instance.vert"
#define INSTANCE_SIMPLE_VERT "shader/instance_simp.vert"
#define BONE_VERT "shader/bone.vert"
#define BONE_FRAG "shader/bone.frag"
#define SKY_VERT "shader/sky.vert"
#define SKY_FRAG "shader/sky.frag"
#define POST_VERT "shader/post.vert"
#define DEFERRED_FRAG "shader/deferred.frag"
#define TERRAIN_VERT "shader/terrain.vert"
#define TERRAIN_FRAG "shader/terrain.frag"
#define BILLBOARD_VERT "shader/billboard.vert"
#define BILLBOARD_FRAG "shader/billboard.frag"
#define WATER_VERT "shader/water.vert"
#define WATER_FRAG "shader/water.frag"
#define AA_FRAG "shader/fxaa.frag"
#define BLUR_FRAG "shader/blur.frag"
#define MEAN_FRAG "shader/mean.frag"
#define DOF_FRAG "shader/dof.frag"
#define DEBUG_VERT "shader/debug.vert"
#define DEBUG_FRAG "shader/debug.frag"
#define SCREEN_FRAG "shader/screen.frag"
#define SSR_FRAG "shader/ssr.frag"
#define COMBINE_FRAG "shader/combined.frag"
#define SSG_FRAG "shader/ssg.frag"
#define GRASS_LAYER_VERT "shader/grassLayer.vert"
#define GRASS_LAYER_FRAG "shader/grassLayer.frag"
#define GRASS_LAYER_GEOM "shader/grassLayer.geom"
#define GRASS_LAYER_TESC "shader/grassLayer.tesc"
#define GRASS_LAYER_TESE "shader/grassLayer.tese"

void SetupShaders(Render* render) {
	Shader* phong = render->shaders->addShader("phong", PHONG_VERT, PHONG_FRAG);
	render->shaders->addShaderBindTex(phong);
	phong->setFloat("shadowPass", 0.0);
	phong->setFloat("lowPass", 0.0);

	Shader* phongIns = render->shaders->addShader("phong_ins", INSTANCE_VERT, PHONG_FRAG);
	render->shaders->addShaderBindTex(phongIns);
	phongIns->setFloat("shadowPass", 0.0);
	phongIns->setFloat("lowPass", 0.0);

	Shader* grass = render->shaders->addShader("grass", GRASS_VERT, PHONG_FRAG);
	render->shaders->addShaderBindTex(grass);
	grass->setFloat("shadowPass", 0.0);

	Shader* phongInsSimp = render->shaders->addShader("phong_ins_simp", INSTANCE_SIMPLE_VERT, PHONG_FRAG);
	render->shaders->addShaderBindTex(phongInsSimp);
	phongInsSimp->setFloat("shadowPass", 0.0);

	Shader* bone = render->shaders->addShader("bone", BONE_VERT, BONE_FRAG);
	render->shaders->addShaderBindTex(bone);
	bone->setFloat("shadowPass", 0.0);

	Shader* billboard = render->shaders->addShader("billboard", BILLBOARD_VERT, BILLBOARD_FRAG);
	render->shaders->addShaderBindTex(billboard);
	billboard->setFloat("shadowPass", 0.0);

	Shader* terrain = render->shaders->addShader("terrain", TERRAIN_VERT, TERRAIN_FRAG);
	render->shaders->addShaderBindTex(terrain);

	Shader* grassLayer = render->shaders->addShader("grassLayer", GRASS_LAYER_VERT, GRASS_LAYER_FRAG, GRASS_LAYER_TESC, GRASS_LAYER_TESE, GRASS_LAYER_GEOM);
	render->shaders->addShaderBindTex(grassLayer);

	render->shaders->addShader("sky", SKY_VERT, SKY_FRAG);
	render->shaders->addShader("water", WATER_VERT, WATER_FRAG);

	Shader* grassLayerShadow = render->shaders->addShader("grassLayerShadow", GRASS_LAYER_VERT, GRASS_LAYER_FRAG, GRASS_LAYER_TESC, GRASS_LAYER_TESE, SHADOW_NONTEX_FRAG);
	render->shaders->addShaderBindTex(grassLayerShadow);

	Shader* phongShadow = render->shaders->addShader("phong_s", PHONG_VERT, SHADOW_TEX_FRAG);
	render->shaders->addShaderBindTex(phongShadow);
	phongShadow->setFloat("shadowPass", 1.0);
	phongShadow->setFloat("lowPass", 0.0);

	Shader* grassShadow = render->shaders->addShader("grass_s", GRASS_VERT, SHADOW_TEX_FRAG);
	render->shaders->addShaderBindTex(grassShadow);
	grassShadow->setFloat("shadowPass", 1.0);

	Shader* phongShadowIns = render->shaders->addShader("phong_s_ins", INSTANCE_VERT, SHADOW_TEX_FRAG);
	render->shaders->addShaderBindTex(phongShadowIns);
	phongShadowIns->setFloat("shadowPass", 1.0);
	phongShadowIns->setFloat("lowPass", 0.0);

	Shader* phongShadowInsSimp = render->shaders->addShader("phong_s_ins_simp", INSTANCE_SIMPLE_VERT, SHADOW_TEX_FRAG);
	render->shaders->addShaderBindTex(phongShadowInsSimp);
	phongShadowInsSimp->setFloat("shadowPass", 1.0);

	Shader* phongShadowLow = render->shaders->addShader("phong_sl", PHONG_VERT, SHADOW_TEX_FRAG);
	phongShadowLow->setFloat("shadowPass", 1.0);
	phongShadowLow->setFloat("lowPass", 1.0);

	Shader* phongSimpShadowLow = render->shaders->addShader("phong_sl_ins", INSTANCE_VERT, SHADOW_NONTEX_FRAG);
	phongSimpShadowLow->setFloat("shadowPass", 1.0);
	phongSimpShadowLow->setFloat("lowPass", 1.0);

	Shader* boneShadow = render->shaders->addShader("bone_s", BONE_VERT, SHADOW_NONTEX_FRAG);
	boneShadow->setFloat("shadowPass", 1.0);

	Shader* billboardShadow = render->shaders->addShader("billboard_s", BILLBOARD_VERT, SHADOW_TEX_FRAG);
	render->shaders->addShaderBindTex(billboardShadow);
	billboardShadow->setFloat("shadowPass", 1.0);

	Shader* deferred = render->shaders->addShader("deferred", POST_VERT, DEFERRED_FRAG);
	deferred->setSlot("texBuffer", 0);
	deferred->setSlot("matBuffer", 1);
	deferred->setSlot("normalGrassBuffer", 2);
	deferred->setSlot("roughMetalBuffer", 3);
	deferred->setSlot("depthBuffer", 4);

	Shader* fxaa = render->shaders->addShader("fxaa", POST_VERT, AA_FRAG);
	fxaa->setSlot("colorBuffer", 0);

	Shader* blur = render->shaders->addShader("blur", POST_VERT, BLUR_FRAG);
	blur->setSlot("colorBuffer", 0);

	Shader* mean = render->shaders->addShader("mean", POST_VERT, MEAN_FRAG);
	mean->setSlot("colorBuffer", 0);

	Shader* dof = render->shaders->addShader("dof", POST_VERT, DOF_FRAG);
	dof->setSlot("colorBufferLow", 0);
	dof->setSlot("colorBufferHigh", 1);

	render->shaders->addShader("debug", DEBUG_VERT, DEBUG_FRAG);
	
	Shader* screen = render->shaders->addShader("screen", POST_VERT, SCREEN_FRAG);
	screen->setSlot("tex", 0);

	Shader* ssr = render->shaders->addShader("ssr", POST_VERT, SSR_FRAG);
	ssr->setSlot("lightBuffer", 0);
	ssr->setSlot("matBuffer", 1);
	ssr->setSlot("normalBuffer", 2);
	ssr->setSlot("depthBuffer", 3);

	Shader* combined = render->shaders->addShader("combined", POST_VERT, COMBINE_FRAG);
	combined->setSlot("sceneBuffer", 0);
	combined->setSlot("sceneDepthBuffer", 1);
	combined->setSlot("waterBuffer", 2);
	combined->setSlot("waterDepthBuffer", 3);
	combined->setSlot("matBuffer", 4);
	combined->setSlot("waterNormalBuffer", 5);

	Shader* ssg = render->shaders->addShader("ssg", POST_VERT, SSG_FRAG);
	ssg->setSlot("colorBuffer", 0);
	ssg->setSlot("normalGrassBuffer", 1);
	ssg->setSlot("depthBuffer", 2);
}


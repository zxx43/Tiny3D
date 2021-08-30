#include "shaderscontainer.h"
#include "../shader/textfile.h"
#include "../mesh/terrain.h"
using namespace std;

#define SHADOW_TEX_FRAG "shader/shadow_tex.frag"
#define SHADOW_NONTEX_FRAG "shader/shadow_nontex.frag"
#define PHONG_VERT "shader/phong.vert"
#define PHONG_FRAG "shader/phong.frag"
#define INSTANCE_VERT "shader/instance.vert"
#define CULL_COMP "shader/cull.comp"
#define MULTI_COMP "shader/multiCull.comp"
#define FLUSH_COMP "shader/flush.comp"
#define BONE_VERT "shader/bone.vert"
#define BONE_FRAG "shader/bone.frag"
#define SKY_VERT "shader/sky.vert"
#define SKY_FRAG "shader/sky.frag"
#define POST_VERT "shader/post.vert"
#define DEFERRED_FRAG "shader/deferred.frag"
#define TERRAIN_VERT "shader/terrain.vert"
#define TERRAIN_FRAG "shader/terrain.frag"
#define TERRAIN_COMP "shader/terrain.comp"
#define WATER_VERT "shader/water.vert"
#define WATER_FRAG "shader/water.frag"
#define EDGE_FRAG "shader/edge.frag"
#define FXAA_FRAG "shader/fxaa.frag"
#define BLUR_FRAG "shader/blur.frag"
#define MEAN_FRAG "shader/mean.frag"
#define GAUSS_FRAG "shader/gaussian.frag"
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
#define GRASS_COMP "shader/grass.comp"
#define GRASS_VERT "shader/grass.vert"
#define ATMOS_VERT "shader/atmosphere.vert"
#define ATMOS_FRAG "shader/atmosphere.frag"
#define NOISE_VERT "shader/noise.vert"
#define NOISE_FRAG "shader/noise.frag"
#define DEPTH_FRAG "shader/depth.frag"
#define HIZ_FRAG "shader/hizmip.frag"

string LoadExShader(char* name) {
	char* fileStr = textFileRead(name);
	string shaderStr = string(fileStr);
	free(fileStr);
	return shaderStr;
}

void SetupShaders(ShaderManager* shaders, const ConfigArg* cfgs) {
	Shader* phong = shaders->addShader("phong", PHONG_VERT, PHONG_FRAG);
	shaders->addShaderBindTex(phong);

	Shader* phongIns = shaders->addShader("phong_ins", INSTANCE_VERT, PHONG_FRAG);
	shaders->addShaderBindTex(phongIns);

	Shader* billIns = shaders->addShader("bill_ins", INSTANCE_VERT, PHONG_FRAG);
	billIns->attachDef("BillPass", "1.0");
	shaders->addShaderBindTex(billIns);

	Shader* bone = shaders->addShader("bone", BONE_VERT, BONE_FRAG);
	shaders->addShaderBindTex(bone);

	Shader* terrain = shaders->addShader("terrain", TERRAIN_VERT, TERRAIN_FRAG);
	shaders->addShaderBindTex(terrain);

	Shader* terrainComp = shaders->addShader("terrainComp", TERRAIN_COMP);
	terrainComp->attachDef("CHUNK_INDEX_COUNT", to_string(CHUNK_INDEX_COUNT).data());
	terrainComp->attachDef("CHUNK_SIZE", to_string(CHUNK_SIZE).data());
	terrainComp->attachDef("LINE_CHUNKS", to_string(LINE_CHUNKS).data());

	Shader* grassLayer = shaders->addShader("grassLayer", GRASS_LAYER_VERT, GRASS_LAYER_FRAG, GRASS_LAYER_TESC, GRASS_LAYER_TESE, GRASS_LAYER_GEOM);
	shaders->addShaderBindTex(grassLayer);

	Shader* grassComp = shaders->addShader("grassComp", GRASS_COMP);
	grassComp->attachDef("WORKGROUP_SIZE", to_string(COMP_GROUPE_SIZE).data());

	Shader* grass = shaders->addShader("grass", GRASS_VERT, GRASS_LAYER_FRAG);
	Shader* sky = shaders->addShader("sky", SKY_VERT, SKY_FRAG);

	Shader* water = shaders->addShader("water", WATER_VERT, WATER_FRAG);
	if (!cfgs->ssr) water->attachDef("DISABLE_SSR", "1");
	if (cfgs->dynsky)
		water->attachDef("DYN_SKY", "1");

	Shader* phongShadow = shaders->addShader("phong_s", PHONG_VERT, SHADOW_TEX_FRAG);
	phongShadow->attachDef("ShadowPass", "1.0");
	shaders->addShaderBindTex(phongShadow);

	Shader* phongShadowIns = shaders->addShader("phong_s_ins", INSTANCE_VERT, SHADOW_TEX_FRAG);
	phongShadowIns->attachDef("ShadowPass", "1.0");
	shaders->addShaderBindTex(phongShadowIns);

	Shader* billShadowIns = shaders->addShader("bill_s_ins", INSTANCE_VERT, SHADOW_TEX_FRAG);
	billShadowIns->attachDef("ShadowPass", "1.0");
	billShadowIns->attachDef("BillPass", "1.0");
	shaders->addShaderBindTex(billShadowIns);

	Shader* phongShadowLow = shaders->addShader("phong_sl", PHONG_VERT, SHADOW_NONTEX_FRAG);
	phongShadowLow->attachDef("ShadowPass", "1.0");
	phongShadowLow->attachDef("LowPass", "1.0");

	Shader* phongSimpShadowLow = shaders->addShader("phong_sl_ins", INSTANCE_VERT, SHADOW_NONTEX_FRAG);
	phongSimpShadowLow->attachDef("ShadowPass", "1.0");
	phongSimpShadowLow->attachDef("LowPass", "1.0");

	Shader* billSimpShadowLow = shaders->addShader("bill_sl_ins", INSTANCE_VERT, SHADOW_NONTEX_FRAG);
	billSimpShadowLow->attachDef("ShadowPass", "1.0");
	billSimpShadowLow->attachDef("LowPass", "1.0");
	billSimpShadowLow->attachDef("BillPass", "1.0");

	Shader* boneShadow = shaders->addShader("bone_s", BONE_VERT, SHADOW_NONTEX_FRAG);
	boneShadow->attachDef("ShadowPass", "1.0");

	Shader* deferred = shaders->addShader("deferred", POST_VERT, DEFERRED_FRAG);
	if (cfgs->shadowQuality > 0)
		deferred->attachDef("USE_SHADOW", "1");
	if (cfgs->cartoon)
		deferred->attachDef("USE_CARTOON", "1");
	if (cfgs->dynsky)
		deferred->attachDef("DYN_SKY", "1");
	if (cfgs->bloom)
		deferred->attachDef("USE_BLOOM", "1");
	deferred->setSlot("texBuffer", 0);
	deferred->setSlot("matBuffer", 1);
	deferred->setSlot("roughMetalBuffer", 2);
	deferred->setSlot("depthBuffer", 3);

	Shader* edge = shaders->addShader("edge", POST_VERT, EDGE_FRAG);
	if (cfgs->cartoon)
		edge->attachDef("USE_CARTOON", "1");
	edge->setSlot("colorBuffer", 0);
	edge->setSlot("normalWaterBuffer", 1);
	edge->setSlot("matBuffer", 2);

	Shader* fxaa = shaders->addShader("fxaa", POST_VERT, FXAA_FRAG);
	fxaa->setSlot("colorBuffer", 0);

	Shader* blur = shaders->addShader("blur", POST_VERT, BLUR_FRAG);
	blur->setSlot("colorBuffer", 0);

	Shader* mean = shaders->addShader("mean", POST_VERT, MEAN_FRAG);
	mean->setSlot("colorBuffer", 0);

	Shader* gaussv = shaders->addShader("gaussv", POST_VERT, GAUSS_FRAG);
	gaussv->attachDef("PASS_V", "1");
	gaussv->setSlot("colorBuffer", 0);

	Shader* gaussh = shaders->addShader("gaussh", POST_VERT, GAUSS_FRAG);
	gaussh->attachDef("PASS_H", "1");
	gaussh->setSlot("colorBuffer", 0);

	Shader* dof = shaders->addShader("dof", POST_VERT, DOF_FRAG);
	dof->setSlot("colorBufferLow", 0);
	dof->setSlot("colorBufferHigh", 1);

	Shader* debug = shaders->addShader("debug", DEBUG_VERT, DEBUG_FRAG);
	
	Shader* screen = shaders->addShader("screen", POST_VERT, SCREEN_FRAG);
	screen->setSlot("tex", 0);

	Shader* ssr = shaders->addShader("ssr", POST_VERT, SSR_FRAG);
	if (cfgs->graphQuality > 3)
		ssr->attachDef("HIGH_QUALITY", "1");
	ssr->setSlot("lightBuffer", 0);
	ssr->setSlot("matBuffer", 1);
	ssr->setSlot("normalBuffer", 2);
	ssr->setSlot("depthBuffer", 3);

	Shader* combined = shaders->addShader("combined", POST_VERT, COMBINE_FRAG);
	if (cfgs->cartoon)
		combined->attachDef("USE_CARTOON", "1");
	if (cfgs->bloom)
		combined->attachDef("USE_BLOOM", "1");
	if (cfgs->graphQuality > 3)
		combined->attachDef("HIGH_QUALITY", "1");
	combined->setSlot("sceneBuffer", 0);
	combined->setSlot("sceneNormalBuffer", 1);
	combined->setSlot("sceneDepthBuffer", 2);
	combined->setSlot("waterBuffer", 3);
	combined->setSlot("waterMatBuffer", 4);
	combined->setSlot("waterNormalBuffer", 5);
	combined->setSlot("waterDepthBuffer", 6);
	combined->setSlot("bloomBuffer", 7);

	Shader* cull = shaders->addShader("cull", CULL_COMP);
	cull->attachDef("WORKGROUP_SIZE", to_string(WORKGROUPE_SIZE).data());

	Shader* multi = shaders->addShader("multi", MULTI_COMP);
	multi->attachDef("WORKGROUP_SIZE", to_string(WORKGROUPE_SIZE).data());
	multi->attachDef("MAX_DISPATCH", to_string(MAX_DISPATCH).data());
	multi->attachDef("InvalidIns", to_string(InvalidInsId).data());

	Shader* animMulti = shaders->addShader("animMulti", MULTI_COMP);
	animMulti->attachDef("WORKGROUP_SIZE", to_string(WORKGROUPE_SIZE).data());
	animMulti->attachDef("MAX_DISPATCH", to_string(MAX_DISPATCH).data());
	animMulti->attachDef("InvalidIns", to_string(InvalidInsId).data());
	animMulti->attachDef("AnimPass", "1.0");

	Shader* multiShadow = shaders->addShader("multi_s", MULTI_COMP);
	multiShadow->attachDef("WORKGROUP_SIZE", to_string(WORKGROUPE_SIZE).data());
	multiShadow->attachDef("MAX_DISPATCH", to_string(MAX_DISPATCH).data());
	multiShadow->attachDef("InvalidIns", to_string(InvalidInsId).data());
	multiShadow->attachDef("ShadowPass", "1.0");

	Shader* animMultiShadow = shaders->addShader("animMulti_s", MULTI_COMP);
	animMultiShadow->attachDef("WORKGROUP_SIZE", to_string(WORKGROUPE_SIZE).data());
	animMultiShadow->attachDef("MAX_DISPATCH", to_string(MAX_DISPATCH).data());
	animMultiShadow->attachDef("InvalidIns", to_string(InvalidInsId).data());
	animMultiShadow->attachDef("AnimPass", "1.0");
	animMultiShadow->attachDef("ShadowPass", "1.0");

	Shader* flush = shaders->addShader("flush", FLUSH_COMP);

	Shader* animFlush = shaders->addShader("animFlush", FLUSH_COMP);
	animFlush->attachDef("AnimPass", "1.0");

	Shader* atmos = shaders->addShader("atmos", ATMOS_VERT, ATMOS_FRAG);
	Shader* noise = shaders->addShader("noise", NOISE_VERT, NOISE_FRAG);
	Shader* depth = shaders->addShader("depth", POST_VERT, DEPTH_FRAG);
	Shader* hiz = shaders->addShader("hiz", POST_VERT, HIZ_FRAG);

	shaders->compile();
}


#include "shaderscontainer.h"
#include "../shader/textfile.h"
#include "../mesh/terrain.h"
#include "../mesh/water.h"
using namespace std;

#define SHADOW_TEX_FRAG "shader/shadow_tex.frag"
#define SHADOW_NONTEX_FRAG "shader/shadow_nontex.frag"
#define PHONG_VERT "shader/phong.vert"
#define PHONG_FRAG "shader/phong.frag"
#define INSTANCE_VERT "shader/instance.vert"
#define CULL_COMP "shader/cull.comp"
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
#define WATER_COMP "shader/water.comp"
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
#define IRR_FRAG "shader/irradiance.frag"
#define PREFILT_FRAG "shader/prefiltered.frag"
#define BRDF_FRAG "shader/brdf.frag"
#define NOISE_VERT "shader/noise.vert"
#define NOISE_FRAG "shader/noise.frag"
#define DEPTH_FRAG "shader/depth.frag"
#define HIZ_FRAG "shader/hizmip.frag"
#define CLEAR_COMP "shader/clear.comp"
#define LOD_COMP "shader/lod.comp"
#define REARRANGE_COMP "shader/rearrange.comp"
#define GATHER_COMP "shader/gather.comp"

string LoadExShader(char* name) {
	char* fileStr = textFileRead(name);
	string shaderStr = string(fileStr);
	free(fileStr);
	return shaderStr;
}

void SetupShaders(ShaderManager* shaders, const ConfigArg* cfgs) {
	Shader* phong = shaders->addShader("phong", PHONG_VERT, PHONG_FRAG);
	phong->attachDef("Shader", "phong");
	shaders->addShaderBindTex(phong);

	Shader* phongIns = shaders->addShader("phong_ins", INSTANCE_VERT, PHONG_FRAG);
	phongIns->attachDef("Shader", "instance");
	shaders->addShaderBindTex(phongIns);

	Shader* billIns = shaders->addShader("bill_ins", INSTANCE_VERT, PHONG_FRAG);
	billIns->attachDef("Shader", "bill_instance");
	billIns->attachDef("BillPass", "1.0");
	shaders->addShaderBindTex(billIns);

	Shader* bone = shaders->addShader("bone", BONE_VERT, BONE_FRAG);
	bone->attachDef("Shader", "bone");
	shaders->addShaderBindTex(bone);

	Shader* terrain = shaders->addShader("terrain", TERRAIN_VERT, TERRAIN_FRAG);
	terrain->attachDef("Shader", "terrain");
	shaders->addShaderBindTex(terrain);

	Shader* terrainDebug = shaders->addShader("terrain_debug", TERRAIN_VERT, TERRAIN_FRAG);
	terrainDebug->attachDef("Shader", "terrain_debug");
	shaders->addShaderBindTex(terrainDebug);
	terrainDebug->attachDef("UseDebug", "1.0");

	Shader* terrainComp = shaders->addShader("terrainComp", TERRAIN_COMP);
	terrainComp->attachDef("Shader", "terrain_culling");
	terrainComp->attachDef("CHUNK_INDEX_COUNT", to_string(CHUNK_INDEX_COUNT).data());
	terrainComp->attachDef("CHUNK_SIZE", to_string(CHUNK_SIZE).data());
	terrainComp->attachDef("LINE_CHUNKS", to_string(LINE_CHUNKS).data());
	terrainComp->attachDef("STEP_SIZE", to_string(STEP_SIZE).data());

	Shader* grassLayer = shaders->addShader("grassLayer", GRASS_LAYER_VERT, GRASS_LAYER_FRAG, GRASS_LAYER_TESC, GRASS_LAYER_TESE, GRASS_LAYER_GEOM);
	grassLayer->attachDef("Shader", "grass_geom");
	shaders->addShaderBindTex(grassLayer);

	Shader* grassComp = shaders->addShader("grassComp", GRASS_COMP);
	grassComp->attachDef("Shader", "grass_culling");
	grassComp->attachDef("WORKGROUP_SIZE", to_string(COMP_GROUPE_SIZE).data());
	grassComp->attachDef("CHUNK_SIZE", to_string(CHUNK_SIZE).data());
	grassComp->attachDef("LINE_CHUNKS", to_string(LINE_CHUNKS).data());
	grassComp->attachDef("STEP_SIZE", to_string(STEP_SIZE).data());

	Shader* grass = shaders->addShader("grass", GRASS_VERT, GRASS_LAYER_FRAG);
	grass->attachDef("Shader", "grass");

	Shader* sky = shaders->addShader("sky", SKY_VERT, SKY_FRAG);
	sky->attachDef("Shader", "sky");

	Shader* water = shaders->addShader("water", WATER_VERT, WATER_FRAG);
	water->attachDef("Shader", "water");
	if (!cfgs->ssr) water->attachDef("DISABLE_SSR", "1");
	if (cfgs->dynsky) water->attachDef("DYN_SKY", "1");

	Shader* waterComp = shaders->addShader("waterComp", WATER_COMP);
	waterComp->attachDef("Shader", "water_culling");
	waterComp->attachDef("CHUNK_INDEX_COUNT", to_string(WATER_CHUNK_INDEX_CNT).data());
	waterComp->attachDef("CHUNK_SIZE", to_string(WATER_CHUNK_SIZE).data());
	waterComp->attachDef("LINE_CHUNKS", to_string(WATER_LINE_CHUNKS).data());

	Shader* phongShadow = shaders->addShader("phong_s", PHONG_VERT, SHADOW_TEX_FRAG);
	phongShadow->attachDef("Shader", "phong_tex_shadow");
	phongShadow->attachDef("ShadowPass", "1.0");
	shaders->addShaderBindTex(phongShadow);

	Shader* phongShadowIns = shaders->addShader("phong_s_ins", INSTANCE_VERT, SHADOW_TEX_FRAG);
	phongShadowIns->attachDef("Shader", "instance_tex_shadow");
	phongShadowIns->attachDef("ShadowPass", "1.0");
	shaders->addShaderBindTex(phongShadowIns);

	Shader* billShadowIns = shaders->addShader("bill_s_ins", INSTANCE_VERT, SHADOW_TEX_FRAG);
	billShadowIns->attachDef("Shader", "billboard_tex_shadow");
	billShadowIns->attachDef("ShadowPass", "1.0");
	billShadowIns->attachDef("BillPass", "1.0");
	shaders->addShaderBindTex(billShadowIns);

	Shader* phongShadowLow = shaders->addShader("phong_sl", PHONG_VERT, SHADOW_NONTEX_FRAG);
	phongShadowLow->attachDef("Shader", "phong_shadow_notex");
	phongShadowLow->attachDef("ShadowPass", "1.0");
	phongShadowLow->attachDef("LowPass", "1.0");

	Shader* phongSimpShadowLow = shaders->addShader("phong_sl_ins", INSTANCE_VERT, SHADOW_NONTEX_FRAG);
	phongSimpShadowLow->attachDef("Shader", "instance_shadow_notex");
	phongSimpShadowLow->attachDef("ShadowPass", "1.0");
	phongSimpShadowLow->attachDef("LowPass", "1.0");

	Shader* billSimpShadowLow = shaders->addShader("bill_sl_ins", INSTANCE_VERT, SHADOW_NONTEX_FRAG);
	billSimpShadowLow->attachDef("Shader", "billboard_shadow_notex");
	billSimpShadowLow->attachDef("ShadowPass", "1.0");
	billSimpShadowLow->attachDef("LowPass", "1.0");
	billSimpShadowLow->attachDef("BillPass", "1.0");

	Shader* boneShadow = shaders->addShader("bone_s", BONE_VERT, SHADOW_NONTEX_FRAG);
	boneShadow->attachDef("Shader", "bone_shadow_notex");
	boneShadow->attachDef("ShadowPass", "1.0");

	Shader* deferred = shaders->addShader("deferred", POST_VERT, DEFERRED_FRAG);
	deferred->attachDef("Shader", "deferred");
	if (cfgs->shadowQuality > 0)
		deferred->attachDef("USE_SHADOW", "1");
	if (cfgs->cartoon)
		deferred->attachDef("USE_CARTOON", "1");
	if (cfgs->dynsky)
		deferred->attachDef("DYN_SKY", "1");
	if (cfgs->bloom)
		deferred->attachDef("USE_BLOOM", "1");
	deferred->attachDef("MAX_REFLECTION_LOD", to_string((float)MaxIblLevel - 1.0).data());
	deferred->setSlot("texBuffer", 0);
	deferred->setSlot("matBuffer", 1);
	deferred->setSlot("roughMetalBuffer", 2);
	deferred->setSlot("depthBuffer", 3);

	Shader* edge = shaders->addShader("edge", POST_VERT, EDGE_FRAG);
	edge->attachDef("Shader", "edge");
	if (cfgs->cartoon)
		edge->attachDef("USE_CARTOON", "1");
	edge->setSlot("colorBuffer", 0);
	edge->setSlot("normalWaterBuffer", 1);
	edge->setSlot("matBuffer", 2);

	Shader* edgeNFG = shaders->addShader("edge_nfg", POST_VERT, EDGE_FRAG);
	edgeNFG->attachDef("Shader", "edge_nofog");
	edgeNFG->attachDef("NO_FOG", "1");
	if (cfgs->cartoon)
		edgeNFG->attachDef("USE_CARTOON", "1");
	edgeNFG->setSlot("colorBuffer", 0);
	edgeNFG->setSlot("normalWaterBuffer", 1);
	edgeNFG->setSlot("matBuffer", 2);

	Shader* fxaa = shaders->addShader("fxaa", POST_VERT, FXAA_FRAG);
	fxaa->attachDef("Shader", "fxaa");
	fxaa->setSlot("colorBuffer", 0);

	Shader* blur = shaders->addShader("blur", POST_VERT, BLUR_FRAG);
	blur->attachDef("Shader", "blur");
	blur->setSlot("colorBuffer", 0);

	Shader* mean = shaders->addShader("mean", POST_VERT, MEAN_FRAG);
	mean->attachDef("Shader", "mean");
	mean->setSlot("colorBuffer", 0);

	Shader* gaussv = shaders->addShader("gaussv", POST_VERT, GAUSS_FRAG);
	gaussv->attachDef("Shader", "gaussv");
	gaussv->attachDef("PASS_V", "1");
	gaussv->setSlot("colorBuffer", 0);

	Shader* gaussh = shaders->addShader("gaussh", POST_VERT, GAUSS_FRAG);
	gaussh->attachDef("Shader", "gaussh");
	gaussh->attachDef("PASS_H", "1");
	gaussh->setSlot("colorBuffer", 0);

	Shader* dof = shaders->addShader("dof", POST_VERT, DOF_FRAG);
	dof->attachDef("Shader", "dof");
	dof->setSlot("colorBufferLow", 0);
	dof->setSlot("colorBufferHigh", 1);

	Shader* debug = shaders->addShader("debug", DEBUG_VERT, DEBUG_FRAG);
	debug->attachDef("Shader", "debug");

	Shader* screen = shaders->addShader("screen", POST_VERT, SCREEN_FRAG);
	screen->attachDef("Shader", "screen");
	screen->setSlot("tex", 0);

	Shader* ssr = shaders->addShader("ssr", POST_VERT, SSR_FRAG);
	ssr->attachDef("Shader", "ssr");
	if (cfgs->graphQuality > 3)
		ssr->attachDef("HIGH_QUALITY", "1");
	ssr->setSlot("lightBuffer", 0);
	ssr->setSlot("matBuffer", 1);
	ssr->setSlot("normalBuffer", 2);
	ssr->setSlot("depthBuffer", 3);

	Shader* combined = shaders->addShader("combined", POST_VERT, COMBINE_FRAG);
	combined->attachDef("Shader", "combined");
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

	Shader* combinedNFG = shaders->addShader("combined_nfg", POST_VERT, COMBINE_FRAG);
	combinedNFG->attachDef("Shader", "combined_nofog");
	combinedNFG->attachDef("NO_FOG", "1");
	if (cfgs->cartoon)
		combinedNFG->attachDef("USE_CARTOON", "1");
	if (cfgs->bloom)
		combinedNFG->attachDef("USE_BLOOM", "1");
	if (cfgs->graphQuality > 3)
		combinedNFG->attachDef("HIGH_QUALITY", "1");
	combinedNFG->setSlot("sceneBuffer", 0);
	combinedNFG->setSlot("sceneNormalBuffer", 1);
	combinedNFG->setSlot("sceneDepthBuffer", 2);
	combinedNFG->setSlot("waterBuffer", 3);
	combinedNFG->setSlot("waterMatBuffer", 4);
	combinedNFG->setSlot("waterNormalBuffer", 5);
	combinedNFG->setSlot("waterDepthBuffer", 6);
	combinedNFG->setSlot("bloomBuffer", 7);

	Shader* cull = shaders->addShader("cull", CULL_COMP);
	cull->attachDef("Shader", "culling");
	cull->attachDef("WORKGROUP_SIZE", to_string(WORKGROUPE_SIZE).data());

	Shader* atmos = shaders->addShader("atmos", ATMOS_VERT, ATMOS_FRAG);
	atmos->attachDef("Shader", "atmos");

	Shader* noise = shaders->addShader("noise", NOISE_VERT, NOISE_FRAG);
	noise->attachDef("Shader", "noise");

	Shader* depth = shaders->addShader("depth", POST_VERT, DEPTH_FRAG);
	depth->attachDef("Shader", "depth_view");

	Shader* hiz = shaders->addShader("hiz", POST_VERT, HIZ_FRAG);
	hiz->attachDef("Shader", "hizgen");

	Shader* irr = shaders->addShader("irradiance", ATMOS_VERT, IRR_FRAG);
	irr->attachDef("Shader", "irradiance");
	if (cfgs->dynsky) irr->attachDef("DYN_SKY", "1");

	Shader* prefilt = shaders->addShader("prefiltered", ATMOS_VERT, PREFILT_FRAG);
	prefilt->attachDef("Shader", "prefiltered");
	if (cfgs->dynsky) prefilt->attachDef("DYN_SKY", "1");

	Shader* brdf = shaders->addShader("brdf", POST_VERT, BRDF_FRAG);
	brdf->attachDef("Shader", "brdf");

	Shader* clear = shaders->addShader("clearProcessor", CLEAR_COMP);
	clear->attachDef("Shader", "clear");

	Shader* lod = shaders->addShader("lodProcessor", LOD_COMP);
	lod->attachDef("Shader", "lod");

	Shader* rearrange = shaders->addShader("rearrangeProcessor", REARRANGE_COMP);
	rearrange->attachDef("Shader", "rearrange");
	
	Shader* gather = shaders->addShader("gatherProcessor", GATHER_COMP);
	gather->attachDef("Shader", "gather");

	shaders->compile();
}


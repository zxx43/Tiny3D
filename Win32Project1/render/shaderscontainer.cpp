#include "shaderscontainer.h"

#define PHONG_VERT "shader/phong.vert"
#define PHONG_FRAG "shader/phong.frag"
#define GRASS_VERT "shader/phong_grass.vert"
#define GRASS_FRAG "shader/phong_grass.frag"
#define GRASS_GEOM "shader/phong_grass.geom"
#define PHONG_INS_VERT "shader/phong_ins.vert"
#define PHONG_INS_FRAG "shader/phong_ins.frag"
#define PHONG_INS_SIMPLE_VERT "shader/phong_ins_simp.vert"
#define PHONG_INS_SIMPLE_FRAG "shader/phong_ins_simp.frag"
#define BONE_VERT "shader/bone.vert"
#define BONE_FRAG "shader/bone.frag"
#define PHONG_SHADOW_VERT "shader/phong_shadow.vert"
#define PHONG_SHADOW_FRAG "shader/phong_shadow.frag"
#define GRASS_SHADOW_VERT "shader/phong_shadow_grass.vert"
#define GRASS_SHADOW_FRAG "shader/phong_shadow_grass.frag"
#define PHONG_SHADOW_INS_VERT "shader/phong_shadow_ins.vert"
#define PHONG_SHADOW_INS_FRAG "shader/phong_shadow_ins.frag"
#define PHONG_SHADOW_INS_SIMPLE_VERT "shader/phong_shadow_ins_simp.vert"
#define PHONG_SHADOW_INS_SIMPLE_FRAG "shader/phong_shadow_ins_simp.frag"
#define PHONG_SHADOW_LOW_VERT "shader/phong_shadow_low.vert"
#define PHONG_SHADOW_LOW_INS_VERT "shader/phong_shadow_low_ins.vert"
#define PHONG_SHADOW_LOW_FRAG "shader/phong_shadow_low.frag"
#define BONE_SHADOW_VERT "shader/bone_shadow.vert"
#define BONE_SHADOW_FRAG "shader/bone_shadow.frag"
#define SKY_VERT "shader/sky.vert"
#define SKY_FRAG "shader/sky.frag"
#define DEFERRED_VERT "shader/deferred.vert"
#define DEFERRED_FRAG "shader/deferred.frag"
#define TERRAIN_VERT "shader/terrain.vert"
#define TERRAIN_FRAG "shader/terrain.frag"
#define BILLBOARD_VERT "shader/billboard.vert"
#define BILLBOARD_FRAG "shader/billboard.frag"
#define BILLBOARD_SHADOW_VERT "shader/billboard_shadow.vert"
#define BILLBOARD_SHADOW_FRAG "shader/billboard_shadow.frag"
#define WATER_VERT "shader/water.vert"
#define WATER_FRAG "shader/water.frag"
#define AA_VERT "shader/fxaa.vert"
#define AA_FRAG "shader/fxaa.frag"
#define BLUR_VERT "shader/blur.vert"
#define BLUR_FRAG "shader/blur.frag"
#define MEAN_VERT "shader/mean.vert"
#define MEAN_FRAG "shader/mean.frag"
#define DOF_VERT "shader/dof.vert"
#define DOF_FRAG "shader/dof.frag"
#define DEBUG_VERT "shader/debug.vert"
#define DEBUG_FRAG "shader/debug.frag"
#define SCREEN_VERT "shader/screen.vert"
#define SCREEN_FRAG "shader/screen.frag"
#define SSR_VERT "shader/ssr.vert"
#define SSR_FRAG "shader/ssr.frag"
#define COMBINE_VERT "shader/combined.vert"
#define COMBINE_FRAG "shader/combined.frag"
#define SSG_VERT "shader/ssg.vert"
#define SSG_FRAG "shader/ssg.frag"

void SetupShaders(Render* render) {
	Shader* phong=render->shaders->addShader("phong",PHONG_VERT,PHONG_FRAG);
	render->setShaderSampler(phong, "texture", 0);

	Shader* phongIns = render->shaders->addShader("phong_ins", PHONG_INS_VERT, PHONG_INS_FRAG);
	render->setShaderSampler(phongIns, "texture", 0);

	//Shader* grass = render->shaders->addShader("grass", GRASS_VERT, GRASS_FRAG, GRASS_GEOM);
	Shader* grass = render->shaders->addShader("grass", GRASS_VERT, GRASS_FRAG);
	render->setShaderSampler(grass, "texture", 0);

	Shader* phongInsSimp = render->shaders->addShader("phong_ins_simp", PHONG_INS_SIMPLE_VERT, PHONG_INS_SIMPLE_FRAG);
	render->setShaderSampler(phongInsSimp, "texture", 0);

	Shader* terrain = render->shaders->addShader("terrain", TERRAIN_VERT, TERRAIN_FRAG);
	render->setShaderSampler(terrain, "texArray", 0);

	Shader* bone=render->shaders->addShader("bone",BONE_VERT,BONE_FRAG);
	render->setShaderSampler(bone, "texture", 0);

	Shader* phongShadow = render->shaders->addShader("phong_s", PHONG_SHADOW_VERT, PHONG_SHADOW_FRAG);
	render->setShaderSampler(phongShadow, "texture", 0);

	Shader* grassShadow = render->shaders->addShader("grass_s", GRASS_SHADOW_VERT, GRASS_SHADOW_FRAG);
	render->setShaderSampler(grassShadow, "texture", 0);

	Shader* phongShadowIns = render->shaders->addShader("phong_s_ins", PHONG_SHADOW_INS_VERT, PHONG_SHADOW_INS_FRAG);
	render->setShaderSampler(phongShadowIns, "texture", 0);

	Shader* phongShadowInsSimp = render->shaders->addShader("phong_s_ins_simp", PHONG_SHADOW_INS_SIMPLE_VERT, PHONG_SHADOW_INS_SIMPLE_FRAG);
	render->setShaderSampler(phongShadowInsSimp, "texture", 0);

	render->shaders->addShader("phong_sl", PHONG_SHADOW_LOW_VERT, PHONG_SHADOW_LOW_FRAG);
	render->shaders->addShader("phong_sl_ins", PHONG_SHADOW_LOW_INS_VERT, PHONG_SHADOW_LOW_FRAG);
	render->shaders->addShader("bone_s", BONE_SHADOW_VERT, BONE_SHADOW_FRAG);

	Shader* sky=render->shaders->addShader("sky",SKY_VERT,SKY_FRAG);
	render->setShaderSampler(sky, "textureSky", 0);

	Shader* billboard = render->shaders->addShader("billboard", BILLBOARD_VERT, BILLBOARD_FRAG);
	render->setShaderSampler(billboard, "texture", 0);

	Shader* billboardShadow = render->shaders->addShader("billboard_s", BILLBOARD_SHADOW_VERT, BILLBOARD_SHADOW_FRAG);
	render->setShaderSampler(billboardShadow, "texture", 0);

	Shader* water = render->shaders->addShader("water", WATER_VERT, WATER_FRAG);
	render->setShaderSampler(water, "textureEnv", 1);
	render->setShaderSampler(water, "reflectBuffer", 2);

	Shader* deferred = render->shaders->addShader("deferred", DEFERRED_VERT, DEFERRED_FRAG);
	render->setShaderSampler(deferred, "texBuffer", 0);
	render->setShaderSampler(deferred, "matBuffer", 1);
	render->setShaderSampler(deferred, "normalGrassBuffer", 2);
	render->setShaderSampler(deferred, "depthBuffer", 3);
	render->setShaderSampler(deferred, "depthBufferNear", 4);
	render->setShaderSampler(deferred, "depthBufferMid", 5);
	render->setShaderSampler(deferred, "depthBufferFar", 6);

	Shader* fxaa = render->shaders->addShader("fxaa", AA_VERT, AA_FRAG);
	render->setShaderSampler(fxaa, "colorBuffer", 0);

	Shader* blur = render->shaders->addShader("blur", BLUR_VERT, BLUR_FRAG);
	render->setShaderSampler(blur, "colorBuffer", 0);

	Shader* mean = render->shaders->addShader("mean", MEAN_VERT, MEAN_FRAG);
	render->setShaderSampler(mean, "colorBuffer", 0);

	Shader* dof = render->shaders->addShader("dof", DOF_VERT, DOF_FRAG);
	render->setShaderSampler(dof, "colorBufferLow", 0);
	render->setShaderSampler(dof, "colorBufferHigh", 1);

	Shader* debug = render->shaders->addShader("debug", DEBUG_VERT, DEBUG_FRAG);

	Shader* screen = render->shaders->addShader("screen", SCREEN_VERT, SCREEN_FRAG);
	render->setShaderSampler(screen, "colorBuffer", 0);

	Shader* ssr = render->shaders->addShader("ssr", SSR_VERT, SSR_FRAG);
	render->setShaderSampler(ssr, "lightBuffer", 0);
	render->setShaderSampler(ssr, "matBuffer", 1);
	render->setShaderSampler(ssr, "normalBuffer", 2);
	render->setShaderSampler(ssr, "depthBuffer", 3);

	Shader* combined = render->shaders->addShader("combined", COMBINE_VERT, COMBINE_FRAG);
	render->setShaderSampler(combined, "sceneBuffer", 0);
	render->setShaderSampler(combined, "sceneDepthBuffer", 1);
	render->setShaderSampler(combined, "waterBuffer", 2);
	render->setShaderSampler(combined, "waterDepthBuffer", 3);
	render->setShaderSampler(combined, "matBuffer", 4);
	render->setShaderSampler(combined, "waterNormalBuffer", 5);

	Shader* ssg = render->shaders->addShader("ssg", SSG_VERT, SSG_FRAG);
	render->setShaderSampler(ssg, "colorBuffer", 0);
	render->setShaderSampler(ssg, "normalGrassBuffer", 1);
	render->setShaderSampler(ssg, "depthBuffer", 2);
}


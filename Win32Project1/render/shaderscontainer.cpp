#include "shaderscontainer.h"

#define PHONG_VERT "shader/phong.vert"
#define PHONG_FRAG "shader/phong.frag"
#define PHONG_INS_VERT "shader/phong_ins.vert"
#define PHONG_INS_FRAG "shader/phong_ins.frag"
#define BONE_VERT "shader/bone.vert"
#define BONE_FRAG "shader/bone.frag"
#define PHONG_SHADOW_VERT "shader/phong_shadow.vert"
#define PHONG_SHADOW_INS_VERT "shader/phong_shadow_ins.vert"
#define PHONG_SHADOW_FRAG "shader/phong_shadow.frag"
#define PHONG_SHADOW_LOW_VERT "shader/phong_shadow_low.vert"
#define PHONG_SHADOW_LOW_INS_VERT "shader/phong_shadow_low_ins.vert"
#define PHONG_SHADOW_LOW_FRAG "shader/phong_shadow_low.frag"
#define BONE_SHADOW_VERT "shader/bone_shadow.vert"
#define BONE_SHADOW_FRAG "shader/bone_shadow.frag"
#define SKY_VERT "shader/sky.vert"
#define SKY_FRAG "shader/sky.frag"
#define BLUR_VERT "shader/blur.vert"
#define BLUR_FRAG "shader/blur.frag"
#define TERRAIN_VERT "shader/terrain.vert"
#define TERRAIN_FRAG "shader/terrain.frag"

void SetupShaders(Render* render) {
	Shader* phong=render->shaders->addShader("phong",PHONG_VERT,PHONG_FRAG);
	render->useShader(phong);
	phong->setSampler("texture",0);
	phong->setInt("useShadow", 0);

	Shader* phongIns = render->shaders->addShader("phong_ins", PHONG_INS_VERT, PHONG_INS_FRAG);
	render->useShader(phongIns);
	phongIns->setSampler("texture", 0);
	phongIns->setInt("useShadow", 0);

	Shader* terrain = render->shaders->addShader("terrain", TERRAIN_VERT, TERRAIN_FRAG);
	render->useShader(terrain);
	terrain->setSampler("texture", 0);
	terrain->setInt("useShadow", 0);

	Shader* bone=render->shaders->addShader("bone",BONE_VERT,BONE_FRAG);
	render->useShader(bone);
	bone->setSampler("texture",0);

	render->shaders->addShader("phong_s", PHONG_SHADOW_VERT, PHONG_SHADOW_FRAG);
	render->shaders->addShader("phong_sl", PHONG_SHADOW_LOW_VERT, PHONG_SHADOW_LOW_FRAG);
	render->shaders->addShader("phong_s_ins", PHONG_SHADOW_INS_VERT, PHONG_SHADOW_FRAG);
	render->shaders->addShader("phong_sl_ins", PHONG_SHADOW_LOW_INS_VERT, PHONG_SHADOW_LOW_FRAG);
	render->shaders->addShader("bone_s", BONE_SHADOW_VERT, BONE_SHADOW_FRAG);

	Shader* sky=render->shaders->addShader("sky",SKY_VERT,SKY_FRAG);
	render->useShader(sky);
	sky->setSampler("textureSky",0);

	Shader* blur=render->shaders->addShader("blur",BLUR_VERT,BLUR_FRAG);
	render->useShader(blur);
	blur->setSampler("texBuffer", 0);
	blur->setSampler("colorBuffer", 1);
	blur->setSampler("normalBuffer", 2);
	blur->setSampler("depthBuffer", 3);
	blur->setSampler("depthBufferNear", 4);
	blur->setSampler("depthBufferMid", 5);
	blur->setSampler("depthBufferFar", 6);
}


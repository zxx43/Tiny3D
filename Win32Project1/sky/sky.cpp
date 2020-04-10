#include "sky.h"
#include "../util/util.h"
#include "../object/staticObject.h"
#include "../assets/assetManager.h"
#include "../scene/scene.h"

Sky::Sky(Scene* scene, bool dyn) {
	mesh=new Sphere(16,16);
	StaticObject* skyObject=new StaticObject(mesh);
	Material* mat = new Material("sky_mat");
	mat->diffuse = vec3(0.5f, 0.7f, 0.8f);
	skyObject->bindMaterial(MaterialManager::materials->add(mat));
	skyObject->setPosition(0,0,0);
	skyObject->setSize(4, 4, 4);
	skyNode=new StaticNode(vec3(0,0,0));
	skyNode->setFullStatic(true);
	skyNode->addObject(scene, skyObject);
	skyNode->updateNode();
	skyNode->prepareDrawcall();

	
	CubeMap* env = new CubeMap("texture/sky/xpos.bmp", "texture/sky/xneg.bmp",
			"texture/sky/yneg.bmp", "texture/sky/ypos.bmp",
			"texture/sky/zpos.bmp", "texture/sky/zneg.bmp");
	AssetManager::assetManager->setEnvTexture(env);

	if (dyn) {
		CubeMap* texture = new CubeMap(512, 512);
		AssetManager::assetManager->setSkyTexture(texture);
		skyBuff = new FrameBuffer(texture);
	} else {
		AssetManager::assetManager->setSkyTexture(env);
		skyBuff = NULL;
	}

	mat4 proj = perspective(90.0, 1.0, 0.5, 20.0);
	matPosx = proj * viewMat(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0, 0.0, 0.0));
	matNegx = proj * viewMat(vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0, 0.0, 0.0));
	matNegy = proj * viewMat(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0, 0.0, 0.0));
	matPosy = proj * viewMat(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0, 0.0, 0.0));
	matNegz = proj * viewMat(vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0, 0.0, 0.0));
	matPosz = proj * viewMat(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0, 0.0, 0.0));

	state = new RenderState();
	state->cullMode = CULL_FRONT;
	state->lightEffect = false;
}

Sky::~Sky() {
	delete mesh; mesh=NULL;
	delete skyNode; skyNode=NULL;
	delete state; state = NULL;
	delete skyBuff; skyBuff = NULL;
}

void Sky::update(Render* render, const vec3& sunPos, Shader* shader) {
	if (!skyBuff) return;
	state->delay = 0;
	state->skyPass = false;
	state->atmoPass = true;
	state->shader = shader;
	state->shader->setVector3("light", -sunPos.x, -sunPos.y, -sunPos.z);
	render->useFrameBuffer(skyBuff);
	
	render->useFrameCube(0);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosx);
	render->draw(NULL, skyNode->drawcall, state);

	render->useFrameCube(1);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegx);
	render->draw(NULL, skyNode->drawcall, state);

	render->useFrameCube(2);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosy);
	render->draw(NULL, skyNode->drawcall, state);

	render->useFrameCube(3);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegy);
	render->draw(NULL, skyNode->drawcall, state);

	render->useFrameCube(4);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosz);
	render->draw(NULL, skyNode->drawcall, state);

	render->useFrameCube(5);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegz);
	render->draw(NULL, skyNode->drawcall, state);
}

void Sky::draw(Render* render,Shader* shader,Camera* camera) {
	state->delay = DELAY_FRAME;
	state->shader = shader;
	state->skyPass = true;
	state->atmoPass = false;
	render->draw(camera,skyNode->drawcall,state);
}


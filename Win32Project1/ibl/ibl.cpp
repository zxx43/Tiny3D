#include "ibl.h"
#include "../util/util.h"
#include "../object/staticObject.h"
#include "../assets/assetManager.h"
#include "../scene/scene.h"

Ibl::Ibl(Scene* scene) {
	mesh = new Sphere(16, 16);
	StaticObject* iblObject = new StaticObject(mesh);
	Material* mat = new Material("sky_mat");
	mat->diffuse = vec3(0.5f, 0.7f, 0.8f);
	iblObject->bindMaterial(MaterialManager::materials->add(mat));
	iblObject->setPosition(0, 0, 0);
	iblObject->setSize(4, 4, 4);
	iblNode = new StaticNode(vec3(0, 0, 0));
	iblNode->setFullStatic(true);
	iblNode->addObject(scene, iblObject);
	iblNode->updateNode(scene);
	iblNode->prepareDrawcall();

	irradianceTex = new CubeMap(512, 512, false, FLOAT_PRE);
	irradianceBuff = new FrameBuffer(irradianceTex);

	mat4 proj = perspective(90.0, 1.0, 0.5, 10.0);
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

Ibl::~Ibl() {
	delete mesh; mesh = NULL;
	delete iblNode; iblNode = NULL;
	delete state; state = NULL;
	if (irradianceBuff) delete irradianceBuff; irradianceBuff = NULL;
}

void Ibl::generate(Render* render, Shader* shader) {
	if (!irradianceBuff) return;
	state->delay = 0;
	state->iblPass = true;
	state->shader = shader;
	render->useFrameBuffer(irradianceBuff);

	render->useFrameCube(0);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosx);
	render->draw(NULL, iblNode->drawcall, state);

	render->useFrameCube(1);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegx);
	render->draw(NULL, iblNode->drawcall, state);

	render->useFrameCube(2);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosy);
	render->draw(NULL, iblNode->drawcall, state);

	render->useFrameCube(3);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegy);
	render->draw(NULL, iblNode->drawcall, state);

	render->useFrameCube(4);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosz);
	render->draw(NULL, iblNode->drawcall, state);

	render->useFrameCube(5);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegz);
	render->draw(NULL, iblNode->drawcall, state);
}
#include "ibl.h"
#include "../util/util.h"
#include "../object/staticObject.h"
#include "../assets/assetManager.h"
#include "../scene/scene.h"

Ibl::Ibl(Scene* scene) {
	sphere = new Sphere(16, 16);
	StaticObject* cubeObject = new StaticObject(sphere);
	Material* mat = new Material("sky_mat");
	mat->diffuse = vec3(0.5f, 0.7f, 0.8f);
	cubeObject->bindMaterial(MaterialManager::materials->add(mat));
	cubeObject->setPosition(0, 0, 0);
	cubeObject->setSize(4, 4, 4);
	cubeNode = new StaticNode(vec3(0, 0, 0));
	cubeNode->setFullStatic(true);
	cubeNode->addObject(scene, cubeObject);
	cubeNode->updateNode(scene);
	cubeNode->prepareDrawcall();

	irradianceTex = new CubeMap(512, 512, false, FLOAT_PRE);
	irradianceBuff = new FrameBuffer(irradianceTex);

	prefilteredTex = new CubeMap(512, 512, true, FLOAT_PRE);
	prefilteredBuff = new FrameBuffer(prefilteredTex);

	board = new Board(2, 2, 2);
	boardNode = new StaticNode(vec3(0, 0, 0));
	boardNode->setFullStatic(true);
	StaticObject* boardObject = new StaticObject(board);
	boardNode->addObject(NULL, boardObject);
	boardNode->prepareDrawcall();

	brdfBuff = new FrameBuffer(128, 128, LOW_PRE, 2, WRAP_CLAMP_TO_EDGE);
	brdfLut = brdfBuff->getColorBuffer(0);
	brdfInited = false;

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
	delete sphere; sphere = NULL;
	delete cubeNode; cubeNode = NULL;
	delete board; board = NULL;
	delete boardNode; boardNode = NULL;
	delete state; state = NULL;
	if (irradianceBuff) delete irradianceBuff; irradianceBuff = NULL;
	if (prefilteredBuff) delete prefilteredBuff; prefilteredBuff = NULL;
	if (brdfBuff) delete brdfBuff; brdfBuff = NULL;
}

void Ibl::genIrradiance(Render* render, Shader* shader) {
	if (!irradianceBuff) return;
	state->delay = 0;
	state->cullMode = CULL_FRONT;
	state->iblPass = true;
	state->shader = shader;
	render->useFrameBuffer(irradianceBuff);

	render->useFrameCube(0, 0);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosx);
	render->draw(NULL, cubeNode->drawcall, state);

	render->useFrameCube(1, 0);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegx);
	render->draw(NULL, cubeNode->drawcall, state);

	render->useFrameCube(2, 0);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosy);
	render->draw(NULL, cubeNode->drawcall, state);

	render->useFrameCube(3, 0);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegy);
	render->draw(NULL, cubeNode->drawcall, state);

	render->useFrameCube(4, 0);
	render->setShaderMat4(shader, "viewProjectMatrix", matPosz);
	render->draw(NULL, cubeNode->drawcall, state);

	render->useFrameCube(5, 0);
	render->setShaderMat4(shader, "viewProjectMatrix", matNegz);
	render->draw(NULL, cubeNode->drawcall, state);
}

void Ibl::genPrefiltered(Render* render, Shader* shader) {
	if (!prefilteredBuff) return;
	state->delay = 0;
	state->cullMode = CULL_FRONT;
	state->iblPass = true;
	state->shader = shader;

	uint maxMipLevels = MaxIblLevel;
	render->useFrameBuffer(prefilteredBuff);

	for (uint mip = 0; mip < maxMipLevels; ++mip) {
		float roughness = (float)mip / (float)(maxMipLevels - 1);

		render->useFrameCube(0, mip);
		render->setShaderMat4(shader, "viewProjectMatrix", matPosx);
		render->setShaderFloat(shader, "uRoughness", roughness);
		render->draw(NULL, cubeNode->drawcall, state);

		render->useFrameCube(1, mip);
		render->setShaderMat4(shader, "viewProjectMatrix", matNegx);
		render->setShaderFloat(shader, "uRoughness", roughness);
		render->draw(NULL, cubeNode->drawcall, state);

		render->useFrameCube(2, mip);
		render->setShaderMat4(shader, "viewProjectMatrix", matPosy);
		render->setShaderFloat(shader, "uRoughness", roughness);
		render->draw(NULL, cubeNode->drawcall, state);

		render->useFrameCube(3, mip);
		render->setShaderMat4(shader, "viewProjectMatrix", matNegy);
		render->setShaderFloat(shader, "uRoughness", roughness);
		render->draw(NULL, cubeNode->drawcall, state);

		render->useFrameCube(4, mip);
		render->setShaderMat4(shader, "viewProjectMatrix", matPosz);
		render->setShaderFloat(shader, "uRoughness", roughness);
		render->draw(NULL, cubeNode->drawcall, state);

		render->useFrameCube(5, mip);
		render->setShaderMat4(shader, "viewProjectMatrix", matNegz);
		render->setShaderFloat(shader, "uRoughness", roughness);
		render->draw(NULL, cubeNode->drawcall, state);
	}
}

void Ibl::genBrdf(Render* render, Shader* shader) {
	if (!brdfBuff) return;
	if (brdfInited) return;
	state->delay = 0;
	state->cullMode = CULL_BACK;
	state->iblPass = true;
	state->shader = shader;

	render->setFrameBuffer(brdfBuff);
	render->draw(NULL, boardNode->drawcall, state);
	brdfInited = true;
}
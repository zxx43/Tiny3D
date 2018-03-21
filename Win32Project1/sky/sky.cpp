#include "sky.h"
#include "../util/util.h"
#include "../object/staticObject.h"

Sky::Sky() {
	mesh=new Sphere(8,8);
	StaticObject* skyObject=new StaticObject(mesh);
	Material* mat = new Material("sky_mat");
	mat->diffuse = VECTOR3D(0.5f, 0.7f, 0.8f);
	skyObject->bindMaterial(MaterialManager::materials->add(mat));
	skyObject->setPosition(0,0,0);
	skyNode=new StaticNode(VECTOR3D(0,0,0));
	skyNode->fullStatic = true;
	skyNode->addObject(skyObject);
	skyNode->prepareDrawcall();

	texture=new CubeMap("texture/sky/sky_ft.bmp","texture/sky/sky_bk.bmp",
			"texture/sky/sky_dn.bmp","texture/sky/sky_up.bmp",
			"texture/sky/sky_rt.bmp","texture/sky/sky_lf.bmp");

	state = new RenderState();
	state->cullMode = CULL_FRONT;
	state->lightEffect = false;
	state->skyPass = true;
}

Sky::~Sky() {
	delete mesh; mesh=NULL;
	delete skyNode; skyNode=NULL;
	delete texture; texture=NULL;
	delete state; state = NULL;
}

void Sky::draw(Render* render,Shader* shader,Camera* camera) {
	state->shader = shader;
	render->useShader(shader);
	render->useTexture(TEXTURE_CUBE, 0, texture->id);
	render->draw(camera,skyNode->drawcall,state);
}


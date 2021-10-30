#ifndef IBL_H_
#define IBL_H_

#include "../mesh/sphere.h"
#include "../node/staticNode.h"
#include "../render/render.h"
#include "../camera/camera.h"

class Scene;

class Ibl {
private:
	StaticNode* iblNode;
	Sphere* mesh;
	FrameBuffer* irradianceBuff;
	mat4 matPosx, matNegx, matPosy, matNegy, matPosz, matNegz;
	CubeMap* irradianceTex;
public:
	RenderState* state;
public:
	Ibl(Scene* scene);
	~Ibl();
public:
	void generate(Render* render, Shader* shader);
	CubeMap* getIrradianceTex() { return irradianceTex; }
};

#endif

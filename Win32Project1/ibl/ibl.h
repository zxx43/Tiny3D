#ifndef IBL_H_
#define IBL_H_

#include "../mesh/sphere.h"
#include "../mesh/board.h"
#include "../node/staticNode.h"
#include "../render/render.h"
#include "../camera/camera.h"

class Scene;

class Ibl {
private:
	StaticNode* cubeNode;
	StaticNode* boardNode;
	Sphere* sphere;
	Board* board;
	FrameBuffer* irradianceBuff;
	FrameBuffer* prefilteredBuff;
	FrameBuffer* brdfBuff;
	bool brdfInited;
	CubeMap* irradianceTex;
	CubeMap* prefilteredTex;
	Texture* brdfLut;
	mat4 matPosx, matNegx, matPosy, matNegy, matPosz, matNegz;
public:
	RenderState* state;
public:
	Ibl(Scene* scene);
	~Ibl();
public:
	void genIrradiance(Render* render, Shader* shader);
	void genPrefiltered(Render* render, Shader* shader);
	void genBrdf(Render* render, Shader* shader);
	CubeMap* getIrradianceTex() { return irradianceTex; }
	CubeMap* getPrefilteredTex() { return prefilteredTex; }
	Texture* getBrdf() { return brdfLut; }
};

#endif

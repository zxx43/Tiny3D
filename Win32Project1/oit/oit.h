#ifndef OIT_H_
#define OIT_H_

#include "../texture/image2d.h"
#include "../render/render.h"
#include "../filter/filter.h"

class Oit {
private:
	int scrWidth, scrHeight;
	Image2D* oitHeader;
	FrameBuffer* clearBuff;
	RenderBuffer* oitCounter;
	RenderBuffer* oitList;
public:
	FrameBuffer* oitBuf;
	std::vector<Texture*> blendInput;
	Filter* blendFilter;
public:
	Oit();
	~Oit();
private:
	void useOitCounter(int loc);
	void unuseOitCounter(int loc);
	void useOitList(int loc);
	void unuseOitList(int loc);
public:
	void resetOit(Render* render, Shader* shader);
	void beginRenderOit(Render* render, RenderState* state, Shader* shader);
	void endRenderOit(Render* render);
	void beginBlendOit(Render* render, RenderState* state, Shader* shader, Texture* backgroundTex);
	void endBlendOit();
	void resize(int width, int height);
	void createOitFramebuffers(FrameBuffer* screen, bool hasNextEffect, const int outputPre);
};

#endif

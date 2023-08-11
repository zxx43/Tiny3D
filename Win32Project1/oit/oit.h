#ifndef OIT_H_
#define OIT_H_

#include "../texture/image2d.h"
#include "../render/render.h"

class Oit {
private:
	int scrWidth, scrHeight;
	Image2D* oitHeader;
	RenderBuffer* oitCounter;
	RenderBuffer* oitList;
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
	void blendOit(Render* render, RenderState* state, Shader* shader);
	void resize(int width, int height);
};

#endif

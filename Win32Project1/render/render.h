/*
 * render.h
 *
 *  Created on: 2017-4-7
 *      Author: a
 */

#ifndef RENDER_H_
#define RENDER_H_

#include "../shader/shadermanager.h"
#include "../camera/camera.h"
#include "../framebuffer/framebuffer.h"
#include "drawcall.h"

#define TEXTURE_2D 1
#define TEXTURE_2D_ARRAY 2
#define TEXTURE_CUBE 3

class Render {
private:
	void initEnvironment();
public: // Global render state
	bool enableCull;
	int cullMode;
	bool drawLine;
	bool enableDepthTest;
	int depthTestMode;
	bool enableAlphaTest;
	int alphaTestMode;
	float alphaThreshold;
	COLOR clearColor;
	Shader* currentShader;
public:
	int viewWidth,viewHeight;
	ShaderManager* shaders;
	std::map<uint, uint> textureInUse;
public:
	Render();
	~Render();
	void clearFrame(bool clearColor,bool clearDepth,bool clearStencil);
	void setState(const RenderState* state);
	void setDepthTest(bool enable,int testMode);
	void setAlphaTest(bool enable, int testMode, float threshold);
	void setCullState(bool enable);
	void setCullMode(int mode);
	void setDrawLine(bool line);
	void setClearColor(float r,float g,float b,float a);
	void setViewPort(int width,int height);
	void resize(int width,int height,Camera* mainCamera);
	Shader* findShader(const char* shader);
	void useShader(Shader* shader);
	void draw(Camera* camera, Drawcall* drawcall, const RenderState* state);
	void finishDraw();
	void setFrameBuffer(FrameBuffer* framebuffer);
	void useTexture(uint type, uint slot, uint texid);
};


#endif /* RENDER_H_ */

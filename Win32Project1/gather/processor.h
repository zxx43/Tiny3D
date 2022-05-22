#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "meshGather.h"
#include "objectGather.h"

struct LodParam {
	mat4 vpMat;
	uint depthTex;
	mat4 prevMat;
	vec2 size;
	vec2 camParam;
	float maxLevel;
	vec2 lodDist;
	vec3 eyePos;
};

struct RenderBuffer;
struct RenderState;
class MeshBuffer;
class Render;
class Camera;
class HizGenerator;
class Texture2D;
class Processor {
public:
	static const uint IndNormalIndex, IndSingleIndex, IndBillbdIndex, IndAnimatIndex;
	static const uint OutputNormal, OutputSingle, OutputBillbd, OutputAnimat;
	RenderBuffer* buffer;
public:
	int indNormalCount, indSingleCount, indBillbdCount, indAnimatCount, inputObjectCount;
public:
	const MeshGather* meshDB;
	const ObjectGather* objectDB;
	RenderBuffer* uniforms;
public:
	Processor(const MeshGather* meshs, const MeshBuffer* meshVBs, const ObjectGather* objects);
	~Processor();
public:
	void clear(Render* render);
	void lod(Render* render, const RenderState* state, const LodParam& param);
	void rearrange(Render* render);
	void gather(Render* render);
	void update();
	void showLog();
};

#endif

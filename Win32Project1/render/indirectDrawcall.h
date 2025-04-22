#ifndef INDIRECT_DRAWCALL_H_
#define INDIRECT_DRAWCALL_H_

#include "../gather/processor.h"
#include "drawcall.h"

#ifndef INDIRECT_NORMAL
#define INDIRECT_NORMAL 0
#define INDIRECT_SINGLE 1
#define INDIRECT_BILLBD 2
#define INDIRECT_ANIMAT 3
#define INDIRECT_TRANSP 4
#endif

class IndirectDrawcall : public Drawcall {
private:
	const Processor* processor;
	RenderBuffer* vbo;
	RenderBuffer* sbo;
	int indirectType;
public:
	IndirectDrawcall(const Processor* process, const MeshBuffer* meshDB, int type);
	virtual ~IndirectDrawcall();
public:
	virtual void draw(Render* render, RenderState* state, Shader* shader);
private:
	void doDraw(Render* render, RenderState* state, Shader* shader);
};

#endif
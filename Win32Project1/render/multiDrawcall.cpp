#include "multiDrawcall.h"
#include "../instance/multiInstance.h"
#include "../render/render.h"

// Attribute slots
const uint VertexSlot = 0;
const uint NormalSlot = 1;
const uint TexcoordSlot = 2;
const uint TexidSlot = 3;
const uint ColorSlot = 4;
const uint TangentSlot = 5;
const uint BoneidSlot = 6;
const uint WeightSlot = 7;
const uint PositionSlot = 8;

// VBO index
const uint VertexIndex = 0;
const uint NormalIndex = 1;
const uint TexcoordIndex = 2;
const uint TexidIndex = 3;
const uint ColorIndex = 4;
const uint TangentIndex = 5;
const uint BoneidIndex = 6;
const uint WeightIndex = 7;
const uint Index = 8;
const uint PositionIndex = 9;
const uint PositionOutIndex = 10;
const uint BaseIndex = 11;

// Indirect vbo index
const uint IndirectNormalIndex = 0;
const uint IndirectSingleIndex = 1;
const uint IndirectBillIndex = 2;
const uint IndirectAnimIndex = 3;

MultiDrawcall::MultiDrawcall(MultiInstance* multi) :Drawcall() {
	multiRef = multi;
	vertexCount = multiRef->vertexCount;
	indexCount = multiRef->indexCount;
	maxObjectCount = multiRef->maxInstance;

	dataBuffer = createBuffers(multiRef, vertexCount, indexCount, maxObjectCount);
	indirectBuffer = createIndirects(multiRef);
	bool dualBuffer = false;
	if (dualBuffer) {
		dataBuffer2 = createBuffers(multiRef, vertexCount, indexCount, maxObjectCount, dataBuffer);
		indirectBuffer2 = createIndirects(multiRef);
	} else {
		dataBuffer2 = NULL;
		indirectBuffer2 = NULL;
	}

	dataBufferPrepare = dataBuffer;
	indirectBufferPrepare = indirectBuffer;
	dataBufferDraw = dataBuffer2 ? dataBuffer2 : dataBuffer;
	indirectBufferDraw = indirectBuffer2 ? indirectBuffer2 : indirectBuffer;

	meshCount = multiRef->meshCount;
	if (!multiRef->hasAnim) setType(MULTI_DC);
	else setType(ANIMATE_DC);
	multiRef->releaseInstanceData();
}

MultiDrawcall::~MultiDrawcall() {
	if (indirectBuffer) delete indirectBuffer;
	if (dataBuffer2) delete dataBuffer2;
	if (indirectBuffer2) delete indirectBuffer2;
}

RenderBuffer* MultiDrawcall::createBuffers(MultiInstance* multi, int vertexCount, int indexCount, int maxObjects, RenderBuffer* ref) {
	RenderBuffer* buffer = new RenderBuffer(12);
	if (!ref) {
		buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex, VertexSlot, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, multi->vertexBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, NormalIndex, NormalSlot, GL_HALF_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, multi->normalBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexcoordIndex, TexcoordSlot, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, multi->texcoordBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexidIndex, TexidSlot, GL_FLOAT, vertexCount, 2, 1, false, GL_STATIC_DRAW, 0, multi->texidBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, ColorIndex, ColorSlot, GL_UNSIGNED_BYTE, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, multi->colorBuffer);
		buffer->setAttribData(GL_ARRAY_BUFFER, TangentIndex, TangentSlot, GL_HALF_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, 0, multi->tangentBuffer);
		buffer->setBufferData(GL_ELEMENT_ARRAY_BUFFER, Index, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, multi->indexBuffer);
		if (multi->hasAnim) {
			buffer->setAttribData(GL_ARRAY_BUFFER, BoneidIndex, BoneidSlot, GL_UNSIGNED_BYTE, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, multi->boneidBuffer);
			buffer->setAttribData(GL_ARRAY_BUFFER, WeightIndex, WeightSlot, GL_HALF_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, 0, multi->weightBuffer);
		}
	} else {
		buffer->setAttribData(GL_ARRAY_BUFFER, VertexIndex, ref->streamDatas[VertexIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, NormalIndex, ref->streamDatas[NormalIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexcoordIndex, ref->streamDatas[TexcoordIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, TexidIndex, ref->streamDatas[TexidIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, ColorIndex, ref->streamDatas[ColorIndex]);
		buffer->setAttribData(GL_ARRAY_BUFFER, TangentIndex, ref->streamDatas[TangentIndex]);
		buffer->setAttribData(GL_ELEMENT_ARRAY_BUFFER, Index, ref->streamDatas[Index]);
		if (multi->hasAnim) {
			buffer->setAttribData(GL_ARRAY_BUFFER, BoneidIndex, ref->streamDatas[BoneidIndex]);
			buffer->setAttribData(GL_ARRAY_BUFFER, WeightIndex, ref->streamDatas[WeightIndex]);
		}
	}

	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, BaseIndex, GL_UNSIGNED_INT, multi->meshCount, 4, GL_DYNAMIC_DRAW, NULL);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, PositionIndex, GL_FLOAT, maxObjects, 16, GL_DYNAMIC_DRAW, NULL);
	buffer->setAttribData(GL_SHADER_STORAGE_BUFFER, PositionOutIndex, PositionSlot, GL_FLOAT, maxObjects, 4, 4, false, GL_STREAM_DRAW, 1, NULL);
	buffer->useAs(PositionOutIndex, GL_ARRAY_BUFFER);
	buffer->setAttrib(PositionOutIndex);
	buffer->unuse();
	return buffer;
}

RenderBuffer* MultiDrawcall::createIndirects(MultiInstance* multi) {
	RenderBuffer* buffer = new RenderBuffer(4, false);
	if (multi->hasAnim) {
		buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectAnimIndex, GL_ONE, multi->animCount * sizeof(Indirect), GL_STREAM_DRAW, multi->indirectsAnim);
	} else {
		buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectNormalIndex, GL_ONE, multi->normalCount * sizeof(Indirect), GL_STREAM_DRAW, multi->indirectsNormal);
		buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectSingleIndex, GL_ONE, multi->singleCount * sizeof(Indirect), GL_STREAM_DRAW, multi->indirectsSingle);
		buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectBillIndex, GL_ONE, multi->billCount * sizeof(Indirect), GL_STREAM_DRAW, multi->indirectsBill);
	}
	return buffer;
}

void MultiDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	if (frame < state->delay) frame++;
	else {
		dataBufferDraw->use();

		bool shadowPass = state->pass < COLOR_PASS;
		if (!multiRef->hasAnim) {
			// Draw normal faces
			if (multiRef->normalCount > 0) {
				render->useShader(shader);
				if (shadowPass) render->setShaderFloat(shader, "uAlpha", 0.0);
				indirectBufferDraw->useAs(IndirectNormalIndex, GL_DRAW_INDIRECT_BUFFER);
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->normalCount, 0);
			}

			// Draw single faces
			if (multiRef->singleCount > 0) {
				if (shadowPass) render->setCullMode(CULL_BACK);
				else render->setCullState(false);
				render->useShader(shader);
				if (shadowPass) render->setShaderFloat(shader, "uAlpha", 1.0);
				indirectBufferDraw->useAs(IndirectSingleIndex, GL_DRAW_INDIRECT_BUFFER);
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->singleCount, 0);
			}

			// Draw billboard faces
			if (multiRef->billCount > 0) {
				render->useShader(state->shaderBill);
				if (shadowPass) {
					render->setCullState(false);
					render->setShaderFloat(state->shaderBill, "uAlpha", 1.0);
				}
				indirectBufferDraw->useAs(IndirectBillIndex, GL_DRAW_INDIRECT_BUFFER);
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->billCount, 0);
			}
		} else {
			render->useShader(shader);
			render->setShaderFloat(shader, "uAlpha", 0.0);
			indirectBufferDraw->useAs(IndirectAnimIndex, GL_DRAW_INDIRECT_BUFFER);
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->animCount, 0);
		}
	}

	if (dataBuffer2) swapBuffers();
}

void MultiDrawcall::swapBuffers() {
	indirectBufferPrepare = indirectBufferPrepare == indirectBuffer ? indirectBuffer2 : indirectBuffer;
	dataBufferPrepare = dataBufferPrepare == dataBuffer ? dataBuffer2 : dataBuffer;
	indirectBufferDraw = indirectBufferDraw == indirectBuffer ? indirectBuffer2 : indirectBuffer;
	dataBufferDraw = dataBufferDraw == dataBuffer ? dataBuffer2 : dataBuffer;
}

void MultiDrawcall::update(Render* render, RenderState* state) {
	objectCount = multiRef->updateTransform();
	dataBufferPrepare->updateBufferData(BaseIndex, meshCount, (void*)(multiRef->bases));
	dataBufferPrepare->updateBufferData(PositionIndex, objectCount, (void*)(multiRef->transforms));
	updateIndirect(render, state);
	prepareRenderData(render, state);
}

void MultiDrawcall::updateIndirect(Render* render, RenderState* state) {
	if (multiRef->hasAnim) 
		indirectBufferPrepare->setShaderBase(IndirectAnimIndex, 4);
	else {
		indirectBufferPrepare->setShaderBase(IndirectNormalIndex, 1);
		indirectBufferPrepare->setShaderBase(IndirectSingleIndex, 2);
		indirectBufferPrepare->setShaderBase(IndirectBillIndex, 3);
	}
	dataBufferPrepare->setShaderBase(BaseIndex, 5);

	render->useShader(state->shaderFlush);
	render->setShaderUVec4(state->shaderFlush, "uCount", multiRef->normalCount, multiRef->singleCount, multiRef->billCount, multiRef->animCount);
	glDispatchCompute(meshCount, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

	if (multiRef->hasAnim)
		indirectBufferPrepare->unbindShaderBase(IndirectAnimIndex, 4);
	else 
		UnbindShaderBuffers(1, 3);
	dataBufferPrepare->unbindShaderBase(BaseIndex, 5);
}

void MultiDrawcall::prepareRenderData(Render* render, RenderState* state) {
	dataBufferPrepare->use();
	dataBufferPrepare->setShaderBase(PositionIndex, 1);
	dataBufferPrepare->setShaderBase(PositionOutIndex, 2);
	if (multiRef->hasAnim)
		indirectBufferPrepare->setShaderBase(IndirectAnimIndex, 7);
	else {
		indirectBufferPrepare->setShaderBase(IndirectNormalIndex, 4);
		indirectBufferPrepare->setShaderBase(IndirectSingleIndex, 5);
		indirectBufferPrepare->setShaderBase(IndirectBillIndex, 6);
	}

	render->useShader(state->shaderMulti);
	render->setShaderUint(state->shaderMulti, "bufferPass", multiRef->bufferPass);
	render->setShaderIVec4(state->shaderMulti, "uCount", multiRef->normalCount, multiRef->singleCount, multiRef->billCount, multiRef->animCount);

	int dispatch = objectCount > MAX_DISPATCH ? MAX_DISPATCH : objectCount;
	render->setShaderUint(state->shaderMulti, "pass", 0);
	glDispatchCompute(dispatch, 1, 1);
	if (objectCount > MAX_DISPATCH) {
		dispatch = objectCount - MAX_DISPATCH;
		render->setShaderUint(state->shaderMulti, "pass", 1);
		glDispatchCompute(dispatch, 1, 1);
	}
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

	UnbindShaderBuffers(1, 2);
	if (multiRef->hasAnim)
		indirectBufferPrepare->unbindShaderBase(IndirectAnimIndex, 7);
	else 
		UnbindShaderBuffers(4, 3);
}


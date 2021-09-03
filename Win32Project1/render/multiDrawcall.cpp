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
const uint OutSlot = 8;

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
const uint InIndex = 9;
const uint OutIndex = 10;
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

	dataBuffer = createBuffers(multiRef, vertexCount, indexCount, InIndex, OutIndex, multiRef->maxNormalInstance);
	singleBuffer = createBuffers(multiRef, vertexCount, indexCount, InIndex, OutIndex, multiRef->maxSingleInstance, dataBuffer);
	billBuffer = createBuffers(multiRef, vertexCount, indexCount, InIndex, OutIndex, multiRef->maxBillInstance, dataBuffer);
	animBuffer = createBuffers(multiRef, vertexCount, indexCount, InIndex, OutIndex, multiRef->maxAnimInstance, dataBuffer);

	indirectBuffer = createIndirects(multiRef);

	meshCount = multiRef->meshCount;
	if (!multiRef->hasAnim) setType(MULTI_DC);
	else setType(ANIMATE_DC);
	multiRef->releaseInstanceData();
}

MultiDrawcall::~MultiDrawcall() {
	if (indirectBuffer) delete indirectBuffer;
	if (singleBuffer) delete singleBuffer;
	if (billBuffer) delete billBuffer;
	if (animBuffer) delete animBuffer;
}

RenderBuffer* MultiDrawcall::createBuffers(MultiInstance* multi, int vertexCount, int indexCount, uint inIndex, uint outIndex, uint maxCount, RenderBuffer* ref) {
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
		buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, BaseIndex, GL_UNSIGNED_INT, multi->meshCount, 4, GL_DYNAMIC_DRAW, NULL);
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
		buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, BaseIndex, ref->streamDatas[BaseIndex]);
	}
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, inIndex, GL_FLOAT, maxCount, 16, GL_DYNAMIC_DRAW, NULL);
	buffer->setAttribData(GL_SHADER_STORAGE_BUFFER, outIndex, OutSlot, GL_FLOAT, maxCount, 4, 4, false, GL_STREAM_DRAW, 1, NULL);
	buffer->useAs(outIndex, GL_ARRAY_BUFFER);
	buffer->setAttrib(outIndex);

	buffer->unuse();
	return buffer;
}

RenderBuffer* MultiDrawcall::createIndirects(MultiInstance* multi) {
	RenderBuffer* buffer = new RenderBuffer(4, false);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectNormalIndex, GL_ONE, multi->normalCount * sizeof(Indirect), GL_STREAM_DRAW, multi->indirectsNormal);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectSingleIndex, GL_ONE, multi->singleCount * sizeof(Indirect), GL_STREAM_DRAW, multi->indirectsSingle);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectBillIndex, GL_ONE, multi->billCount * sizeof(Indirect), GL_STREAM_DRAW, multi->indirectsBill);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, IndirectAnimIndex, GL_ONE, multi->animCount * sizeof(Indirect), GL_STREAM_DRAW, multi->indirectsAnim);
	return buffer;
}

void MultiDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	if (frame < state->delay) frame++;
	else {
		bool shadowPass = state->pass < COLOR_PASS;
		if (!multiRef->hasAnim) {
			// Draw normal faces
			if (multiRef->normalCount > 0 && multiRef->normalInsCount > 0) {
				dataBuffer->use();
				render->useShader(shader);
				if (shadowPass) render->setShaderFloat(shader, "uAlpha", 0.0);
				indirectBuffer->useAs(IndirectNormalIndex, GL_DRAW_INDIRECT_BUFFER);
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->normalCount, 0);
			}

			// Draw single faces
			if (multiRef->singleCount > 0 && multiRef->singleInsCount > 0) {
				singleBuffer->use();
				if (shadowPass) render->setCullMode(CULL_BACK);
				else render->setCullState(false);
				render->useShader(shader);
				if (shadowPass) render->setShaderFloat(shader, "uAlpha", 1.0);
				indirectBuffer->useAs(IndirectSingleIndex, GL_DRAW_INDIRECT_BUFFER);
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->singleCount, 0);
			}

			// Draw billboard faces
			if (multiRef->billCount > 0 && multiRef->billInsCount > 0) {
				billBuffer->use();
				render->useShader(state->shaderBill);
				if (shadowPass) {
					render->setCullState(false);
					render->setShaderFloat(state->shaderBill, "uAlpha", 1.0);
				}
				indirectBuffer->useAs(IndirectBillIndex, GL_DRAW_INDIRECT_BUFFER);
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->billCount, 0);
			}
		} else {
			if (multiRef->animCount > 0 && multiRef->animInsCount > 0) {
				animBuffer->use();
				render->useShader(shader);
				render->setShaderFloat(shader, "uAlpha", 0.0);
				indirectBuffer->useAs(IndirectAnimIndex, GL_DRAW_INDIRECT_BUFFER);
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, multiRef->animCount, 0);
			}
		}
	}
}

void MultiDrawcall::update(Camera* camera, Render* render, RenderState* state) {
	objectCount = multiRef->updateTransform();
	dataBuffer->updateBufferData(BaseIndex, meshCount, (void*)(multiRef->bases));
	if (multiRef->normalInsCount > 0)
		dataBuffer->updateBufferData(InIndex, multiRef->normalInsCount, (void*)(multiRef->transformsNormal));
	if (multiRef->singleInsCount > 0)
		singleBuffer->updateBufferData(InIndex, multiRef->singleInsCount, (void*)(multiRef->transformsSingle));
	if (multiRef->billInsCount > 0)
		billBuffer->updateBufferData(InIndex, multiRef->billInsCount, (void*)(multiRef->transformsBill));
	if (multiRef->animInsCount > 0) 
		animBuffer->updateBufferData(InIndex, multiRef->animInsCount, (void*)(multiRef->transformsAnim));

	updateIndirect(render, state);
	prepareRenderData(camera, render, state);
}

void MultiDrawcall::updateIndirect(Render* render, RenderState* state) {
	indirectBuffer->setShaderBase(IndirectNormalIndex, 1);
	indirectBuffer->setShaderBase(IndirectSingleIndex, 2);
	indirectBuffer->setShaderBase(IndirectBillIndex, 3);
	indirectBuffer->setShaderBase(IndirectAnimIndex, 4);
	dataBuffer->setShaderBase(BaseIndex, 5);

	render->useShader(state->shaderFlush);
	render->setShaderUVec4(state->shaderFlush, "uCount", multiRef->normalCount, multiRef->singleCount, multiRef->billCount, multiRef->animCount);
	glDispatchCompute(meshCount, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void MultiDrawcall::prepareRenderData(Camera* camera, Render* render, RenderState* state) {
	dataBuffer->setShaderBase(InIndex, 1);
	singleBuffer->setShaderBase(InIndex, 2);
	billBuffer->setShaderBase(InIndex, 3);
	animBuffer->setShaderBase(InIndex, 4);

	dataBuffer->setShaderBase(OutIndex, 5);
	singleBuffer->setShaderBase(OutIndex, 6);
	billBuffer->setShaderBase(OutIndex, 7);
	animBuffer->setShaderBase(OutIndex, 8);

	indirectBuffer->setShaderBase(IndirectNormalIndex, 9);
	indirectBuffer->setShaderBase(IndirectSingleIndex, 10);
	indirectBuffer->setShaderBase(IndirectBillIndex, 11);
	indirectBuffer->setShaderBase(IndirectAnimIndex, 12);

	render->useShader(state->shaderMulti);
	render->setShaderUint(state->shaderMulti, "bufferPass", multiRef->bufferPass);
	render->setShaderIVec4(state->shaderMulti, "uCount", multiRef->normalCount, multiRef->singleCount, multiRef->billCount, multiRef->animCount);
	render->setShaderUVec4(state->shaderMulti, "uInsCount", multiRef->normalInsCount, multiRef->singleInsCount, multiRef->billInsCount, multiRef->animInsCount);
	render->setShaderMat4(state->shaderMulti, "viewProjectMatrix", camera->viewProjectMatrix);

	int dispatch = objectCount > MAX_DISPATCH ? MAX_DISPATCH : objectCount;
	render->setShaderUint(state->shaderMulti, "pass", 0);
	glDispatchCompute(dispatch, 1, 1);
	if (objectCount > MAX_DISPATCH) {
		dispatch = objectCount - MAX_DISPATCH;
		render->setShaderUint(state->shaderMulti, "pass", 1);
		glDispatchCompute(dispatch, 1, 1);
	}
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	/*
	if (!multiRef->hasAnim && state->pass == COLOR_PASS) {
		Indirect* buf = new Indirect[multiRef->normalCount];
		indirectBuffer->readBufferData(GL_DRAW_INDIRECT_BUFFER, IndirectNormalIndex, multiRef->normalCount * sizeof(Indirect), buf);
		int objCulled = 0;
		for (int i = 0; i < multiRef->normalCount; ++i) 
			objCulled += multiRef->getNormalInstance(i)->insData->count - buf[i].primCount;
		if (objCulled > 0) printf("object culled %d\n", objCulled);
		delete[] buf;

		buf = new Indirect[multiRef->singleCount];
		indirectBuffer->readBufferData(GL_DRAW_INDIRECT_BUFFER, IndirectSingleIndex, multiRef->singleCount * sizeof(Indirect), buf);
		int treeCulled = 0;
		for (int i = 0; i < multiRef->singleCount; ++i)
			treeCulled += multiRef->getSingleInstance(i)->insData->count - buf[i].primCount;
		if (treeCulled > 0) printf("tree culled %d\n", treeCulled);
		delete[] buf;

		buf = new Indirect[multiRef->billCount];
		indirectBuffer->readBufferData(GL_DRAW_INDIRECT_BUFFER, IndirectBillIndex, multiRef->billCount * sizeof(Indirect), buf);
		int billCulled = 0;
		for (int i = 0; i < multiRef->billCount; ++i)
			billCulled += multiRef->getBillInstance(i)->insData->count - buf[i].primCount;
		if (billCulled > 0) printf("billboard culled %d\n", billCulled);
		delete[] buf;
		if (objCulled > 0 || treeCulled > 0 || billCulled > 0) printf("\n");
	}
	//*/
}


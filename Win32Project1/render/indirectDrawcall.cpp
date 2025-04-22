#include "indirectDrawcall.h"
#include "render.h"
#include "../gather/meshBuffer.h"
#include "../assets/assetManager.h"

IndirectDrawcall::IndirectDrawcall(const Processor* process, const MeshBuffer* meshDB, int type) :Drawcall() {
	indirectType = type;

	uint bufferCount = 6, inputIndex = Processor::OutputNormal;
	switch (indirectType) {
		case INDIRECT_SINGLE:
			inputIndex = Processor::OutputSingle;
			vbo = meshDB->meshVBs;
			bufferCount = 6;
			break;
		case INDIRECT_BILLBD:
			inputIndex = Processor::OutputBillbd;
			vbo = meshDB->meshVBs;
			bufferCount = 6;
			break;
		case INDIRECT_ANIMAT:
			inputIndex = Processor::OutputAnimat;
			vbo = meshDB->animVBs;
			bufferCount = 8;
			break;
		case INDIRECT_TRANSP:
			inputIndex = Processor::OutputTransp;
			vbo = meshDB->meshVBs;
			bufferCount = 6;
			break;
		default:
			inputIndex = Processor::OutputNormal;
			vbo = meshDB->meshVBs;
			bufferCount = 6;
			break;
	}
	processor = process;
	sbo = processor->buffer;
	
	if (vbo) {
		dataBuffer = new RenderBuffer(bufferCount);
		dataBuffer->setAttribData(GL_ARRAY_BUFFER, 0, vbo->streamDatas[MeshBuffer::VertexIndex]);
		dataBuffer->setAttribData(GL_ARRAY_BUFFER, 1, vbo->streamDatas[MeshBuffer::NormalIndex]);
		dataBuffer->setAttribData(GL_ARRAY_BUFFER, 2, vbo->streamDatas[MeshBuffer::TangentIndex]);
		dataBuffer->setAttribData(GL_ARRAY_BUFFER, 3, vbo->streamDatas[MeshBuffer::TexcoordIndex]);
		dataBuffer->setBufferData(GL_ELEMENT_ARRAY_BUFFER, 4, vbo->streamDatas[MeshBuffer::EboIndex]);
		dataBuffer->setAttribData(GL_ARRAY_BUFFER, 5, sbo->streamDatas[inputIndex]);
		if (type == INDIRECT_ANIMAT) {
			dataBuffer->setAttribData(GL_ARRAY_BUFFER, 6, vbo->streamDatas[MeshBuffer::BoneIndex]);
			dataBuffer->setAttribData(GL_ARRAY_BUFFER, 7, vbo->streamDatas[MeshBuffer::WeightIndex]);
		}
		dataBuffer->unuse();
	}

	setType(type == INDIRECT_ANIMAT ? ANIMATE_DC : MULTI_DC);
}

IndirectDrawcall::~IndirectDrawcall() {

}

void IndirectDrawcall::draw(Render* render, RenderState* state, Shader* shader) {
	if (!dataBuffer) return;
	//if (frame < state->delay) {
	//	frame++;
	//	GLenum error = glGetError();
	//	if (error != GL_NO_ERROR)
	//		printf("gl error! %d\n", error);
	//} 
	else doDraw(render, state, shader);
}

void IndirectDrawcall::doDraw(Render* render, RenderState* state, Shader* shader) {
	uint indirectCount, indirectIndex;
	Shader* shaderToUse = shader;
	switch (indirectType) {
	case INDIRECT_SINGLE:
		indirectCount = processor->indSingleCount;
		indirectIndex = Processor::IndSingleIndex;
		shaderToUse = state->shaderIns;
		break;
	case INDIRECT_BILLBD:
		indirectCount = processor->indBillbdCount;
		indirectIndex = Processor::IndBillbdIndex;
		shaderToUse = state->shaderBill;
		break;
	case INDIRECT_ANIMAT:
		indirectCount = processor->indAnimatCount;
		indirectIndex = Processor::IndAnimatIndex;
		shaderToUse = state->shaderBone;
		break;
	case INDIRECT_TRANSP:
		indirectCount = processor->indTranspCount;
		indirectIndex = Processor::IndTranspIndex;
		shaderToUse = state->shaderTrans;
		break;
	default:
		indirectCount = processor->indNormalCount;
		indirectIndex = Processor::IndNormalIndex;
		shaderToUse = state->shaderIns;
		break;
	}

	MaterialManager::materials->useMaterialBuffer(1);
	dataBuffer->use();
	render->useShader(shaderToUse);

	if (indirectType == INDIRECT_NORMAL || indirectType == INDIRECT_ANIMAT) {
		render->setCullState(true);
		if (!state->isShadowPass()) render->setCullMode(CULL_BACK);
		else {
			render->setCullMode(CULL_FRONT);
			render->setShaderFloat(shaderToUse, "uAlpha", 0.0);
		}
		if (indirectType == INDIRECT_ANIMAT && !shaderToUse->isTexBinded(AssetManager::assetManager->frames->datas, AssetManager::assetManager->frames->animCount)) {
			shaderToUse->setHandle64v("boneTex", AssetManager::assetManager->frames->animCount, AssetManager::assetManager->frames->datas);
		}
	} else if (indirectType == INDIRECT_SINGLE) {
		if (!state->isShadowPass()) render->setCullState(false);
		else {
			render->setCullMode(CULL_BACK);
			render->setCullState(true);
			render->setShaderFloat(shaderToUse, "uAlpha", 1.0);
		}
	} else if (indirectType == INDIRECT_TRANSP) {
		render->setCullState(false);
	} else if (indirectType == INDIRECT_BILLBD) {
		if (!state->isShadowPass()) {
			render->setCullMode(CULL_BACK);
			render->setCullState(true);
		}
		else {
			render->setCullState(false);
			render->setShaderFloat(shaderToUse, "uAlpha", 1.0);
		}
	}

	if (state->debug) {
		render->setCullState(false);
		render->setDrawLine(true);
	}

	sbo->useAs(indirectIndex, GL_DRAW_INDIRECT_BUFFER);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, 0, indirectCount, 0);
}
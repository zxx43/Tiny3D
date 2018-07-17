#include "instanceDrawcall.h"
#include "../instance/instance.h"

InstanceDrawcall::InstanceDrawcall(Instance* instance) :Drawcall() {
	vertexCount = instance->vertexCount;
	indexCount = instance->indexCount;
	indexed = indexCount > 0 ? true : false;
	objectCount = MAX_INSTANCE_COUNT;
	objectToDraw = 0;
	setBillboard(instance->isBillboard);

	bufferCount = isBillboard() ? 1 : bufferCount;
	dataBuffers = new RenderBuffer*[bufferCount];
	if (isBillboard()) {
		dataBuffers[0] = new RenderBuffer(indexed ? 5 : 4);
		dataBuffers[0]->pushData(0, new RenderData(0, GL_FLOAT, vertexCount, 3, 1,
			dataBuffers[0]->vbos[0], false, GL_STATIC_DRAW, 0, instance->vertexBuffer));
		dataBuffers[0]->pushData(1, new RenderData(1, GL_FLOAT, vertexCount, instance->textureChannel, 1,
			dataBuffers[0]->vbos[1], false, GL_STATIC_DRAW, 0, instance->texcoordBuffer));
		dataBuffers[0]->pushData(2, new RenderData(2, GL_FLOAT, objectCount, 3, 1,
			dataBuffers[0]->vbos[2], false, GL_DYNAMIC_DRAW, 1, instance->positions));
		dataBuffers[0]->pushData(3, new RenderData(3, GL_FLOAT, objectCount, 2, 1,
			dataBuffers[0]->vbos[3], false, GL_DYNAMIC_DRAW, 1, instance->billboards));
		if (indexed)
			dataBuffers[0]->pushData(4, new RenderData(GL_UNSIGNED_SHORT, indexCount,
				dataBuffers[0]->vbos[4], GL_STATIC_DRAW, instance->indexBuffer));
		dataBuffers[0]->unuse();
	} else {
		dataBuffers[0] = new RenderBuffer(indexed ? 3 : 2);
		dataBuffers[0]->pushData(0, new RenderData(0, GL_FLOAT, vertexCount, 3, 1,
			dataBuffers[0]->vbos[0], false, GL_STATIC_DRAW, 0, instance->vertexBuffer));
		dataBuffers[0]->pushData(1, new RenderData(1, GL_FLOAT, objectCount, 4, 3,
			dataBuffers[0]->vbos[1], false, GL_DYNAMIC_DRAW, 1, instance->modelMatrices));
		if (indexed)
			dataBuffers[0]->pushData(2, new RenderData(GL_UNSIGNED_SHORT, indexCount,
			dataBuffers[0]->vbos[2], GL_STATIC_DRAW, instance->indexBuffer));
		dataBuffers[0]->unuse();

		dataBuffers[1] = new RenderBuffer(indexed ? 4 : 3);
		dataBuffers[1]->pushData(0, new RenderData(0, GL_FLOAT, vertexCount, 3, 1,
			dataBuffers[1]->vbos[0], false, GL_STATIC_DRAW, 0, instance->vertexBuffer));
		dataBuffers[1]->pushData(1, new RenderData(1, GL_FLOAT, vertexCount, instance->textureChannel, 1,
			dataBuffers[1]->vbos[1], false, GL_STATIC_DRAW, 0, instance->texcoordBuffer));
		dataBuffers[1]->pushData(2, new RenderData(2, GL_FLOAT, objectCount, 4, 3,
			dataBuffers[1]->vbos[2], false, GL_DYNAMIC_DRAW, 1, instance->modelMatrices));
		if (indexed)
			dataBuffers[1]->pushData(3, new RenderData(GL_UNSIGNED_SHORT, indexCount,
			dataBuffers[1]->vbos[3], GL_STATIC_DRAW, instance->indexBuffer));
		dataBuffers[1]->unuse();

		dataBuffers[2] = new RenderBuffer(indexed ? 6 : 5);
		dataBuffers[2]->pushData(0, new RenderData(VERTEX_LOCATION, GL_FLOAT, vertexCount, 3, 1,
			dataBuffers[2]->vbos[0], false, GL_STATIC_DRAW, 0, instance->vertexBuffer));
		dataBuffers[2]->pushData(1, new RenderData(NORMAL_LOCATION, GL_FLOAT, vertexCount, 3, 1,
			dataBuffers[2]->vbos[1], false, GL_STATIC_DRAW, 0, instance->normalBuffer));
		dataBuffers[2]->pushData(2, new RenderData(TEXCOORD_LOCATION, GL_FLOAT, vertexCount, instance->textureChannel, 1,
			dataBuffers[2]->vbos[2], false, GL_STATIC_DRAW, 0, instance->texcoordBuffer));
		dataBuffers[2]->pushData(3, new RenderData(COLOR_LOCATION, GL_UNSIGNED_BYTE, vertexCount, 3, 1,
			dataBuffers[2]->vbos[3], false, GL_STATIC_DRAW, 0, instance->colorBuffer));
		dataBuffers[2]->pushData(4, new RenderData(MODEL_MATRIX_LOCATION, GL_FLOAT, objectCount, 4, 3,
			dataBuffers[2]->vbos[4], false, GL_DYNAMIC_DRAW, 1, instance->modelMatrices));
		if (indexed)
			dataBuffers[2]->pushData(5, new RenderData(GL_UNSIGNED_SHORT, indexCount,
			dataBuffers[2]->vbos[5], GL_STATIC_DRAW, instance->indexBuffer));
		dataBuffers[2]->unuse();
	}

	setType(INSTANCE_DC);
}

InstanceDrawcall::~InstanceDrawcall() {
	for (int i = 0; i < bufferCount; i++)
		delete dataBuffers[i];
	delete[] dataBuffers;
}

void InstanceDrawcall::draw(Shader* shader,int pass) {
	if (!isBillboard()) {
		int bufId = 0;
		switch (pass) {
			case 1:
			case 2:
				bufId = 1;
				break;
			case 3:
				bufId = 0;
				break;
			case 4:
				bufId = 2;
				break;
		}
		dataBuffers[bufId]->use();
	} else
		dataBuffers[0]->use();
	if(!indexed)
		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, objectToDraw);
	else
		glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0, objectToDraw);
}

void InstanceDrawcall::updateInstances(Instance* instance, int pass) {
	if (!isBillboard()) {
		int bufId = 0, stream = 2;
		switch (pass) {
			case 1:
			case 2:
				bufId = 1;
				stream = 2;
				break;
			case 3:
				bufId = 0;
				stream = 1;
				break;
			case 4:
				bufId = 2;
				stream = 4;
				break;
		}
		dataBuffers[bufId]->streamDatas[stream]->updateAttrBuf(objectToDraw, (void*)(instance->modelMatrices), GL_DYNAMIC_DRAW);
		dataBuffers[bufId]->unuseAttr();
	} else {
		dataBuffers[0]->streamDatas[2]->updateAttrBuf(objectToDraw, (void*)(instance->positions), GL_DYNAMIC_DRAW);
		dataBuffers[0]->streamDatas[3]->updateAttrBuf(objectToDraw, (void*)(instance->billboards), GL_DYNAMIC_DRAW);
		dataBuffers[0]->unuseAttr();
	}
}

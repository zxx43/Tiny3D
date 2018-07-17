#include "animationDrawcall.h"

AnimationDrawcall::AnimationDrawcall() {}

AnimationDrawcall::AnimationDrawcall(Animation* anim) :Drawcall() {
	animData = new AnimationData(anim);
	vertexCount = animData->vertexCount;
	indexCount = animData->indexCount;
	objectCount = 1;

	bufferCount = 2;
	dataBuffers = new RenderBuffer*[bufferCount];

	dataBuffers[0] = new RenderBuffer(4);
	dataBuffers[0]->pushData(0, new RenderData(0, GL_FLOAT, vertexCount, 3, 1,
		dataBuffers[0]->vbos[0], false, GL_STATIC_DRAW, -1, animData->vertices));
	dataBuffers[0]->pushData(1, new RenderData(1, GL_UNSIGNED_BYTE, vertexCount, 4, 1,
		dataBuffers[0]->vbos[1], false, GL_STATIC_DRAW, -1, animData->boneids));
	dataBuffers[0]->pushData(2, new RenderData(2, GL_FLOAT, vertexCount, 4, 1,
		dataBuffers[0]->vbos[2], false, GL_STATIC_DRAW, -1, animData->weights));
	dataBuffers[0]->pushData(3, new RenderData(GL_UNSIGNED_SHORT, indexCount,
		dataBuffers[0]->vbos[3], GL_STATIC_DRAW, animData->indices));
	dataBuffers[0]->unuse();

	dataBuffers[1] = new RenderBuffer(7);
	dataBuffers[1]->pushData(0, new RenderData(VERTEX_LOCATION, GL_FLOAT, vertexCount, 3, 1,
		dataBuffers[1]->vbos[0], false, GL_STATIC_DRAW, -1, animData->vertices));
	dataBuffers[1]->pushData(1, new RenderData(NORMAL_LOCATION, GL_FLOAT, vertexCount, 3, 1,
		dataBuffers[1]->vbos[1], false, GL_STATIC_DRAW, -1, animData->normals));
	dataBuffers[1]->pushData(2, new RenderData(TEXCOORD_LOCATION, GL_FLOAT, vertexCount, animData->textureChannel, 1,
		dataBuffers[1]->vbos[2], false, GL_STATIC_DRAW, -1, animData->texcoords));
	dataBuffers[1]->pushData(3, new RenderData(COLOR_LOCATION, GL_UNSIGNED_BYTE, vertexCount, 3, 1,
		dataBuffers[1]->vbos[3], false, GL_STATIC_DRAW, -1, animData->colors));
	dataBuffers[1]->pushData(4, new RenderData(BONEIDS_LOCATION, GL_UNSIGNED_BYTE, vertexCount, 4, 1,
		dataBuffers[1]->vbos[4], false, GL_STATIC_DRAW, -1, animData->boneids));
	dataBuffers[1]->pushData(5, new RenderData(WEIGHTS_LOCATION, GL_FLOAT, vertexCount, 4, 1,
		dataBuffers[1]->vbos[5], false, GL_STATIC_DRAW, -1, animData->weights));
	dataBuffers[1]->pushData(6, new RenderData(GL_UNSIGNED_SHORT, indexCount,
		dataBuffers[1]->vbos[6], GL_STATIC_DRAW, animData->indices));
	dataBuffers[1]->unuse();

	setType(ANIMATE_DC);
}

AnimationDrawcall::~AnimationDrawcall() {
	for (int i = 0; i < bufferCount; i++)
		delete dataBuffers[i];
	delete[] dataBuffers;
	delete animData;
}

void AnimationDrawcall::draw(Shader* shader,int pass) {
	if (uModelMatrix)
		shader->setMatrix4("uModelMatrix", uModelMatrix);
	if (uNormalMatrix && pass == 4) 
		shader->setMatrix3("uNormalMatrix", uNormalMatrix);
	if (animData->animation->boneTransformMats)
		shader->setMatrix3x4("boneMats", animData->boneCount, animData->animation->boneTransformMats);
	switch (pass) {
		case 1:
		case 2:
		case 3:
			dataBuffers[0]->use();
			break;
		case 4:
			dataBuffers[1]->use();
			break;
	}
	glDrawElements(GL_TRIANGLES, animData->indexCount, GL_UNSIGNED_SHORT, 0);
}

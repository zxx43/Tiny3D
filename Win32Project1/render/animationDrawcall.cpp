#include "animationDrawcall.h"

AnimationDrawcall::AnimationDrawcall() {}

AnimationDrawcall::AnimationDrawcall(Animation* anim) :Drawcall() {
	animData = new AnimationData(anim);
	vertexCount = animData->vertexCount;
	indexCount = animData->indexCount;
	objectCount = 1;

	dataBuffer = new RenderBuffer(7);
	dataBuffer->pushData(0, new RenderData(VERTEX_LOCATION, GL_FLOAT, vertexCount, 3, 1,
		dataBuffer->vbos[0], false, GL_STATIC_DRAW, -1, animData->vertices));
	dataBuffer->pushData(1, new RenderData(NORMAL_LOCATION, GL_FLOAT, vertexCount, 3, 1,
		dataBuffer->vbos[1], false, GL_STATIC_DRAW, -1, animData->normals));
	dataBuffer->pushData(2, new RenderData(TEXCOORD_LOCATION, GL_FLOAT, vertexCount, animData->textureChannel, 1,
		dataBuffer->vbos[2], false, GL_STATIC_DRAW, -1, animData->texcoords));
	dataBuffer->pushData(3, new RenderData(COLOR_LOCATION, GL_UNSIGNED_BYTE, vertexCount, 3, 1,
		dataBuffer->vbos[3], false, GL_STATIC_DRAW, -1, animData->colors));
	dataBuffer->pushData(4, new RenderData(BONEIDS_LOCATION, GL_UNSIGNED_BYTE, vertexCount, 4, 1,
		dataBuffer->vbos[4], false, GL_STATIC_DRAW, -1, animData->boneids));
	dataBuffer->pushData(5, new RenderData(WEIGHTS_LOCATION, GL_FLOAT, vertexCount, 4, 1,
		dataBuffer->vbos[5], false, GL_STATIC_DRAW, -1, animData->weights));
	dataBuffer->pushData(6, new RenderData(GL_UNSIGNED_SHORT, indexCount,
		dataBuffer->vbos[6], GL_STATIC_DRAW, animData->indices));

	createSimple();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setType(ANIMATE_DC);
}

AnimationDrawcall::~AnimationDrawcall() {
	releaseSimple();
	delete dataBuffer;
	delete animData;
}

void AnimationDrawcall::createSimple() {
	simpleBuffer = new RenderBuffer(4);
	simpleBuffer->pushData(0, new RenderData(0, GL_FLOAT, vertexCount, 3, 1,
		simpleBuffer->vbos[0], false, GL_STATIC_DRAW, -1, animData->vertices));
	simpleBuffer->pushData(1, new RenderData(1, GL_UNSIGNED_BYTE, vertexCount, 4, 1,
		simpleBuffer->vbos[1], false, GL_STATIC_DRAW, -1, animData->boneids));
	simpleBuffer->pushData(2, new RenderData(2, GL_FLOAT, vertexCount, 4, 1,
		simpleBuffer->vbos[2], false, GL_STATIC_DRAW, -1, animData->weights));
	simpleBuffer->pushData(3, new RenderData(GL_UNSIGNED_SHORT, indexCount,
		simpleBuffer->vbos[3], GL_STATIC_DRAW, animData->indices));
}

void AnimationDrawcall::releaseSimple() {
	delete simpleBuffer;
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
			simpleBuffer->use();
			break;
		case 4:
			dataBuffer->use();
			break;
	}
	glDrawElements(GL_TRIANGLES, animData->indexCount, GL_UNSIGNED_SHORT, 0);
}

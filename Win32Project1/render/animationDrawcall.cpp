#include "animationDrawcall.h"

AnimationDrawcall::AnimationDrawcall() {}

AnimationDrawcall::AnimationDrawcall(Animation* anim) :Drawcall() {
	animData = new AnimationData(anim);
	vertexCount = animData->vertexCount;
	indexCount = animData->indexCount;
	objectCount = 1;

	dataBuffer = new RenderBuffer(7);
	dataBuffer->setAttribData(0, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, -1, animData->vertices);
	dataBuffer->setAttribData(1, GL_FLOAT, vertexCount, 3, 1, false, GL_STATIC_DRAW, -1, animData->normals);
	dataBuffer->setAttribData(2, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, -1, animData->texcoords);
	dataBuffer->setAttribData(3, GL_UNSIGNED_BYTE, vertexCount, 3, 1, false, GL_STATIC_DRAW, -1, animData->colors);
	dataBuffer->setAttribData(4, GL_UNSIGNED_BYTE, vertexCount, 4, 1, false, GL_STATIC_DRAW, -1, animData->boneids);
	dataBuffer->setAttribData(5, GL_FLOAT, vertexCount, 4, 1, false, GL_STATIC_DRAW, -1, animData->weights);
	dataBuffer->setIndexData(6, GL_UNSIGNED_SHORT, indexCount, GL_STATIC_DRAW, animData->indices);
	dataBuffer->unuse();

	setType(ANIMATE_DC);
}

AnimationDrawcall::~AnimationDrawcall() {
	delete animData;
}

void AnimationDrawcall::draw(Shader* shader,int pass) {
	if (uModelMatrix)
		shader->setMatrix4("uModelMatrix", uModelMatrix);
	if (uNormalMatrix && pass == COLOR_PASS) 
		shader->setMatrix3("uNormalMatrix", uNormalMatrix);
	if (animData->animation->boneTransformMats)
		shader->setMatrix3x4("boneMats", animData->boneCount, animData->animation->boneTransformMats);
	dataBuffer->use();
	glDrawElements(GL_TRIANGLES, animData->indexCount, GL_UNSIGNED_SHORT, 0);
}

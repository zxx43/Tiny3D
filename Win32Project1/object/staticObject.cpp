#include "staticObject.h"
#include "../node/node.h"
#include "../util/util.h"
#include "../scene/scene.h"

StaticObject::StaticObject(Mesh* mesh) :Object() {
	this->mesh = mesh;
	this->meshMid = mesh;
	this->meshLow = mesh;
	positionBefore = vec3(0.0);
	initMatricesData();
	type = OBJ_TYPE_STATIC;
}

StaticObject::StaticObject(Mesh* mesh, Mesh* meshMid, Mesh* meshLow) :Object() {
	this->mesh = mesh;
	this->meshMid = meshMid;
	this->meshLow = meshLow;
	positionBefore = vec3(0.0);
	initMatricesData();
	type = OBJ_TYPE_STATIC;
}

StaticObject::StaticObject(const StaticObject& rhs) :Object(rhs) {
	mesh = rhs.mesh;
	meshMid = rhs.meshMid;
	meshLow = rhs.meshLow;
	if (rhs.bounding)
		bounding = rhs.bounding->clone();
	else
		bounding = NULL;
	positionBefore = rhs.positionBefore;

	position = rhs.position;
	size = rhs.size;
	localTransformMatrix = rhs.localTransformMatrix;
	normalMatrix = rhs.normalMatrix;

	rotateMat = rhs.rotateMat;
	translateMat = rhs.translateMat;
	scaleMat = rhs.scaleMat;

	genShadow = rhs.genShadow;
	detailLevel = rhs.detailLevel;

	if (rhs.transforms) {
		transforms = (float*)malloc(4 * sizeof(float));
		memcpy(transforms, rhs.transforms, 4 * sizeof(float));
	}
	if (rhs.transformsFull) {
		transformsFull = (buff*)malloc(16 * sizeof(buff));
		memcpy(transformsFull, rhs.transformsFull, 16 * sizeof(buff));
	}

	type = rhs.type;
}

StaticObject::~StaticObject() {

}

StaticObject* StaticObject::clone() {
	return new StaticObject(*this);
}

void StaticObject::vertexTransform() {
	translateMat = translate(position.x, position.y, position.z);
	rotateMat = Quat2Mat(rotateQuat);
	scaleMat = scale(size.x, size.y, size.z);
	localTransformMatrix = translateMat * rotateMat * scaleMat;
}

void StaticObject::normalTransform() {
	if (size.x == size.y && size.y == size.z) {
		normalMatrix = localTransformMatrix;
		return;
	}
	normalMatrix = localTransformMatrix.GetInverse().GetTranspose();
}

void StaticObject::setPosition(float x, float y, float z) {
	positionBefore = GetTranslate(localTransformMatrix);
	position.x = x;
	position.y = y;
	position.z = z;
	updateLocalMatrices();
}

void StaticObject::setRotation(float ax, float ay, float az) {
	rotateQuat = Euler2Quat(vec3(ax, ay, az));
	updateLocalMatrices();
}

void StaticObject::setSize(float sx, float sy, float sz) {
	size = vec3(sx, sy, sz);
	updateLocalMatrices();
	//if (billboard) { // todo in shader
	//	billboard->data[0] *= size.x;
	//	billboard->data[1] *= size.y;
	//}
}

void StaticObject::translateAtWorld(const vec3& position) {
	mat4 gParentTransform = parent->nodeTransform; // Parent node's global transform
	vec3 gParentPosition = GetTranslate(gParentTransform);

	vec3 positionBefore = this->position;
	vec3 lPosition = position - gParentPosition;
	setPosition(lPosition.x, lPosition.y, lPosition.z);

	static float pushThrehold = 0.1;
	if (fabsf(this->position.x - positionBefore.x) > pushThrehold || fabsf(this->position.z - positionBefore.z) > pushThrehold)
		playEffect("push");
}

void StaticObject::rotateAtWorld(const vec4& q) {
	rotateQuat = q;
	updateLocalMatrices();
}

void StaticObject::standOnGround(Scene* scene) {
	vec3 worldCenter = GetTranslate(parent->nodeTransform * localTransformMatrix);
	int bx, bz;
	scene->terrainNode->caculateBlock(worldCenter.x, worldCenter.z, bx, bz);
	scene->terrainNode->cauculateY(bx, bz, worldCenter.x, worldCenter.z, worldCenter.y);
	worldCenter.y += collisionShape->getBox()->getHalfExtentsWithMargin().y();

	translateAtWorld(worldCenter);
	collisionObject->initTranslate(worldCenter);

	// Update object's aabb
	localBoundPosition = boundCenter + GetTranslate(localTransformMatrix);
	parent->updateObjectBoundingInNode(this, true);
	parent->boundingBox->merge(parent->objectsBBs);
	Node* superior = parent->parent;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}
}





#include "object.h"
#include <stdlib.h>
#include "../node/node.h"
#include "../constants/constants.h"

Object::Object() {
	belong = NULL;
	parent = NULL;
	position = vec3(0.0);
	size = vec3(1.0);
	localTransformMatrix.LoadIdentity();
	normalMatrix.LoadIdentity();

	translateMat.LoadIdentity();
	rotateMat.LoadIdentity();
	scaleMat.LoadIdentity();

	mesh = NULL;
	meshMid = NULL;
	meshLow = NULL;
	bounding = NULL;
	baseBounding = NULL;
	material = -1;
	boundCenter = vec3(0.0);
	localBoundPosition = vec3(0.0);

	billboard = NULL;
	genShadow = true;
	detailLevel = 2;

	transforms = NULL;
	transformsFull = NULL;
	rotateQuat = MatrixToQuat(rotateMat);
	boundInfo = vec4(0.0);

	shapeOffset = vec3(0.0);
	collisionShape = NULL;
	collisionObject = NULL;
	mass = 0;
	dynamic = false;
	hasPhysic = true;
	debug = false;

	sounds.clear();
	type = OBJ_TYPE_NONE;
	groupid = 0;
	boardid = -1;
}

Object::Object(const Object& rhs) {
	belong = rhs.belong;
	parent = rhs.parent;
	if (rhs.billboard)
		billboard = new Billboard(*rhs.billboard);
	else
		billboard = NULL;
	transforms = NULL;
	transformsFull = NULL;
	rotateQuat = rhs.rotateQuat;
	boundInfo = rhs.boundInfo;
	material = rhs.material;

	shapeOffset = rhs.shapeOffset;
	collisionShape = NULL;
	collisionObject = NULL;
	mass = rhs.mass;
	dynamic = rhs.dynamic;
	hasPhysic = rhs.hasPhysic;
	debug = rhs.debug;
	boundCenter = rhs.boundCenter;
	localBoundPosition = rhs.localBoundPosition;
	baseBounding = rhs.baseBounding ? rhs.baseBounding->clone() : NULL;
	bounding = rhs.bounding ? rhs.bounding->clone() : NULL;

	sounds.clear();
	std::map<std::string, SoundObject*>::iterator it;
	std::map<std::string, SoundObject*> soundsToCopy = rhs.sounds;
	for (it = soundsToCopy.begin(); it != soundsToCopy.end(); ++it)
		sounds[it->first] = new SoundObject(*it->second);

	type = rhs.type;
	groupid = rhs.groupid;
	boardid = rhs.boardid;
}

Object::~Object() {
	if (baseBounding) delete baseBounding; baseBounding = NULL;
	if (bounding) delete bounding; bounding = NULL;
	if (billboard) delete billboard; billboard = NULL;

	if (transforms) free(transforms); transforms = NULL;
	if (transformsFull) free(transformsFull); transformsFull = NULL;
	
	if (collisionShape) delete collisionShape;

	std::map<std::string, SoundObject*>::iterator it;
	for (it = sounds.begin(); it != sounds.end(); ++it)
		delete it->second;
	sounds.clear();
}

void Object::initMatricesData() {
	transforms = (float*)malloc(4 * sizeof(float));
	transformsFull = (buff*)malloc(16 * sizeof(buff));

	transforms[0] = 0.0, transforms[1] = 0.0, transforms[2] = 0.0, transforms[3] = 1.0;
	transformsFull[0] = transforms[0];
	transformsFull[1] = transforms[1];
	transformsFull[2] = transforms[2];
	transformsFull[3] = transforms[3];
	transformsFull[4] = rotateQuat.x;
	transformsFull[5] = rotateQuat.y;
	transformsFull[6] = rotateQuat.z;
	transformsFull[7] = rotateQuat.w;
}

void Object::caculateLocalAABB(bool hasRotateScale) {
	if (!mesh) return; // caculate AABB by yourself
	int vertexCount = mesh->vertexCount;
	if (vertexCount <= 0) return;

	if (!baseBounding || hasRotateScale) {
		vec4* vertices = mesh->vertices;
		mat4 localScaleRotateMatrix = GetRotateAndScale(localTransformMatrix);
		vec4 first4 = localScaleRotateMatrix * vertices[0];
		float firstInvw = 1.0 / first4.w;
		float sx = first4.x * firstInvw;
		float sy = first4.y * firstInvw;
		float sz = first4.z * firstInvw;
		float lx = sx;
		float ly = sy;
		float lz = sz;
		for (int i = 1; i < vertexCount; i++) {
			vec4 vertex4 = vertices[i];
			vec4 local4 = localScaleRotateMatrix * vertex4;
			float invw = 1.0 / local4.w;
			vec3 local3(local4.x * invw, local4.y * invw, local4.z * invw);
			sx = sx > local3.x ? local3.x : sx;
			sy = sy > local3.y ? local3.y : sy;
			sz = sz > local3.z ? local3.z : sz;
			lx = lx < local3.x ? local3.x : lx;
			ly = ly < local3.y ? local3.y : ly;
			lz = lz < local3.z ? local3.z : lz;
		}
		vec3 minVertex(sx, sy, sz), maxVertex(lx, ly, lz);
		// replace base bounding with accurate vertices
		if (!baseBounding) baseBounding = new BoxInfo(minVertex, maxVertex);
		else baseBounding->update(minVertex, maxVertex);
	}

	if (!bounding) bounding = new AABB(baseBounding->minPos, baseBounding->maxPos);
	else {
		AABB* aabb = (AABB*)bounding;
		aabb->update(baseBounding->minPos, baseBounding->maxPos);
	}

	boundCenter = bounding->position;
	localBoundPosition = boundCenter + GetTranslate(localTransformMatrix);
	bounding->update(localBoundPosition);
}

void Object::caculateLocalAABBFast(bool hasRotateScale) {
	if (!mesh) return; // caculate AABB by yourself
	int vertexCount = mesh->vertexCount;
	if (vertexCount <= 0) return;

	if (!baseBounding || hasRotateScale) {
		if (!mesh->boundBox) mesh->caculateExData();
		// replace bounding box with mesh bounding
		if (!bounding) bounding = new AABB(mesh->boundBox->minPos, mesh->boundBox->maxPos);
		else {
			AABB* aabb = (AABB*)bounding;
			aabb->update(mesh->boundBox->minPos, mesh->boundBox->maxPos);
		}

		AABB* aabb = (AABB*)bounding;
		// apply rotate & scale to bounding box
		aabb->update(GetRotateAndScale(localTransformMatrix));

		if (!baseBounding) baseBounding = new BoxInfo(aabb->minVertex, aabb->maxVertex);
		else baseBounding->update(aabb->minVertex, aabb->maxVertex);
	}

	if (!bounding) bounding = new AABB(baseBounding->minPos, baseBounding->maxPos);
	else {
		AABB* aabb = (AABB*)bounding;
		aabb->update(baseBounding->minPos, baseBounding->maxPos);
	}

	boundCenter = bounding->position;
	localBoundPosition = boundCenter + GetTranslate(localTransformMatrix);
	bounding->update(localBoundPosition);
}

void Object::caculateCollisionShape() {
	if (collisionShape) delete collisionShape;
	if (!mesh) { // Animation object use node bounding box
		vec3 halfSize = ((AABB*)parent->boundingBox)->halfSize;
		collisionShape = new CollisionShape(halfSize);
	} else {
		float sx = MAX_VAL; float sy = MAX_VAL; float sz = MAX_VAL;
		float lx = MIN_VAL; float ly = MIN_VAL; float lz = MIN_VAL;
		for (uint n = 0; n < mesh->vertexCount; ++n) {
			int mid = material;
			Material* mat = NULL;
			bool isLeaf = false;
			if (mid >= 0) mat = MaterialManager::materials->find(mid);
			if (!mat) {
				mid = mesh->materialids[n];
				if (mid >= 0) mat = MaterialManager::materials->find(mid);
			}
			if (mat) isLeaf = mat->leaf;
			if (!isLeaf) {
				vec4 vertex = mesh->vertices[n];
				vertex = scale(size.x, size.y, size.z) * vertex;
				float invw = 1.0 / vertex.w;
				vertex.x *= invw;
				vertex.y *= invw;
				vertex.z *= invw;
				
				sx = sx > vertex.x ? vertex.x : sx;
				sy = sy > vertex.y ? vertex.y : sy;
				sz = sz > vertex.z ? vertex.z : sz;
				lx = lx < vertex.x ? vertex.x : lx;
				ly = ly < vertex.y ? vertex.y : ly;
				lz = lz < vertex.z ? vertex.z : lz;
			}
		}
		vec3 halfSize = vec3(lx - sx, ly - sy, lz - sz) * 0.5;
		halfSize *= mesh->getBoundScale();
		collisionShape = new CollisionShape(halfSize);
	}
}

CollisionObject* Object::initCollisionObject() {
	if (!collisionObject)
		collisionObject = new CollisionObject(collisionShape->shape, mass);
	else {
		collisionObject->setCollisionShape(collisionShape->shape);
		collisionObject->setMass(mass);
	}
	collisionObject->object->setUserPointer(this);
	if (mass > 0) 
		collisionObject->object->setActivationState(DISABLE_DEACTIVATION);
	return collisionObject;
}

void Object::removeCollisionObject() {
	if (collisionObject) delete collisionObject;
	collisionObject = NULL;
}

void Object::updateLocalMatrices() {
	vertexTransform();
	normalTransform();
}

void Object::bindMaterial(int mid) {
	material = mid;
}

bool Object::checkInCamera(Camera* camera) {
	if (!bounding) return true;
	return bounding->checkWithCamera(camera->frustum, detailLevel);
}

bool Object::sphereInCamera(Camera* camera) {
	if (!bounding) return true;
	return bounding->sphereWithCamera(camera->frustum);
}

void Object::setBillboard(float sx, float sy, int mid) {
	if (billboard) delete billboard;
	billboard = new Billboard(sx, sy, mid);
}

void Object::updateObjectTransform(bool translate, bool rotate) {
	if (translate) {
		transformMatrix = parent->nodeTransform * localTransformMatrix;
		transformTransposed = transformMatrix.GetTranspose();
	}
	if (translate || rotate) {
		AABB* bbox = (AABB*)bounding;
		if (!bbox) bbox = (AABB*)parent->boundingBox;
		boundInfo = vec4(bbox->sizex, bbox->sizey, bbox->sizez, bbox->position.y);
	}
	if (transforms && translate) {
		vec3 transPos = GetTranslate(transformMatrix);
		transforms[0] = transPos.x;
		transforms[1] = transPos.y;
		transforms[2] = transPos.z;
		transforms[3] = size.x;
		updateSoundsPosition(transPos);
	}
	if (transformsFull) {
		if (translate) 
			memcpy(transformsFull, transforms, 4 * sizeof(buff));
		if (rotate) {
			transformsFull[4] = (rotateQuat.x);
			transformsFull[5] = (rotateQuat.y);
			transformsFull[6] = (rotateQuat.z);
			transformsFull[7] = (rotateQuat.w);
		}
		if (translate || rotate) {
			transformsFull[8] = (boundInfo.x);
			transformsFull[9] = (boundInfo.y);
			transformsFull[10] = (boundInfo.z);
			transformsFull[11] = (boundInfo.w);
		}
	}
}

void Object::updateSoundsPosition(const vec3& position) {
	std::map<std::string, SoundObject*>::iterator it;
	for (it = sounds.begin(); it != sounds.end(); ++it)
		it->second->setPosition(position);
}

void Object::setSound(const char* name, const char* path) {
	std::map<std::string, SoundObject*>::iterator it = sounds.find(name);
	if (it != sounds.end()) delete it->second;
	else sounds[name] = new SoundObject(path);
}

SoundObject* Object::getSound(const char* name) { 
	std::map<std::string, SoundObject*>::iterator it = sounds.find(name);
	return (it == sounds.end()) ? NULL : it->second;
}
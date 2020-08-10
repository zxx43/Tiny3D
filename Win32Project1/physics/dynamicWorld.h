#ifndef DYNAMIC_WORLD_H_
#define DYNAMIC_WORLD_H_

#include <list>
#include "../util/util.h"
#include <bullet/btBulletDynamicsCommon.h>

struct CollisionShape {
	btCollisionShape* shape;
	CollisionShape(const vec3& halfSize) {
		shape = new btBoxShape(btVector3(halfSize.x, halfSize.y, halfSize.z));
	}
	CollisionShape(float radius, float height) {
		shape = new btConeShape(radius, height);
	}
	~CollisionShape() {
		delete shape;
	}
};

struct CollisionObject {
	btRigidBody* object;
	btMotionState* motion;
	float threhold;
	CollisionObject(btCollisionShape* shape, float mass) {
		threhold = 0.0;
		btVector3 inertia(0.0, 0.0, 0.0);
		if (mass > 0.0) shape->calculateLocalInertia(mass, inertia);
		btTransform trans; trans.setIdentity();
		motion = new btDefaultMotionState(trans);
		object = new btRigidBody(mass, motion, shape, inertia);
		object->setUserIndex(-1);
	}
	~CollisionObject() {
		if (motion) delete motion;
		object->setMotionState(NULL);
		delete object;
	}
	void setCollisionShape(btCollisionShape* shape) {
		object->setCollisionShape(shape);
	}
	void setMass(float mass) {
		btVector3 inertia(0.0, 0.0, 0.0);
		if (mass > 0.0) object->getCollisionShape()->calculateLocalInertia(mass, inertia);
		object->setMassProps(mass, inertia);
	}
	bool isStatic() {
		return (!object->isActive() || object->isStaticObject());
	}
	void initTranslate(const vec3& pos) {
		btTransform trans = object->getWorldTransform();
		trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
		object->setWorldTransform(trans);
	}
	void initRotate(const vec4& rot) {
		btTransform trans = object->getWorldTransform();
		trans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
		object->setWorldTransform(trans);
	}
	void initTransform(const vec3& pos, const vec4& rot) {
		btTransform trans = object->getWorldTransform();
		trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
		trans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
		object->setWorldTransform(trans);
	}
	void setTranslate(const vec3& after, const vec3& before) {
		object->activate();
		vec3 vel = after - before;
		if (fabsf(vel.x) < 0.0001) vel.x = 0.0;
		if (fabsf(vel.y) < 0.0001) vel.y = 0.0;
		if (fabsf(vel.z) < 0.0001) vel.z = 0.0;
		object->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
	}
	void setRotate(const vec3& after, const vec3& before) {
		object->activate();
		vec3 vel = after - before;
		if (fabsf(vel.x) < 0.0001) vel.x = 0.0;
		if (fabsf(vel.y) < 0.0001) vel.y = 0.0;
		if (fabsf(vel.z) < 0.0001) vel.z = 0.0;
		vel *= A2R;
		object->setAngularVelocity(btVector3(vel.x, vel.y, vel.z));
	}
	vec3 getTranslate() {
		btTransform trans = object->getWorldTransform();
		btVector3 res = trans.getOrigin();
		return vec3(res.getX(), res.getY(), res.getZ());
	}
	vec3 getRotate() {
		btTransform trans = object->getWorldTransform();
		btQuaternion quat = trans.getRotation();
		vec3 angle;
		quat.getEulerZYX(angle.z, angle.y, angle.x);
		angle *= R2A;
		return angle;
	}
	vec3 getLinearVelocity() {
		btVector3 vel = object->getLinearVelocity();
		if (fabsf(vel.x()) < threhold) vel.setX(0.0);
		if (fabsf(vel.y()) < threhold) vel.setY(0.0);
		if (fabsf(vel.z()) < threhold) vel.setZ(0.0);
		return vec3(vel.x(), vel.y(), vel.z());
	}
	vec3 getAngularVelocity() {
		btVector3 ang = object->getAngularVelocity();
		if (fabsf(ang.x()) < threhold) ang.setX(0.0);
		if (fabsf(ang.y()) < threhold) ang.setY(0.0);
		if (fabsf(ang.z()) < threhold) ang.setZ(0.0);
		return vec3(ang.x(), ang.y(), ang.z());
	}
	void resetVelocity() {
		object->setLinearVelocity(btVector3(0, 0, 0));
		object->setAngularVelocity(btVector3(0, 0, 0));
	}
};

class DynamicWorld {
private:
	std::list<CollisionObject*> objects;
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
public:
	DynamicWorld();
	~DynamicWorld();
public:
	void addObject(CollisionObject* cob);
	void removeObject(CollisionObject* cob);
	void act(float dtime);
	int getNumCollisionObjects() { return dynamicsWorld->getNumCollisionObjects(); }
	btCollisionObjectArray& getCollisionObjectArray() { return dynamicsWorld->getCollisionObjectArray(); }
};

#endif
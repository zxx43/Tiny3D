#ifndef DYNAMIC_WORLD_H_
#define DYNAMIC_WORLD_H_

#include <list>
#include "../util/util.h"
#include <bullet/btBulletDynamicsCommon.h>

inline mat4 Quat2Mat(const vec4& q) {
	btQuaternion quat(q.x, q.y, q.z, q.w);
	btTransform tran(quat);
	mat4 res;
	tran.getOpenGLMatrix(res);
	return res;
}

inline vec3 GetAxis(const btQuaternion& q, const vec3& axis) {
	btTransform tran(q);
	mat4 mat;
	tran.getOpenGLMatrix(mat);
	vec4 aix = vec4(axis, 0.0);
	vec3 res = mat * aix;
	return res;
}

inline vec4 Euler2Quat(const vec3& angle) {
	vec3 ax(1, 0, 0), ay(0, 1, 0), az(0, 0, 1);
	vec3 ang = angleToRadian(angle);
	btQuaternion quat(0, 0, 0, 1);
	btQuaternion qx; qx.setRotation(btVector3(ax.x, ax.y, ax.z), ang.x);
	quat = qx * quat;
	mat4 m = Quat2Mat(vec4(quat.x(), quat.y(), quat.z(), quat.w()));
	ay = GetAxisY(m);
	btQuaternion qy; qy.setRotation(btVector3(ay.x, ay.y, ay.z), ang.y);
	quat = qy * quat;
	m = Quat2Mat(vec4(quat.x(), quat.y(), quat.z(), quat.w()));
	az = GetAxisZ(m);
	btQuaternion qz; qz.setRotation(btVector3(az.x, az.y, az.z), ang.z);
	quat = qz * quat;

	vec4 res(quat.x(), quat.y(), quat.z(), quat.w());
	return res;
}

inline vec4 MulQuat(const vec4& quat1, const vec4& quat2) {
	btQuaternion q1(quat1.x, quat1.y, quat1.z, quat1.w);
	btQuaternion q2(quat2.x, quat2.y, quat2.z, quat2.w);
	btQuaternion qr = q1 * q2;
	vec4 res(qr.x(), qr.y(), qr.z(), qr.w());
	return res;
}

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
	btBoxShape* getBox() {
		return (btBoxShape*)shape;
	}
	btConeShape* getCone() {
		return (btConeShape*)shape;
	}
};

struct CollisionObject {
	btRigidBody* object;
	btMotionState* motion;
	vec3 ax, ay, az;
	CollisionObject(btCollisionShape* shape, float mass) {
		btVector3 inertia(0.0, 0.0, 0.0);
		if (mass > 0.0) shape->calculateLocalInertia(mass, inertia);
		btTransform trans; trans.setIdentity();
		motion = new btDefaultMotionState(trans);
		object = new btRigidBody(mass, motion, shape, inertia);
		object->setUserIndex(-1);
		resetVelocity();
		ax = vec3(1, 0, 0);
		ay = vec3(0, 1, 0);
		az = vec3(0, 0, 1);
	}
	~CollisionObject() {
		if (motion) delete motion;
		object->setMotionState(NULL);
		delete object;
	}
	void setAxis(const btQuaternion& q) {
		mat4 m = Quat2Mat(vec4(q.x(), q.y(), q.z(), q.w()));
		ax = GetAxisX(m);
		ay = GetAxisY(m);
		az = GetAxisZ(m);
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
		btQuaternion q = btQuaternion(rot.x, rot.y, rot.z, rot.w);
		trans.setRotation(q);
		object->setWorldTransform(trans);
		setAxis(q);
	}
	void initTransform(const vec3& pos, const vec4& rot) {
		btTransform trans = object->getWorldTransform();
		trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
		btQuaternion q = btQuaternion(rot.x, rot.y, rot.z, rot.w);
		trans.setRotation(q);
		object->setWorldTransform(trans);
		setAxis(q);
	}
	void setTranslate(const vec3& after, const vec3& before) {
		object->activate();
		vec3 vel = after - before;
		object->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
	}
	void setRotateAngle(const vec3& angle, bool inverseYZ) {
		btTransform trans = object->getWorldTransform();
		btQuaternion quat(0, 0, 0, 1);
		btQuaternion qx; qx.setRotation(btVector3(ax.x, ax.y, ax.z), angleToRadian(angle.x));
		quat = qx * quat;
		setAxis(quat);
		btQuaternion qy; qy.setRotation(btVector3(ay.x, ay.y, ay.z), angleToRadian(angle.y));
		quat = qy * quat;
		setAxis(quat);
		if (inverseYZ) {
			btQuaternion qex;
			qex.setRotation(btVector3(ax.x, ax.y, ax.z), angleToRadian(270));
			quat = qex * quat;
			setAxis(quat);
		}
		btQuaternion qz; qz.setRotation(btVector3(az.x, az.y, az.z), angleToRadian(angle.z));
		quat = qz * quat;
		setAxis(quat);
		trans.setRotation(quat);
		object->setWorldTransform(trans);
	}
	void setRotate(const vec4& quat) {
		btTransform trans = object->getWorldTransform();
		btQuaternion q(quat.x, quat.y, quat.z, quat.w);
		trans.setRotation(q);
		object->setWorldTransform(trans);
	}
	vec3 getTranslate() {
		btTransform trans = object->getWorldTransform();
		btVector3 res = trans.getOrigin();
		return vec3(res.getX(), res.getY(), res.getZ());
	}
	vec4 getRotate() {
		btTransform trans = object->getWorldTransform();
		btQuaternion quat = trans.getRotation();
		return vec4(quat.x(), quat.y(), quat.z(), quat.w());
	}
	vec3 getLinearVelocity() {
		btVector3 vel = object->getLinearVelocity();
		return vec3(vel.x(), vel.y(), vel.z());
	}
	void resetVelocity() {
		object->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
		object->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
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
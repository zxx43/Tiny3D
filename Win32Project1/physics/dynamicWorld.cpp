#include "dynamicWorld.h"

DynamicWorld::DynamicWorld() {
	objects.clear();
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0.0, -10.0, 0.0));
	dynamicsWorld->setForceUpdateAllAabbs(true);
}

DynamicWorld::~DynamicWorld() {
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		dynamicsWorld->removeCollisionObject(dynamicsWorld->getCollisionObjectArray()[i]);
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;

	std::list<CollisionObject*>::iterator it = objects.begin();
	while (it != objects.end()) {
		delete *it;
		++it;
	}
	objects.clear();
}

void DynamicWorld::addObject(CollisionObject* cob) {
	if (cob->object->getUserIndex() >= 0) return;
	cob->object->setUserIndex(objects.size());
	dynamicsWorld->addRigidBody(cob->object);
	objects.push_back(cob);
}

void DynamicWorld::removeObject(CollisionObject* cob) {
	dynamicsWorld->removeRigidBody(cob->object);
	objects.remove(cob);
}

void DynamicWorld::act(float dtime) {
	dynamicsWorld->stepSimulation(dtime, 10, 0.0083334f);
}
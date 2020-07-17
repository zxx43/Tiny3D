#include "node.h"
#include "../util/util.h"
#include "../instance/instance.h"
#include "../scene/scene.h"

std::vector<Node*> Node::nodesToUpdate;
std::vector<Node*> Node::nodesToRemove;

Node::Node(const vec3& position,const vec3& size) {
	this->position = position;
	this->size = size;
	boundingBox=new AABB(position,size.x,size.y,size.z);
	objects.clear();
	objectsBBs.clear();
	drawcall=NULL;
	needUpdateDrawcall = false;
	needCreateDrawcall = false;
	needUpdateNormal = false;
	needUpdateNode = false;

	parent=NULL;
	children.clear();
	nodeBBs.clear();

	type = TYPE_NULL;
	shadowLevel = 3;
	detailLevel = 3;
}

Node::~Node() {
	if(boundingBox)
		delete boundingBox;
	boundingBox=NULL;

	objectsBBs.clear();
	for (uint i = 0; i < objects.size(); i++) 
		delete objects[i];
	objects.clear();

	if(drawcall)
		delete drawcall;
	drawcall=NULL;

	nodeBBs.clear();
	clearChildren();
}

void Node::clearChildren() {
	for(unsigned int i=0;i<children.size();i++) {
		Node* child=children[i];
		child->clearChildren();
		delete child;
	}
	children.clear();
}

bool Node::checkInCamera(Camera* camera) {
	return checkInFrustum(camera->frustum);
}

bool Node::checkInFrustum(Frustum* frustum) {
	if (boundingBox)
		return boundingBox->checkWithCamera(frustum, detailLevel);
	return true;
}

// Update Object's bounding box from local to world
void Node::updateObjectBoundingInNode(Object* object) {
	BoundingBox* objectBB = object->bounding;
	if (objectBB) {
		mat4 nodeMat; nodeMat.LoadIdentity();
		recursiveTransform(nodeMat);
		vec4 localBB4(object->localBoundPosition.x, object->localBoundPosition.y, object->localBoundPosition.z, 1.0);
		vec4 bb4 = nodeMat * localBB4;
		float invw = 1.0 / bb4.w;
		objectBB->update(vec3(bb4.x * invw, bb4.y * invw, bb4.z * invw));
	}
}

void Node::addObject(Scene* scene, Object* object) {
	object->parent = this;
	objects.push_back(object);
	object->caculateLocalAABB(false, false);
	BoundingBox* objectBB = object->bounding;
	if (objectBB) {
		updateObjectBoundingInNode(object);
		objectsBBs.push_back(objectBB);
		boundingBox->merge(objectsBBs);

		Node* superior = parent;
		while (superior) {
			superior->updateBounding();
			superior = superior->parent;
		}
	}
	needCreateDrawcall = true;
	pushToUpdate(scene);
}

Object* Node::removeObject(Scene* scene, Object* object) {
	std::vector<Object*>::iterator it;
	std::vector<BoundingBox*>::iterator itbb;
	for (it = objects.begin(); it != objects.end(); ++it) {
		if ((*it) == object) {
			objects.erase(it);
			for (itbb = objectsBBs.begin(); itbb != objectsBBs.end(); ++itbb) {
				if ((*itbb) == object->bounding) {
					objectsBBs.erase(itbb);
					break;
				}
			}
			boundingBox->merge(objectsBBs);

			Node* superior = parent;
			while (superior) {
				superior->updateBounding();
				superior = superior->parent;
			}

			needCreateDrawcall = true;
			pushToUpdate(scene);
			object->parent = NULL;

			return object;
		}
	}
	return NULL;
}

// Update the Node's bounding with objects maybe its children's
void Node::updateBaseNodeBounding() {
	for(unsigned int i=0;i<objects.size();i++)
		updateObjectBoundingInNode(objects[i]);
	if (objects.size()>0) {
		if (objectsBBs.size() > 0) 
			boundingBox->merge(objectsBBs);
		else if (objectsBBs.size() <= 0) { // Base Node and without object boundings
			mat4 nodeBBTransform; nodeBBTransform.LoadIdentity();
			recursiveTransform(nodeBBTransform);
			boundingBox->update(GetTranslate(nodeBBTransform));
			//boundingBox->update(nodeBBTransform * vec4(0, 0, 0, 1));
		}
	}

	for(unsigned int n=0;n<children.size();n++)
		children[n]->updateBaseNodeBounding();
}

// Update Node's bounding & its children's & children's children...
void Node::updateSelfAndDownwardNodesBounding() {
	nodeBBs.clear();
	for(unsigned int n=0;n<children.size();n++) {
		Node* child=children[n];
		child->updateSelfAndDownwardNodesBounding();
		AABB* childAABB=(AABB*)child->boundingBox;
		if(childAABB&&(childAABB->sizex>0||childAABB->sizey>0||childAABB->sizez>0))
			nodeBBs.push_back(child->boundingBox);
	}
	if (nodeBBs.size()>0)
		boundingBox->merge(nodeBBs);
}

// Move Node's objects's bounding
void Node::moveBaseObjectsBounding(float dx, float dy, float dz) {
	vec3 offset = vec3(dx, dy, dz);
	for (uint i = 0; i < objects.size(); i++) {
		BoundingBox* objectBB = objects[i]->bounding;
		if (objectBB) objectBB->update(objectBB->position + offset);
	}
	for (uint n = 0; n < children.size(); n++)
		children[n]->moveBaseObjectsBounding(dx, dy, dz);
}

// Just move Node's bounding & its children's & children's children...
void Node::moveSelfAndDownwardNodesBounding(float dx, float dy, float dz) {
	if (boundingBox) {
		vec3 offset = vec3(dx, dy, dz);
		boundingBox->update(boundingBox->position + offset);
	}
	for (uint n = 0; n < children.size(); n++) 
		children[n]->moveSelfAndDownwardNodesBounding(dx, dy, dz);
}

// Update current Node's bounding
void Node::updateBounding() {
	nodeBBs.clear();
	for (unsigned int n = 0; n<children.size(); n++) {
		Node* child = children[n];
		AABB* childAABB = (AABB*)child->boundingBox;
		if (childAABB && (childAABB->sizex > 0 || childAABB->sizey > 0 || childAABB->sizez > 0))
			nodeBBs.push_back(child->boundingBox);
	}
	if (nodeBBs.size()>0)
		boundingBox->merge(nodeBBs);
}

// Update Node's drawcall & its children's & children's children...
void Node::updateSelfAndDownwardNodesDrawcall(Scene* scene, bool updateNormal) {
	if (objects.size() > 0) {
		needUpdateNormal = updateNormal;
		needUpdateDrawcall = true;
		pushToUpdate(scene);
	}

	for (unsigned int i = 0; i < children.size(); i++)
		children[i]->updateSelfAndDownwardNodesDrawcall(scene, updateNormal);
}

// Find ancestor of this Node
Node* Node::getAncestor() {
	Node* root = this;
	Node* superior = parent;
	while (superior) {
		root = superior;
		superior = superior->parent;
	}
	return root;
}

void Node::attachChild(Scene* scene, Node* child) {
	children.push_back(child);
	child->parent=this;

	child->updateBaseNodeBounding();
	child->updateSelfAndDownwardNodesBounding();
	child->updateNodeTransform();

	Node* superior = this;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}

	updateSelfAndDownwardNodesDrawcall(scene, false);
}

Node* Node::detachChild(Node* child) {
	std::vector<Node*>::iterator it;
	for(it=children.begin();it!=children.end();++it) {
		if((*it)==child) {
			child->parent=NULL;
			children.erase(it);

			Node* superior = this;
			while (superior) {
				superior->updateBounding();
				superior = superior->parent;
			}

			if (child->type == TYPE_INSTANCE) {
				for (uint i = 0; i < child->objects.size(); i++) {
					Object* object = child->objects[i];
					Instance::instanceTable[object->mesh]--;
					if (object->meshMid)
						Instance::instanceTable[object->meshMid]--;
					if (object->meshLow)
						Instance::instanceTable[object->meshLow]--;
				}
			}

			return child;
		}
	}
	return NULL;
}

void Node::translateNode(Scene* scene, float x, float y, float z) {
	float dx = x - position.x;
	float dy = y - position.y;
	float dz = z - position.z;

	position.x = x;
	position.y = y;
	position.z = z;

	// Inefficient
	//updateBaseNodeBounding();
	//updateSelfAndDownwardNodesBounding();
	moveBaseObjectsBounding(dx, dy, dz);
	moveSelfAndDownwardNodesBounding(dx, dy, dz);

	Node* superior = parent;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}

	updateSelfAndDownwardNodesDrawcall(scene, false);
	updateNodeTransform();
}

void Node::translateNodeObject(Scene* scene, int i, float x, float y, float z) {
	Object* object = objects[i];
	object->setPosition(x, y, z);
	object->caculateLocalAABB(false, false);

	updateObjectBoundingInNode(object);
	boundingBox->merge(objectsBBs);
	Node* superior = parent;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}
	needUpdateNormal = false;
	needUpdateDrawcall = true;
	pushToUpdate(scene);
}

void Node::translateNodeObjectCenterAtWorld(Scene* scene, int i, float x, float y, float z) {
	Object* object = objects[i];
	vec3 worldCenter = object->bounding->position;
	vec3 offset = vec3(x, y, z) - worldCenter;
	vec3 localPosition = object->position;
	translateNodeObject(scene, i, localPosition.x + offset.x, localPosition.y + offset.y, localPosition.z + offset.z);
}

void Node::rotateNodeObject(Scene* scene, int i, float ax, float ay, float az) {
	Object* object = objects[i];
	object->setRotation(ax,ay,az);
	object->caculateLocalAABB(false, false);

	updateObjectBoundingInNode(object);
	boundingBox->merge(objectsBBs);
	Node* superior = parent;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}
	needUpdateNormal = true;
	needUpdateDrawcall = true;
	pushToUpdate(scene);
}

void Node::scaleNodeObject(Scene* scene, int i, float sx, float sy, float sz) {
	Object* object = objects[i];
	object->setSize(sx, sy, sz);
	object->caculateLocalAABB(false, false);

	updateObjectBoundingInNode(object);
	boundingBox->merge(objectsBBs);
	Node* superior = parent;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}
	if (sx == sy && sy == sz) needUpdateNormal = false;
	else needUpdateNormal = true;
	needUpdateDrawcall = true;
	pushToUpdate(scene);
}

void Node::pushToUpdate(Scene* scene) {
	if (!needUpdateNode) {
		Node::nodesToUpdate.push_back(this);
		needUpdateNode = true;
	}
}

void Node::updateNodeObject(Object* object, bool translate, bool rotate) {
	if (translate) {
		object->transformMatrix = nodeTransform * object->localTransformMatrix;
		object->transformTransposed = object->transformMatrix.GetTranspose();
	}
	if(rotate) object->rotateQuat = MatrixToQuat(object->rotateMat);
	if (translate || rotate) {
		AABB* bbox = (AABB*)object->bounding;
		if (!bbox) bbox = (AABB*)boundingBox;
		object->boundInfo = vec4(bbox->sizex, bbox->sizey, bbox->sizez, bbox->position.y);
	}
	if (object->transforms && translate) {
		vec3 transPos = GetTranslate(object->transformMatrix);
		object->transforms[0] = transPos.x;
		object->transforms[1] = transPos.y;
		object->transforms[2] = transPos.z;
		object->transforms[3] = object->size.x;
	}
	if (object->transformsFull) {
		if (translate) {
			object->transformsFull[0] = (object->transforms[0]);
			object->transformsFull[1] = (object->transforms[1]);
			object->transformsFull[2] = (object->transforms[2]);
			object->transformsFull[3] = (object->transforms[3]);
		}
		if (rotate) {
			object->transformsFull[4] = (object->rotateQuat.x);
			object->transformsFull[5] = (object->rotateQuat.y);
			object->transformsFull[6] = (object->rotateQuat.z);
			object->transformsFull[7] = (object->rotateQuat.w);
		}
		if (translate || rotate) {
			object->transformsFull[8] = (object->boundInfo.x);
			object->transformsFull[9] = (object->boundInfo.y);
			object->transformsFull[10] = (object->boundInfo.z);
			object->transformsFull[11] = (object->boundInfo.w);
		}
	}
}

void Node::updateNode() {
	if (type != TYPE_ANIMATE) {
		updateNodeTransform();
		for (unsigned int i = 0; i < objects.size(); i++) {
			Object* object = objects[i];
			updateNodeObject(object, true, true);

			vec3 gPosition = GetTranslate(nodeTransform * object->translateMat * object->rotateMat);
			vec4 gQuat = vec4(object->rotateQuat);
			// todo update collision object
			//btTransform trans;
			//object->collisionObject->getMotionState()->getWorldTransform(trans);
			//trans.setRotation(gQuat);
			//trans.setOrigin(gPosition);
			//object->collisionObject->getMotionState()->setWorldTransform(trans);
		}
	}
	needUpdateNode = false;
}

void Node::pushToRemove() {
	Node::nodesToRemove.push_back(this);
}

void Node::recursiveTransform(mat4& finalNodeMatrix) {
	if (parent) {
		mat4 parentTransform;
		parent->recursiveTransform(parentTransform);
		finalNodeMatrix = parentTransform * translate(position.x, position.y, position.z);
	} else
		finalNodeMatrix = translate(position.x, position.y, position.z);
}

// Update node transform & its children's & children's children
void Node::updateNodeTransform() {
	recursiveTransform(nodeTransform);
	for (uint i = 0; i < children.size(); ++i)
		children[i]->updateNodeTransform();
}

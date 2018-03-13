#include "node.h"
#include "../util/util.h"
#include "../instance/instance.h"

std::vector<Node*> Node::nodesToUpdate;

Node::Node(const VECTOR3D& position,const VECTOR3D& size) {
	this->position.x=position.x;
	this->position.y=position.y;
	this->position.z=position.z;
	this->position.w=1;
	this->size.x=size.x;
	this->size.y=size.y;
	this->size.z=size.z;
	boundingBox=new AABB(position,size.x,size.y,size.z);
	objects.clear();
	objectsBBs.clear();
	drawcall=NULL;
	singleSide = false;
	needUpdateDrawcall = false;
	needCreateDrawcall = false;
	needUpdateNormal = false;
	needUpdateNode = false;
	uTransformMatrix = NULL;
	uNormalMatrix = NULL;

	parent=NULL;
	children.clear();
	nodeBBs.clear();

	type = TYPE_NULL;
}

Node::~Node() {
	if (uTransformMatrix) delete uTransformMatrix; 
	uTransformMatrix = NULL;
	if (uNormalMatrix) delete uNormalMatrix; 
	uNormalMatrix = NULL;

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
	if (boundingBox)
		return boundingBox->checkWithCamera(camera);
	return true;
}

// Update Object's bounding box from local to world
void Node::updateObjectBoundingInNode(Object* object) {
	BoundingBox* objectBB = object->bounding;
	if (objectBB) {
		MATRIX4X4 nodeMat; nodeMat.LoadIdentity();
		recursiveTransform(nodeMat);
		VECTOR4D localBB4(object->localBoundPosition.x, object->localBoundPosition.y, object->localBoundPosition.z, 1.0);
		VECTOR4D bb4 = nodeMat * localBB4;
		objectBB->update(VECTOR3D(bb4.x / bb4.w, bb4.y / bb4.w, bb4.z / bb4.w));
	}
}

void Node::addObject(Object* object) {
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
	pushToUpdate();
}

Object* Node::removeObject(Object* object) {
	std::vector<Object*>::iterator it;
	std::vector<BoundingBox*>::iterator itbb;
	for (it = objects.begin(); it != objects.end(); it++) {
		if ((*it) == object) {
			objects.erase(it);
			for (itbb = objectsBBs.begin(); itbb != objectsBBs.end(); itbb++) {
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
			pushToUpdate();

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
			MATRIX4X4 nodeTransform; nodeTransform.LoadIdentity();
			recursiveTransform(nodeTransform);
			boundingBox->update(nodeTransform * VECTOR4D(0, 0, 0, 1));
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
	VECTOR3D offset = VECTOR3D(dx, dy, dz);
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
		VECTOR3D offset = VECTOR3D(dx, dy, dz);
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
void Node::updateSelfAndDownwardNodesDrawcall(bool updateNormal) {
	if (objects.size() > 0) {
		needUpdateNormal = updateNormal;
		needUpdateDrawcall = true;
		pushToUpdate();
	}

	for (unsigned int i = 0; i < children.size(); i++)
		children[i]->updateSelfAndDownwardNodesDrawcall(updateNormal);
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

void Node::attachChild(Node* child) {
	children.push_back(child);
	child->parent=this;

	child->updateBaseNodeBounding();
	child->updateSelfAndDownwardNodesBounding();

	Node* superior = this;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}

	updateSelfAndDownwardNodesDrawcall(false);
}

Node* Node::detachChild(Node* child) {
	std::vector<Node*>::iterator it;
	for(it=children.begin();it!=children.end();it++) {
		if((*it)==child) {
			child->parent=NULL;
			children.erase(it);

			Node* superior = this;
			while (superior) {
				superior->updateBounding();
				superior = superior->parent;
			}
			
			if (child->type == TYPE_INSTANCE) {
				for (uint i = 0; i < child->objects.size(); i++)
					Instance::instanceTable[child->objects[i]->mesh]--;
			}
			return child;
		}
	}
	return NULL;
}

void Node::translateNode(float x, float y, float z) {
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

	updateSelfAndDownwardNodesDrawcall(false);
}

void Node::translateNodeObject(int i, float x, float y, float z) {
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
	pushToUpdate();
}

void Node::translateNodeObjectCenterAtWorld(int i, float x, float y, float z) {
	Object* object = objects[i];
	VECTOR3D worldCenter = object->bounding->position;
	VECTOR3D offset = VECTOR3D(x, y, z) - worldCenter;
	VECTOR3D localPosition = object->position;
	translateNodeObject(i, localPosition.x + offset.x, localPosition.y + offset.y, localPosition.z + offset.z);
}

void Node::rotateNodeObject(int i, float ax, float ay, float az) {
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
	pushToUpdate();
}

void Node::scaleNodeObject(int i, float sx, float sy, float sz) {
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
	pushToUpdate();
}

void Node::pushToUpdate() {
	if (!needUpdateNode) {
		Node::nodesToUpdate.push_back(this);
		needUpdateNode = true;
	}
}

void Node::updateNode() {
	if (type != TYPE_ANIMATE) {
		recursiveTransform(nodeTransform);
		for (unsigned int i = 0; i < objects.size(); i++) {
			Object* object = objects[i];
			object->transformMatrix = nodeTransform * object->localTransformMatrix;
		}
	}
	needUpdateNode = false;
}

void Node::recursiveTransform(MATRIX4X4& finalNodeMatrix) {
	if(parent) {
		MATRIX4X4 parentTransform;
		parent->recursiveTransform(parentTransform);
		finalNodeMatrix=parentTransform*translate(position.x,position.y,position.z);
	} else
		finalNodeMatrix=translate(position.x,position.y,position.z);
}

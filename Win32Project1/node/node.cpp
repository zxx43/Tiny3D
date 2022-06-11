#include "node.h"
#include "../util/util.h"
#include "../object/staticObject.h"
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

	staticObjects.clear();
	dynamicObjects.clear();
}

Node::~Node() {
	// clear children first
	nodeBBs.clear();
	clearChildren();

	if (parent) { // delete this node in parent node bounding list & children list
		std::vector<BoundingBox*>::iterator itbb = parent->nodeBBs.begin();
		while (itbb != parent->nodeBBs.end()) {
			if (*itbb == boundingBox) {
				parent->nodeBBs.erase(itbb);
				break;
			}
			++itbb;
		}
		std::vector<Node*>::iterator itchild = parent->children.begin();
		while (itchild != parent->children.end()) {
			if (*itchild == this) {
				parent->children.erase(itchild);
				break;
			}
			++itchild;
		}
		parent = NULL;
	}

	objectsBBs.clear();
	while (objects.size() > 0) {
		Object* object = objects[objects.size() - 1];
		delete removeObject(object);
	}
	objects.clear();

	cancelFromUpdate(); // do not update node to be deleted

	if (boundingBox)
		delete boundingBox;
	boundingBox = NULL;

	if(drawcall)
		delete drawcall;
	drawcall=NULL;

	staticObjects.clear();
	dynamicObjects.clear();
}

void Node::clearChildren() {
	for(unsigned int i=0;i<children.size();i++) {
		Node* child=children[i];
		delete child;
	}
	children.clear();
}

bool Node::checkInCamera(Camera* camera) {
	return checkInFrustum(camera->frustum);
}

bool Node::checkInSphere(Camera* camera) {
	if (boundingBox)
		return boundingBox->sphereWithCamera(camera->frustum);
	return true;
}

bool Node::checkInFrustum(Frustum* frustum) {
	if (boundingBox)
		return boundingBox->checkWithCamera(frustum, detailLevel);
	return true;
}

// Update Object's bounding box from local to world
void Node::updateObjectBoundingInNode(Object* object, bool nodeTransformed) {
	BoundingBox* objectBB = object->bounding;
	if (objectBB) {
		mat4 nodeMat; 
		if (!nodeTransformed)
			recursiveTransform(nodeMat);
		else
			nodeMat = nodeTransform;
		vec4 localBB4(object->localBoundPosition, 1.0);
		vec4 bb4 = nodeMat * localBB4;
		float invw = 1.0 / bb4.w;
		objectBB->update(vec3(bb4.x * invw, bb4.y * invw, bb4.z * invw));
	}
}

void Node::addObject(Scene* scene, Object* object) {
	object->belong = scene;
	object->parent = this;
	objects.push_back(object);
	object->caculateLocalAABB(true);
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
	pushToUpdate(object->belong);

	if (object->type == OBJ_TYPE_STATIC) {
		if (object->isDynamic()) dynamicObjects.push_back(object);
		else staticObjects.push_back(object);
	}
}

Object* Node::removeObject(Object* object) {
	std::vector<Object*>::iterator it;
	if (object->type == OBJ_TYPE_STATIC) {
		if (object->isDynamic()) {
			for (it = dynamicObjects.begin(); it != dynamicObjects.end(); ++it) {
				if ((*it) == object) {
					dynamicObjects.erase(it);
					break;
				}
			}
		} else {
			for (it = staticObjects.begin(); it != staticObjects.end(); ++it) {
				if ((*it) == object) {
					staticObjects.erase(it);
					break;
				}
			}
		}
	}

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
			pushToUpdate(object->belong);
			object->parent = NULL;
			object->belong = NULL;

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
	if (boundingBox && nodeBBs.size()>0)
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
	object->caculateLocalAABB(false);

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
	object->caculateLocalAABBFast(true);

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
	object->caculateLocalAABBFast(true);

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
	if (!needUpdateNode && type != TYPE_ANIMATE) {
		Node::nodesToUpdate.push_back(this);
		needUpdateNode = true;
	}
}

void Node::cancelFromUpdate() {
	std::vector<Node*>::iterator it = Node::nodesToUpdate.begin();
	while (it != Node::nodesToUpdate.end()) {
		if (*it == this) {
			needUpdateNode = false;
			Node::nodesToUpdate.erase(it);
			break;
		}
		++it;
	}
}

void Node::updateNode(const Scene* scene) {
	if (type != TYPE_ANIMATE) {
		updateNodeTransform();
		for (unsigned int i = 0; i < objects.size(); i++) {
			Object* object = objects[i];
			object->updateObjectTransform(true, true);

			if (object->collisionObject) {
				vec3 gPosition = GetTranslate(nodeTransform * object->translateMat);
				vec4 gQuat = object->rotateQuat;
				object->collisionObject->initTransform(gPosition, gQuat);
			}
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

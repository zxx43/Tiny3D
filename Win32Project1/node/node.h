/*
 * node.h
 *
 *  Created on: 2017-6-22
 *      Author: a
 */

#ifndef NODE_H_
#define NODE_H_

#include "../bounding/AABB.h"
#include "../camera/camera.h"
#include "../object/object.h"
#include "../render/drawcall.h"

class Node {
private:
	void updateObjectBoundingInNode(Object* object);
	void updateBaseNodeBounding();
	void updateSelfAndDownwardNodesBounding();
	void moveBaseObjectsBounding(float dx,float dy,float dz);
	void moveSelfAndDownwardNodesBounding(float dx,float dy,float dz);
	void updateSelfAndDownwardNodesDrawcall(bool updateNormal);
public:
	MATRIX4X4* uTransformMatrix;
	MATRIX4X4* uNormalMatrix;
public:
	VECTOR4D position;
	VECTOR3D size;
	BoundingBox* boundingBox;

	std::vector<Object*> objects;
	std::vector<BoundingBox*> objectsBBs;

	Node* parent;
	std::vector<Node*> children;
	std::vector<BoundingBox*> nodeBBs;

	Drawcall* drawcall;
	bool singleSide;
	bool needUpdateNormal;
	bool needUpdateDrawcall;
	bool needCreateDrawcall;

	Node(const VECTOR3D& position,const VECTOR3D& size);
	virtual ~Node();
	bool checkInCamera(Camera* camera);
	virtual void prepareDrawcall()=0;
	virtual void updateDrawcall(bool updateNormal) = 0;

	void updateBounding();
	virtual void addObject(Object* object);
	virtual Object* removeObject(Object* object);
	void attachChild(Node* child);
	Node* detachChild(Node* child);
	virtual void translateNode(float x, float y, float z);
	void translateNodeObject(int i, float x, float y, float z);
	void translateNodeObjectCenterAtWorld(int i, float x, float y, float z);
	void rotateNodeObject(int i, float ax, float ay, float az);
	void scaleNodeObject(int i, float sx, float sy, float sz);
	Node* getAncestor();
	void clearChildren();
	void recursiveTransform(MATRIX4X4& finalNodeMatrix);
};

#endif /* NODE_H_ */

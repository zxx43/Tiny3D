/*
 * node.h
 *
 *  Created on: 2017-6-22
 *      Author: a
 */

#ifndef NODE_H_
#define NODE_H_

#define TYPE_NULL 0
#define TYPE_STATIC 1
#define TYPE_TERRAIN 2
#define TYPE_WATER 3
#define TYPE_INSTANCE 4
#define TYPE_ANIMATE 5

#include "../bounding/AABB.h"
#include "../object/object.h"
#include "../render/drawcall.h"

class Scene;

class Node {
public:
	static std::vector<Node*> nodesToUpdate;
	static std::vector<Node*> nodesToRemove;
private:
	void updateObjectBoundingInNode(Object* object);
	void updateBaseNodeBounding();
	void updateSelfAndDownwardNodesBounding();
	void moveBaseObjectsBounding(float dx,float dy,float dz);
	void moveSelfAndDownwardNodesBounding(float dx,float dy,float dz);
	void updateSelfAndDownwardNodesDrawcall(Scene* scene, bool updateNormal);
public:
	vec3 position; // Local position
	vec3 size;
	int type;
	int shadowLevel, detailLevel;
	BoundingBox* boundingBox; // Bounding box in world space
	mat4 nodeTransform; // Global transform

	std::vector<Object*> objects;
	std::vector<BoundingBox*> objectsBBs;

	Node* parent;
	std::vector<Node*> children;
	std::vector<BoundingBox*> nodeBBs;

	Drawcall* drawcall;
	bool needUpdateNormal;
	bool needUpdateDrawcall;
	bool needCreateDrawcall;
	bool needUpdateNode;

	Node(const vec3& position,const vec3& size);
	virtual ~Node();
	bool checkInCamera(Camera* camera);
	bool checkInFrustum(Frustum* frustum);
	virtual void prepareDrawcall() = 0;
	virtual void updateRenderData() = 0;
	virtual void updateDrawcall() = 0;
	void updateNode();
	void updateNodeObject(Object* object, bool translate, bool rotate);
	virtual void pushToUpdate(Scene* scene);

	void updateBounding();
	virtual void addObject(Scene* scene, Object* object);
	virtual Object* removeObject(Scene* scene, Object* object);
	void attachChild(Scene* scene, Node* child);
	Node* detachChild(Node* child);
	virtual void translateNode(Scene* scene, float x, float y, float z);
	void translateNodeObject(Scene* scene, int i, float x, float y, float z);
	void translateNodeObjectCenterAtWorld(Scene* scene, int i, float x, float y, float z);
	void rotateNodeObject(Scene* scene, int i, float ax, float ay, float az);
	void scaleNodeObject(Scene* scene, int i, float sx, float sy, float sz);
	Node* getAncestor();
	void clearChildren();
	void pushToRemove();
	void recursiveTransform(mat4& finalNodeMatrix);
};

#endif /* NODE_H_ */

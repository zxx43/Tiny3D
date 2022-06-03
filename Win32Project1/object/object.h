/*
 * object.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "../mesh/mesh.h"
#include "../material/materialManager.h"
#include "../billboard/billboard.h"
#include "../bounding/aabb.h"
#include "../physics/dynamicWorld.h"
#include "../sound/soundManager.h"

#ifndef OBJ_TYPE_NONE
#define OBJ_TYPE_NONE	0
#define OBJ_TYPE_STATIC 1
#define OBJ_TYPE_ANIMAT 2
#endif

class Scene;
class Node;
class Object {
public:
	Scene* belong;
	Node* parent;
	vec3 position; // Object local position
	vec3 size;
	mat4 translateMat, rotateMat, scaleMat; // Local transform
	Mesh* mesh;
	Mesh* meshMid;
	Mesh* meshLow;
	uint groupid;
	int material;
	Billboard* billboard;
	int boardid;
	mat4 localTransformMatrix,normalMatrix; // Local transform
	mat4 transformMatrix,transformTransposed; // Global transform
	vec4 rotateQuat;
	vec4 boundInfo;
	float* transforms; // Global translate used in GPU
	buff* transformsFull; // Global transform used in GPU (translate, quat, bounding)
	BoundingBox* bounding; // Bounding box in world space
	BoxInfo* baseBounding; // Bounding info with scale and rotate
	vec3 boundCenter; // Bounding center in model space
	vec3 localBoundPosition; // Bounding position in node
	bool genShadow;
	int detailLevel;
	vec3 shapeOffset;
	CollisionShape* collisionShape;
	CollisionObject* collisionObject;
	float mass;
	bool dynamic;
	bool hasPhysic;
	bool debug;
	std::map<std::string, SoundObject*> sounds;
	int type;
public:
	Object();
	Object(const Object& rhs);
	virtual ~Object();
	virtual Object* clone()=0;
	void caculateLocalAABB(bool hasRotateScale);
	void caculateLocalAABBFast(bool hasRotateScale);
	void caculateCollisionShape(); // Caculate collision obb
	CollisionObject* initCollisionObject();
	void setShapeOffset(const vec3& off) { shapeOffset = off; }
	void removeCollisionObject();
	void initMatricesData();
	void updateLocalMatrices();
	virtual void vertexTransform()=0;
	virtual void normalTransform()=0;
	void bindMaterial(int mid);
	bool checkInCamera(Camera* camera);
	bool sphereInCamera(Camera* camera);
	virtual void setPosition(float x, float y, float z) = 0;
	virtual void setRotation(float ax, float ay, float az) = 0;
	virtual void setSize(float sx, float sy, float sz) = 0;
	void setBillboard(float sx, float sy, int mid);
	void updateObjectTransform(bool translate, bool rotate);
	void setMass(float m) { mass = m; }
	bool isDynamic() { return dynamic; }
	bool isPhysic() { return hasPhysic; }
	bool isDebug() { return debug; }
	void setPhysic(bool phy) { hasPhysic = phy; }
	void setDebug(bool db) { debug = db; }
	void setSound(const char* name, const char* path);
	SoundObject* getSound(const char* name);
	void playEffect(const char* name) { SoundObject* sound = getSound(name); if (sound) sound->play(); }
private:
	void updateSoundsPosition(const vec3& position);
};


#endif /* OBJECT_H_ */

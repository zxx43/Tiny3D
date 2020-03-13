#ifndef WATER_NODE_H_
#define WATER_NODE_H_

#include "staticNode.h"

class WaterNode :public StaticNode {
private:
	float centerX, centerZ;
private:
	void putCenter();
public:
	WaterNode(const vec3& position);
	virtual ~WaterNode();
	virtual void addObject(Scene* scene, Object* object);
	void moveWaterWithCamera(const Camera* camera);
};

#endif
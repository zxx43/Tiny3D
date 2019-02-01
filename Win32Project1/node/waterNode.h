#ifndef WATER_NODE_H_
#define WATER_NODE_H_

#include "staticNode.h"

class WaterNode :public StaticNode {
private:
	float centerX, centerZ;
private:
	void putCenter();
public:
	WaterNode(const VECTOR3D& position);
	virtual ~WaterNode();
	virtual void addObject(Object* object);
	void moveWaterWithCamera(Camera* camera);
};

#endif
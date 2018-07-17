#ifndef WATER_NODE_H_
#define WATER_NODE_H_

#include "staticNode.h"

class WaterNode :public StaticNode {
private:
	float centerX, centerZ;
public:
	WaterNode(const VECTOR3D& position);
	virtual ~WaterNode();
	void putCenter();
	void moveWaterWithCamera(Camera* camera);
};

#endif
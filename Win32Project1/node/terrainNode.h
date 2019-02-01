#ifndef TERRAIN_NODE_H_
#define TERRAIN_NODE_H_

#include "staticNode.h"
#include "../util/triangle.h"

class TerrainNode: public StaticNode {
public:
	std::vector<Triangle*> triangles;
	int blockCount, lineSize;
	VECTOR3D offset, offsize;
public:
	TerrainNode(const VECTOR3D& position);
	virtual ~TerrainNode();
	void prepareCollisionData();
	bool cauculateY(float x, float z, float& y);
	void standObjectsOnGround(Node* node);
};

#endif
#ifndef TERRAIN_NODE_H_
#define TERRAIN_NODE_H_

#include "staticNode.h"
#include "../mesh/terrain.h"
#include "../util/triangle.h"
#include "../render/terrainDrawcall.h"

class TerrainNode: public StaticNode {
public:
	std::vector<Triangle*> triangles;
	int blockCount, lineSize;
	vec3 offset, offsize;
public:
	TerrainNode(const vec3& position);
	virtual ~TerrainNode();
	void prepareCollisionData();
	void caculateBlock(float x, float z, int& bx, int& bz);
	bool cauculateY(int bx, int bz, float x, float z, float& y);
	void cauculateBlockIndices(int cx, int cz, int sizex, int sizez);
	void standObjectsOnGround(Scene* scene, Node* node);
	Terrain* getMesh() { return (Terrain*)(objects[0]->mesh); }
};

#endif
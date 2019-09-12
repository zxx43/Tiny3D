#include "terrainNode.h"
#include "../mesh/terrain.h"
#include "../object/staticObject.h"
#include "../util/util.h"
#include "animationNode.h"

TerrainNode::TerrainNode(const vec3& position) : StaticNode(position) {
	triangles.clear();
	blockCount = 0;
	lineSize = 0;
	offset = vec3(0, 0, 0);
	offsize = vec3(1, 1, 1);

	type = TYPE_TERRAIN;
}

TerrainNode::~TerrainNode() {
	for (uint i = 0; i < triangles.size(); i++)
		delete triangles[i];
	triangles.clear();
}

void TerrainNode::prepareCollisionData() {
	StaticObject* object = (StaticObject*)(objects[0]);
	offsize.x = object->sizex;
	offsize.y = object->sizey;
	offsize.z = object->sizez;
	offset.x = position.x + object->position.x;
	offset.y = position.y + object->position.y;
	offset.z = position.z + object->position.z;

	Terrain* mesh = (Terrain*)(objects[0]->mesh);
	blockCount = mesh->blockCount;
	lineSize = sqrt(blockCount);
	vec4* vertices = mesh->vertices;
	for (int i = 0; i < lineSize; i++) {
		for (int j = 0; j < lineSize; j++) {
			vec4 a = vertices[i*(lineSize + 1) + j];
			vec4 b = vertices[i*(lineSize + 1) + (j + 1)];
			vec4 c = vertices[(i + 1)*(lineSize + 1) + j];
			vec4 d = vertices[(i + 1)*(lineSize + 1) + (j + 1)];
			
			vec3 pa = offset + mul(offsize, vec3(a.x,a.y,a.z));
			vec3 pb = offset + mul(offsize, vec3(b.x,b.y,b.z));
			vec3 pc = offset + mul(offsize, vec3(c.x,c.y,c.z));
			vec3 pd = offset + mul(offsize, vec3(d.x,d.y,d.z));

			Triangle* t1 = new Triangle(pa, pb, pc);
			Triangle* t2 = new Triangle(pb, pd, pc);
			triangles.push_back(t1);
			triangles.push_back(t2);
		}
	}
}

bool TerrainNode::cauculateY(float x, float z, float& y) {
	float offx = x - offset.x;
	float offz = z - offset.z;
	offx /= offsize.x;
	offz /= offsize.z;
	float invStepSize = 1.0 / STEP_SIZE;
	int ix = (int)(offx*invStepSize);
	int iz = (int)(offz*invStepSize);
	if (ix < 0 || iz < 0) return false;
	if (ix < lineSize && iz < lineSize) {
		int ib = iz*lineSize + ix;
		int ita = ib * 2, itb = ib * 2 + 1;

		if ((uint)ita >= triangles.size() ||
			(uint)itb >= triangles.size()) 
				return false;

		Triangle* ta = triangles[ita];
		Triangle* tb = triangles[itb];
		vec2 p2d = vec2(x, z);
		if (ta->pointIsIn(p2d))
			y = ta->caculateY(x, z);
		else
			y = tb->caculateY(x, z);
		return true;
	}
	return false;
}

void TerrainNode::standObjectsOnGround(Node* node) {
	if (node->type == TYPE_TERRAIN) return;
	if (node->children.size() <= 0) {
		if (node->type == TYPE_ANIMATE) {
			AnimationNode* animNode = (AnimationNode*)node;
			vec3 worldCenter = animNode->boundingBox->position;
			this->cauculateY(worldCenter.x, worldCenter.z, worldCenter.y);
			worldCenter.y += ((AABB*)animNode->boundingBox)->sizey * 0.5;
			animNode->translateNodeCenterAtWorld(worldCenter.x, worldCenter.y, worldCenter.z);
		} else {
			for (uint i = 0; i < node->objects.size(); i++) {
				StaticObject* obj = (StaticObject*)node->objects[i];
				vec3 worldCenter = obj->bounding->position;
				this->cauculateY(worldCenter.x, worldCenter.z, worldCenter.y);
				worldCenter.y += ((AABB*)obj->bounding)->sizey * 0.47;
				node->translateNodeObjectCenterAtWorld(i, worldCenter.x, worldCenter.y, worldCenter.z);
			}
		}
	} else if (node->children.size() > 0) {
		for (uint c = 0; c < node->children.size(); c++)
			standObjectsOnGround(node->children[c]);
	}
}

#include "terrainNode.h"
#include "../mesh/terrain.h"
#include "../object/staticObject.h"
#include "../util/util.h"
#include "animationNode.h"

TerrainNode::TerrainNode(const VECTOR3D& position) : StaticNode(position) {
	triangles.clear();
	blockCount = 0;
	lineSize = 0;
	offset = VECTOR3D(0, 0, 0);
	offsize = VECTOR3D(1, 1, 1);
}

TerrainNode::~TerrainNode() {
	for (uint i = 0; i < triangles.size(); i++)
		delete triangles[i];
	triangles.clear();
}

void TerrainNode::prepareTriangles() {
	StaticObject* object = dynamic_cast<StaticObject*>(objects[0]);
	offsize.x = object->sizex;
	offsize.y = object->sizey;
	offsize.z = object->sizez;
	offset.x = position.x + object->position.x;
	offset.y = position.y + object->position.y;
	offset.z = position.z + object->position.z;

	Terrain* mesh = dynamic_cast<Terrain*>(objects[0]->mesh);
	blockCount = mesh->blockCount;
	lineSize = sqrt(blockCount);
	VECTOR4D* vertices = mesh->vertices;
	for (int i = 0; i < lineSize; i++) {
		for (int j = 0; j < lineSize; j++) {
			VECTOR4D a = vertices[i*(lineSize + 1) + j];
			VECTOR4D b = vertices[i*(lineSize + 1) + (j + 1)];
			VECTOR4D c = vertices[(i + 1)*(lineSize + 1) + j];
			VECTOR4D d = vertices[(i + 1)*(lineSize + 1) + (j + 1)];
			
			VECTOR3D pa = offset + mul(offsize, VECTOR3D(a.x,a.y,a.z));
			VECTOR3D pb = offset + mul(offsize, VECTOR3D(b.x,b.y,b.z));
			VECTOR3D pc = offset + mul(offsize, VECTOR3D(c.x,c.y,c.z));
			VECTOR3D pd = offset + mul(offsize, VECTOR3D(d.x,d.y,d.z));

			Triangle* t1 = new Triangle(pa, pb, pc);
			Triangle* t2 = new Triangle(pb, pd, pc);
			triangles.push_back(t1);
			triangles.push_back(t2);
		}
	}
}

float TerrainNode::cauculateY(float x, float z) {
	float offx = x - offset.x;
	float offz = z - offset.z;
	offx /= offsize.x;
	offz /= offsize.z;
	float invStepSize = 1.0 / STEP_SIZE;
	int ix = (int)(offx*invStepSize);
	int iz = (int)(offz*invStepSize);
	if (ix < lineSize && iz < lineSize) {
		int ib = iz*lineSize + ix;
		int ita = ib * 2, itb = ib * 2 + 1;
		Triangle* ta = triangles[ita];
		Triangle* tb = triangles[itb];
		VECTOR2D p2d = VECTOR2D(x, z);
		if (ta->pointIsIn(p2d))
			return ta->caculateY(x, z);
		else
			return tb->caculateY(x, z);
	}
	return 0.0;
}

void standObjectsOnGround(Node* node, TerrainNode* terrain) {
	if (node->children.size() <= 0) {
		AnimationNode* animNode = dynamic_cast<AnimationNode*>(node);
		if (animNode) {
			VECTOR3D worldCenter = animNode->boundingBox->position;
			worldCenter.y = terrain->cauculateY(worldCenter.x, worldCenter.z) + ((AABB*)animNode->boundingBox)->sizey * 0.5;
			animNode->translateNodeCenterAtWorld(worldCenter.x, worldCenter.y, worldCenter.z);
		} else {
			for (uint i = 0; i < node->objects.size(); i++) {
				StaticObject* obj = (StaticObject*)node->objects[i];
				VECTOR3D worldCenter = obj->bounding->position;
				worldCenter.y = terrain->cauculateY(worldCenter.x, worldCenter.z) + ((AABB*)obj->bounding)->sizey * 0.499;
				node->translateNodeObjectCenterAtWorld(i, worldCenter.x, worldCenter.y, worldCenter.z);
			}
		}
	} else if (node->children.size() > 0) {
		for (uint c = 0; c < node->children.size(); c++)
			standObjectsOnGround(node->children[c], terrain);
	}
}

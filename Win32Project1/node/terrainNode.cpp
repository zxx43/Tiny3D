#include "terrainNode.h"
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

	info = NULL;
	infoBuffer = NULL;
}

TerrainNode::~TerrainNode() {
	for (uint i = 0; i < triangles.size(); i++)
		delete triangles[i];
	triangles.clear();
	if (info) delete info;
	if (infoBuffer) delete infoBuffer;
}

void TerrainNode::prepareCollisionData() {
	StaticObject* object = (StaticObject*)(objects[0]);
	offsize = object->size;
	offset = vec3(position.x, position.y, position.z) + object->position;

	Terrain* mesh = (Terrain*)(objects[0]->mesh);
	blockCount = mesh->blockCount;
	lineSize = sqrt(blockCount);
	vec4* vertices = mesh->vertices;
	vec3* normals = mesh->normals;
	uint curIndex = 0;
	for (int i = 0; i < lineSize; i++) {
		for (int j = 0; j < lineSize; j++) {
			uint i0 = i * (lineSize + 1) + j;
			uint i1 = i * (lineSize + 1) + (j + 1);
			uint i2 = (i + 1)*(lineSize + 1) + j;
			uint i3 = (i + 1)*(lineSize + 1) + (j + 1);

			vec4 a = vertices[i0];
			vec4 b = vertices[i1];
			vec4 c = vertices[i2];
			vec4 d = vertices[i3];
			
			vec3 pa = offset + mul(offsize, vec3(a.x,a.y,a.z));
			vec3 pb = offset + mul(offsize, vec3(b.x,b.y,b.z));
			vec3 pc = offset + mul(offsize, vec3(c.x,c.y,c.z));
			vec3 pd = offset + mul(offsize, vec3(d.x,d.y,d.z));

			Triangle* t1 = new Triangle(pa, pb, pc);
			Triangle* t2 = new Triangle(pb, pd, pc);
			triangles.push_back(t1);
			triangles.push_back(t2);

			vec3 na = normals[i0];
			vec3 nb = normals[i1];
			vec3 nc = normals[i2];
			vec3 nd = normals[i3];

			vec4 ta = vec4(t1->normal.x, t1->normal.y, t1->normal.z, t1->pd);
			vec4 tb = vec4(t2->normal.x, t2->normal.y, t2->normal.z, t2->pd);

			mesh->initPoint(pa, na, ta, tb, curIndex);
			mesh->initPoint(pb, nb, ta, tb, curIndex);
			mesh->initPoint(pc, nc, ta, tb, curIndex);
			mesh->initPoint(pb, nb, ta, tb, curIndex);
			mesh->initPoint(pd, nd, ta, tb, curIndex);
			mesh->initPoint(pc, nc, ta, tb, curIndex);
		}
	}
}

void TerrainNode::caculateBlock(float x, float z, int& bx, int& bz) {
	float offx = x - offset.x;
	float offz = z - offset.z;
	offx /= offsize.x;
	offz /= offsize.z;
	float invStepSize = 1.0 / STEP_SIZE;
	bx = (int)(offx*invStepSize);
	bz = (int)(offz*invStepSize);
}

bool TerrainNode::cauculateY(int bx, int bz, float x, float z, float& y) {
	if (bx < 0 || bz < 0) return false;
	if (bx < lineSize && bz < lineSize) {
		int ib = bz * lineSize + bx;
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

void TerrainNode::cauculateBlockIndices(int bx, int bz, int sizex, int sizez) {
	static const int BORDER = 2;

	StaticObject* object = (StaticObject*)(objects[0]);
	int maxBlock = lineSize - 1 - BORDER;
	int left = bx - sizex;
	int right = bx + sizex;
	int bottom = bz - sizez;
	int top = bz + sizez;
	left = left < BORDER ? BORDER : left;
	right = right > maxBlock ? maxBlock : right;
	bottom = bottom < BORDER ? BORDER : bottom;
	top = top > maxBlock ? maxBlock : top;

	uint curIndex = 0, count = 0;
	Terrain* mesh = (Terrain*)object->mesh;
	for (int i = bottom; i <= top; i++) {
		for (int j = left; j < right; j++) {
			uint blockIndex = i * lineSize + j;
			uint* blockIndices = mesh->blockIndexMap[blockIndex];
			for (int b = 0; b < 6; b++) {
				uint trIndex = blockIndices[b];
				mesh->visualIndices[curIndex] = trIndex;
				++curIndex, ++count;
			}
		}
	}
	mesh->visualIndCount = count;
}

void TerrainNode::standObjectsOnGround(Scene* scene, Node* node) {
	if (node->type == TYPE_TERRAIN) return;
	if (node->children.size() <= 0) {
		if (node->type == TYPE_ANIMATE) {
			AnimationNode* animNode = (AnimationNode*)node;
			vec3 worldCenter = GetTranslate(animNode->nodeTransform);
			int bx, bz;
			this->caculateBlock(worldCenter.x, worldCenter.z, bx, bz);
			this->cauculateY(bx, bz, worldCenter.x, worldCenter.z, worldCenter.y);
			worldCenter.y += ((AABB*)animNode->boundingBox)->sizey * 0.45;
			animNode->translateNodeCenterAtWorld(scene, worldCenter);
		} else {
			for (uint i = 0; i < node->objects.size(); i++) {
				StaticObject* obj = (StaticObject*)node->objects[i];
				vec3 worldCenter = obj->bounding->position;
				int bx, bz;
				this->caculateBlock(worldCenter.x, worldCenter.z, bx, bz);
				this->cauculateY(bx, bz, worldCenter.x, worldCenter.z, worldCenter.y);
				worldCenter.y += ((AABB*)obj->bounding)->sizey * 0.4;
				node->translateNodeObjectCenterAtWorld(scene, i, worldCenter.x, worldCenter.y, worldCenter.z);
			}
		}
	} else if (node->children.size() > 0) {
		for (uint c = 0; c < node->children.size(); c++)
			standObjectsOnGround(scene, node->children[c]);
	}
}

void TerrainNode::createTerrainInfo() {
	StaticObject* terrain = (StaticObject*)objects[0];
	if (info) delete info;
	info = new TerrainInfo();
	info->trans = vec4(GetTranslate(terrain->transformMatrix), 1.0);
	info->scale = vec4(terrain->size, 1.0);
	info->info = vec4(STEP_SIZE, lineSize, MAP_SIZE, MAP_SIZE);

	int channelCount = 12;
	float* infoData = (float*)malloc(channelCount * sizeof(float));
	infoData[0] = info->trans.x;
	infoData[1] = info->trans.y;
	infoData[2] = info->trans.z;
	infoData[3] = info->trans.w;
	infoData[4] = info->scale.x;
	infoData[5] = info->scale.y;
	infoData[6] = info->scale.z;
	infoData[7] = info->scale.w;
	infoData[8] = info->info.x;
	infoData[9] = info->info.y;
	infoData[10] = info->info.z;
	infoData[11] = info->info.w;

	if (infoBuffer) delete infoBuffer;
	infoBuffer = new RenderBuffer(1, false);
	infoBuffer->setBufferData(GL_UNIFORM_BUFFER, 0, GL_FLOAT, 1, channelCount, GL_STATIC_DRAW, infoData);
	free(infoData);
}

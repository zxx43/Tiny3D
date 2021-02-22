/*
 * terrain.h
 *
 *  Created on: 2017-7-29
 *      Author: a
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "mesh.h"
#include "../constants/constants.h"
#include "../bounding/aabb.h"
#include <map>

#define MAP_SIZE 1024
#define	STEP_SIZE 4
#define CHUNK_SIZE 5
#define CHUNK_INDEX_COUNT (CHUNK_SIZE * CHUNK_SIZE * 6)

struct Chunk {
	std::vector<uint> indices;
	vec3 boundCenter;
	vec3 boundSize;
	AABB* bounding;
	Chunk() {
		bounding = NULL;
		indices.clear();
	}
	~Chunk() {
		if (bounding) delete bounding; bounding = NULL;
		indices.clear();
	}
	void genBounding(const float* vertices) {
		uint index = indices[0];
		float vx = vertices[index * 3 + 0];
		float vy = vertices[index * 3 + 1];
		float vz = vertices[index * 3 + 2];
		vec3 minVert = vec3(vx, vy, vz), maxVert = vec3(vx, vy, vz);
		for (int i = 1; i < CHUNK_INDEX_COUNT; i++) {
			index = indices[i];
			vx = vertices[index * 3 + 0];
			vy = vertices[index * 3 + 1];
			vz = vertices[index * 3 + 2];
			vec3 vertex(vx, vy, vz);
			minVert.x = minVert.x > vertex.x ? vertex.x : minVert.x;
			minVert.y = minVert.y > vertex.y ? vertex.y : minVert.y;
			minVert.z = minVert.z > vertex.z ? vertex.z : minVert.z;
			maxVert.x = maxVert.x < vertex.x ? vertex.x : maxVert.x;
			maxVert.y = maxVert.y < vertex.y ? vertex.y : maxVert.y;
			maxVert.z = maxVert.z < vertex.z ? vertex.z : maxVert.z;
		}
		bounding = new AABB(minVert, maxVert);
		boundCenter = bounding->position;
		boundSize = bounding->halfSize;
	}
};

class Terrain: public Mesh {
private:
	unsigned char* heightMap;
	int chunkStep, stepCount;
private:
	void loadHeightMap(const char* fileName);
	float getHeight(int px,int pz);
	vec3 caculateNormal(vec3 p1,vec3 p2,vec3 p3);
	vec3 normalize(vec3 n1,vec3 n2,vec3 n3,vec3 n4,vec3 n5,vec3 n6);
	vec3 getTerrainNormal(float x,float y,float z);
	vec3 getTerrainTangent(float x, float y, float z, float u, float v, float du, float dv);
	vec3 getTangent(const vec3& normal);
	void createChunks();
	virtual void initFaces();
public:
	int blockCount;
	std::map<uint, uint*> blockIndexMap;
	uint* visualIndices;
	uint visualIndCount;
	float* visualPoints;
	uint visualPointsSize;
	std::vector<Chunk*> chunks;
public:
	Terrain(const char* fileName);
	virtual ~Terrain();
public:
	unsigned char* getHeightMap() { return heightMap; }
	void initPoint(const vec3& p, const vec3& n, const vec4& t1, const vec4& t2, uint& index);
};

#endif /* TERRAIN_H_ */

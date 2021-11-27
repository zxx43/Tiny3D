#ifndef CHUNK_H_
#define CHUNK_H_

#include "../constants/constants.h"
#include "../bounding/aabb.h"

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
	void genBounding(const float* vertices, int chunkIndexCount) {
		uint index = indices[0];
		float vx = vertices[index * 3 + 0];
		float vy = vertices[index * 3 + 1];
		float vz = vertices[index * 3 + 2];
		vec3 minVert = vec3(vx, vy, vz), maxVert = vec3(vx, vy, vz);
		for (int i = 1; i < chunkIndexCount; i++) {
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

#endif

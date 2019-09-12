/*
 * terrain.h
 *
 *  Created on: 2017-7-29
 *      Author: a
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "mesh.h"

#define MAP_SIZE 1024
#define	STEP_SIZE 4

class Terrain: public Mesh {
private:
	unsigned char* heightMap;

	void loadHeightMap(const char* fileName);
	float getHeight(int px,int pz);
	vec3 caculateNormal(vec3 p1,vec3 p2,vec3 p3);
	vec3 normalize(vec3 n1,vec3 n2,vec3 n3,vec3 n4,vec3 n5,vec3 n6);
	vec3 getTerrainNormal(float x,float y,float z);
	vec3 getTerrainTangent(float x, float y, float z, float u, float v, float du, float dv);
	vec3 getTangent(const vec3& normal);
	virtual void initFaces();
public:
	int blockCount;
public:
	Terrain(const char* fileName);
	virtual ~Terrain();
};

#endif /* TERRAIN_H_ */

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
#define	STEP_SIZE 8

class Terrain: public Mesh {
private:
	unsigned char* heightMap;

	void loadHeightMap(const char* fileName);
	float getHeight(int px,int pz);
	VECTOR3D caculateNormal(VECTOR3D p1,VECTOR3D p2,VECTOR3D p3);
	VECTOR3D normalize(VECTOR3D n1,VECTOR3D n2,VECTOR3D n3,VECTOR3D n4,VECTOR3D n5,VECTOR3D n6);
	VECTOR3D getTerrainNormal(float x,float y,float z);
	virtual void initFaces();
public:
	int blockCount;
public:
	Terrain(const char* fileName);
	virtual ~Terrain();
};

#endif /* TERRAIN_H_ */

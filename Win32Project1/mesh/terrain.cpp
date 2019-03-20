#include "terrain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Terrain::Terrain(const char* fileName):Mesh() {
	heightMap=(unsigned char*)malloc(MAP_SIZE*MAP_SIZE*sizeof(unsigned char));
	loadHeightMap(fileName);
	blockCount=(MAP_SIZE-STEP_SIZE)*(MAP_SIZE-STEP_SIZE)/(STEP_SIZE*STEP_SIZE);
	vertexCount=MAP_SIZE*MAP_SIZE/(STEP_SIZE*STEP_SIZE);
	indexCount=6*blockCount;

	vertices=new VECTOR4D[vertexCount];
	normals=new VECTOR3D[vertexCount];
	texcoords=new VECTOR2D[vertexCount];
	materialids = NULL;
	indices = (int*)malloc(indexCount*sizeof(int));

	initFaces();
	caculateExData();
}

void Terrain::loadHeightMap(const char* fileName) {
	int nSize=MAP_SIZE*MAP_SIZE;
	FILE* file=NULL;
	file=fopen(fileName,"rb");
	if(file==NULL)
		return;
	fread(heightMap,1,nSize,file);
	if(ferror(file))
		return;
	fclose(file);
}

Terrain::~Terrain() {
	free(heightMap);
	heightMap=NULL;
}

float Terrain::getHeight(int px,int pz) {
	int x=px%MAP_SIZE;
	int z=pz%MAP_SIZE;
	if(!heightMap)
		return 0;
	float y=0;
	if(px>=0&&pz>=0)
		y=heightMap[x+(z*MAP_SIZE)];
	return y;
}

VECTOR3D Terrain::caculateNormal(VECTOR3D p1,VECTOR3D p2,VECTOR3D p3) {
	VECTOR3D l1(p2.x-p1.x, p2.y-p1.y, p2.z-p1.z);
	VECTOR3D l2(p3.x-p1.x, p3.y-p1.y, p3.z-p1.z);
	VECTOR3D normal(l1.y*l2.z - l1.z*l2.y,
			l1.z*l2.x - l1.x*l2.z,
			l1.x*l2.y - l1.y*l2.x);
	normal.Normalize();
	normal.x*=0.5;
	normal.y*=0.5;
	normal.z*=0.5;
	return normal;
}

VECTOR3D Terrain::normalize(VECTOR3D n1,VECTOR3D n2,VECTOR3D n3,
		VECTOR3D n4,VECTOR3D n5,VECTOR3D n6) {
	VECTOR3D result=n1+n2+n3+n4+n5+n6;
	result.Normalize();
	return result;
}

VECTOR3D Terrain::getTerrainNormal(float x,float y,float z) {
	VECTOR3D c1(x,y,z);
	VECTOR3D u1(x,getHeight(x,z+STEP_SIZE),z+STEP_SIZE);
	VECTOR3D r1(x+STEP_SIZE,getHeight(x+STEP_SIZE,z),z);
	VECTOR3D d1(x,getHeight(x,z-STEP_SIZE),z-STEP_SIZE);
	VECTOR3D l1(x-STEP_SIZE,getHeight(x-STEP_SIZE,z),z);
	VECTOR3D dl1(x-STEP_SIZE,getHeight(x-STEP_SIZE,z-STEP_SIZE),z-STEP_SIZE);
	VECTOR3D ur1(x+STEP_SIZE,getHeight(x+STEP_SIZE,z+STEP_SIZE),z+STEP_SIZE);

	VECTOR3D n1=caculateNormal(c1,l1,u1);
	VECTOR3D n2=caculateNormal(c1,ur1,r1);
	VECTOR3D n3=caculateNormal(c1,r1,d1);
	VECTOR3D n4=caculateNormal(c1,d1,dl1);
	VECTOR3D n5=caculateNormal(c1,dl1,l1);
	VECTOR3D n6=caculateNormal(c1,u1,ur1);
	VECTOR3D nf=normalize(n1,n2,n3,n4,n5,n6);
	return nf;
}

void Terrain::initFaces() {
	int currentVertex=0;
	int stepCount=(MAP_SIZE-STEP_SIZE)/STEP_SIZE;

	float x,y,z,u,v;
	for(int i = 0, row = 0; row < stepCount + 1; i += STEP_SIZE, row++) {
		for(int j = 0, col = 0; col < stepCount + 1; j += STEP_SIZE, col++) {
			x=j; z=i; y=getHeight(x,z);
			u=col/4.0; v=row/4.0;
			vertices[currentVertex]=VECTOR4D(x,y,z,1);
			normals[currentVertex]=getTerrainNormal(x,y,z);
			texcoords[currentVertex]=VECTOR2D(u,v);
			currentVertex++;
		}
	}

	int currentIndex=0, blockFirstIndex=0;
	int sideVertexCount=stepCount+1;
	for(int i=0;i<stepCount;i++) {
		for(int j=0;j<stepCount;j++) {
			indices[currentIndex]=blockFirstIndex; currentIndex++;
			indices[currentIndex]=blockFirstIndex+sideVertexCount; currentIndex++;
			indices[currentIndex]=blockFirstIndex+sideVertexCount+1; currentIndex++;
			indices[currentIndex]=blockFirstIndex; currentIndex++;
			indices[currentIndex]=blockFirstIndex+sideVertexCount+1; currentIndex++;
			indices[currentIndex]=blockFirstIndex+1; currentIndex++;
			if(j<stepCount-1)
				blockFirstIndex++;
			else
				blockFirstIndex+=2;
		}
	}
}


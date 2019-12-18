#include "terrain.h"
#include "../util/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Terrain::Terrain(const char* fileName):Mesh() {
	heightMap=(unsigned char*)malloc(MAP_SIZE*MAP_SIZE*sizeof(unsigned char));
	loadHeightMap(fileName);
	blockCount=(MAP_SIZE-STEP_SIZE)*(MAP_SIZE-STEP_SIZE)/(STEP_SIZE*STEP_SIZE);
	vertexCount=MAP_SIZE*MAP_SIZE/(STEP_SIZE*STEP_SIZE);
	indexCount=6*blockCount;

	vertices=new vec4[vertexCount];
	normals=new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords=new vec2[vertexCount];
	materialids = NULL;
	indices = (int*)malloc(indexCount*sizeof(int));

	visualIndices = (uint*)malloc(indexCount * sizeof(uint));
	memset(visualIndices, 0, indexCount * sizeof(uint));
	visualIndCount = 0;
	blockIndexMap.clear();

	visualPointsSize = indexCount * 16;
	visualPoints = (float*)malloc(visualPointsSize * sizeof(float));
	memset(visualPoints, 0, visualPointsSize * sizeof(float));

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

	std::map<uint, uint*>::iterator it = blockIndexMap.begin();
	while (it != blockIndexMap.end()) {
		free(it->second);
		++it;
	}
	blockIndexMap.clear();

	free(visualIndices);
	free(visualPoints);
}

float Terrain::getHeight(int px, int pz) {
	int x = px % MAP_SIZE;
	int z = pz % MAP_SIZE;
	if (!heightMap) return 0;
	float y = 0;
	if (px >= 0 && pz >= 0)
		y = heightMap[x + (z * MAP_SIZE)];
	return y;
}

vec3 Terrain::caculateNormal(vec3 p1,vec3 p2,vec3 p3) {
	vec3 l1(p2.x-p1.x, p2.y-p1.y, p2.z-p1.z);
	vec3 l2(p3.x-p1.x, p3.y-p1.y, p3.z-p1.z);
	vec3 normal(l1.y*l2.z - l1.z*l2.y,
			l1.z*l2.x - l1.x*l2.z,
			l1.x*l2.y - l1.y*l2.x);
	normal.Normalize();
	normal.x*=0.5;
	normal.y*=0.5;
	normal.z*=0.5;
	return normal;
}

vec3 Terrain::normalize(vec3 n1,vec3 n2,vec3 n3,
		vec3 n4,vec3 n5,vec3 n6) {
	vec3 result=n1+n2+n3+n4+n5+n6;
	result.Normalize();
	return result;
}

vec3 Terrain::getTerrainNormal(float x,float y,float z) {
	vec3 c1(x,y,z);
	vec3 u1(x,getHeight(x,z+STEP_SIZE),z+STEP_SIZE);
	vec3 r1(x+STEP_SIZE,getHeight(x+STEP_SIZE,z),z);
	vec3 d1(x,getHeight(x,z-STEP_SIZE),z-STEP_SIZE);
	vec3 l1(x-STEP_SIZE,getHeight(x-STEP_SIZE,z),z);
	vec3 dl1(x-STEP_SIZE,getHeight(x-STEP_SIZE,z-STEP_SIZE),z-STEP_SIZE);
	vec3 ur1(x+STEP_SIZE,getHeight(x+STEP_SIZE,z+STEP_SIZE),z+STEP_SIZE);

	vec3 n1=caculateNormal(c1,l1,u1);
	vec3 n2=caculateNormal(c1,ur1,r1);
	vec3 n3=caculateNormal(c1,r1,d1);
	vec3 n4=caculateNormal(c1,d1,dl1);
	vec3 n5=caculateNormal(c1,dl1,l1);
	vec3 n6=caculateNormal(c1,u1,ur1);
	vec3 nf=normalize(n1,n2,n3,n4,n5,n6);
	return nf;
}

vec3 Terrain::getTerrainTangent(float x, float y, float z, float u, float v, float du, float dv) {
	vec3 c1(x, y, z);
	vec3 u1(x, getHeight(x, z + STEP_SIZE), z + STEP_SIZE);
	vec3 r1(x + STEP_SIZE, getHeight(x + STEP_SIZE, z), z);
	vec3 d1(x, getHeight(x, z - STEP_SIZE), z - STEP_SIZE);
	vec3 l1(x - STEP_SIZE, getHeight(x - STEP_SIZE, z), z);
	vec3 dl1(x - STEP_SIZE, getHeight(x - STEP_SIZE, z - STEP_SIZE), z - STEP_SIZE);
	vec3 ur1(x + STEP_SIZE, getHeight(x + STEP_SIZE, z + STEP_SIZE), z + STEP_SIZE);

	vec2 c1t(u, v);
	vec2 u1t(u, v + dv);
	vec2 r1t(u + du, v);
	vec2 d1t(u, v - dv);
	vec2 l1t(x - du, v);
	vec2 dl1t(x - du, v - dv);
	vec2 ur1t(x + du, v + dv);

	vec3 tg1 = CaculateTangent(c1, l1, u1, c1t, l1t, u1t);
	vec3 tg2 = CaculateTangent(c1, ur1, r1, c1t, ur1t, r1t);
	vec3 tg3 = CaculateTangent(c1, r1, d1, c1t, r1t, d1t);
	vec3 tg4 = CaculateTangent(c1, d1, dl1, c1t, d1t, dl1t);
	vec3 tg5 = CaculateTangent(c1, dl1, l1, c1t, dl1t, l1t);
	vec3 tg6 = CaculateTangent(c1, u1, ur1, c1t, u1t, ur1t);
	vec3 tg = normalize(tg1, tg2, tg3, tg4, tg5, tg6);
	return tg;
}

vec3 Terrain::getTangent(const vec3& normal) {
	vec3 c1 = normal.CrossProduct(vec3(0.0, 0.0, 1.0));
	vec3 c2 = normal.CrossProduct(vec3(0.0, 1.0, 0.0));

	if (c1.GetSquaredLength() > c2.GetSquaredLength())
		return c1.GetNormalized();
	else
		return c2.GetNormalized();
}

void Terrain::initFaces() {
	int currentVertex = 0;
	int stepCount = (MAP_SIZE - STEP_SIZE) / STEP_SIZE;

	float x, y, z, u, v;
	for (int i = 0, row = 0; row < stepCount + 1; i += STEP_SIZE, row++) {
		for (int j = 0, col = 0; col < stepCount + 1; j += STEP_SIZE, col++) {
			x = j; z = i; y = getHeight(x, z);
			u = col; v = row;
			vertices[currentVertex] = vec4(x, y, z, 1);
			vec3 normal = getTerrainNormal(x, y, z);
			normals[currentVertex] = normal;
			//tangents[currentVertex] = getTerrainTangent(x, y, z, u, v, 0.25, 0.25);
			tangents[currentVertex] = getTangent(normal);
			texcoords[currentVertex] = vec2(u, v);
			currentVertex++;
		}
	}

	int currentIndex = 0, blockFirstIndex = 0;
	int sideVertexCount = stepCount + 1;
	for (int i = 0; i < stepCount; i++) {
		for (int j = 0; j < stepCount; j++) {
			uint* iArray = (uint*)malloc(6 * sizeof(uint));
			iArray[0] = blockFirstIndex;
			iArray[1] = blockFirstIndex + sideVertexCount;
			iArray[2] = blockFirstIndex + sideVertexCount + 1;
			iArray[3] = blockFirstIndex;
			iArray[4] = blockFirstIndex + sideVertexCount + 1;
			iArray[5] = blockFirstIndex + 1;

			indices[currentIndex++] = iArray[0];
			indices[currentIndex++] = iArray[1];
			indices[currentIndex++] = iArray[2];
			indices[currentIndex++] = iArray[3];
			indices[currentIndex++] = iArray[4];
			indices[currentIndex++] = iArray[5];

			if (j < stepCount - 1)
				blockFirstIndex++;
			else
				blockFirstIndex += 2;

			uint blockIndex = i * stepCount + j;
			blockIndexMap[blockIndex] = iArray;
		}
	}
}

void Terrain::initPoint(const vec3& p, const vec3& n, const vec4& t1, const vec4& t2, uint& index) {
	PushVec3(p, visualPoints, index);
	PushFloat(1.0, visualPoints, index);
	PushVec3(n, visualPoints, index);
	PushFloat(0.0, visualPoints, index);
	PushVec4(t1, visualPoints, index);
	PushVec4(t2, visualPoints, index);
}

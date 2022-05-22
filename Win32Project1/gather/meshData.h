#ifndef  MESH_DATA_H_
#define MESH_DATA_H_

#include "../constants/constants.h"
#include "../util/util.h"

class Mesh;
class Animation;

class MeshData {
public:
	float* vertexBuffer;
	half* normalBuffer;
	half* tangentBuffer;
	float* texcoordBuffer;
	float* texidBuffer;
	byte* colorBuffer;
	ushort* indexBuffer;
public:
	byte* boneids;
	half* weights;
public:
	bool isAnim;
	int meshCount;
	int indexCount, vertexCount;
public:
	MeshData();
	MeshData(int vSize, int iSize, bool isA);
	MeshData(Mesh* mesh);
	MeshData(Animation* anim);
	~MeshData();
public:
	void append(MeshData* data);
private:
	void createMeshBuffers();
	void createAnimBuffers();
	void releaseBuffers();
};

#endif // ! MESH_DATA_H_


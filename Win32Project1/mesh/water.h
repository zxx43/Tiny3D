#ifndef WATER_H_
#define WATER_H_

#include "mesh.h"
#include "chunk.h"

#ifndef WATER_CHUNK_SIZE
#define WATER_CHUNK_SIZE 32
#define WATER_CHUNK_INDEX_CNT (WATER_CHUNK_SIZE * WATER_CHUNK_SIZE * 6)
#endif

class Water: public Mesh {
private:
	int waterSize;
	float waterHeight;
	int chunkStep, stepCount;
public:
	std::vector<Chunk*> chunks;
private:
	virtual void initFaces();
	void createChunks();
public:
	Water(int size, float height);
	virtual ~Water();
};

#endif
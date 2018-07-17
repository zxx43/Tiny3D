#ifndef WATER_H_
#define WATER_H_

#include "mesh.h"

class Water: public Mesh {
private:
	int waterSize;
	float waterHeight;
private:
	virtual void initFaces();
public:
	Water(int size, float height);
	virtual ~Water();
};

#endif
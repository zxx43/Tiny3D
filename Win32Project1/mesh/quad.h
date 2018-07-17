/*
 * quad.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef QUAD_H_
#define QUAD_H_

#include "mesh.h"

class Quad: public Mesh {
private:
	float baseX, baseY, baseZ;
private:
	virtual void initFaces();
public:
	Quad();
	Quad(float sizex, float sizey, float sizez);
	Quad(const Quad& rhs);
	virtual ~Quad();
};

#endif /* QUAD_H_ */

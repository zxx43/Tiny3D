/*
 * box.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef BOX_H_
#define BOX_H_

#include "mesh.h"

class Box: public Mesh {
private:
	virtual void initFaces();
public:
	Box();
	Box(const Box& rhs);
	virtual ~Box();
};


#endif /* BOX_H_ */

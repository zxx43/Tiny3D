/*
 * mesh.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef MESH_H_
#define MESH_H_

#include "../maths/Maths.h"

class Mesh {
private:
	virtual void initFaces()=0;
public:
	int vertexCount,indexCount;
	VECTOR4D* vertices;
	VECTOR3D* normals;
	VECTOR2D* texcoords;
	int* materialids;
	int* indices;

	Mesh();
	Mesh(const Mesh& rhs);
	virtual ~Mesh();
};


#endif /* MESH_H_ */

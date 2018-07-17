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
	VECTOR3D* vertices3;
	VECTOR3D* normals;
	VECTOR4D* normals4;
	VECTOR2D* texcoords;
	int* materialids;
	int* indices;
	bool isBillboard;

	Mesh();
	Mesh(const Mesh& rhs);
	virtual ~Mesh();
	void caculateExData();
	void setIsBillboard(bool billboard);
};


#endif /* MESH_H_ */

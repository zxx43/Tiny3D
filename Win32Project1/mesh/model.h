/*
 * model.h
 *
 *  Created on: 2017-4-25
 *      Author: a
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "mesh.h"
#include "../model/objloader.h"
#include <vector>

class Model: public Mesh {
private:
	ObjLoader* loader;
private:
	virtual void initFaces();
	void correctVertices(const char* obj);
public:
	std::vector<int> mats;
public:
	Model(const char* obj, const char* mtl, int vt);
	Model(const Model& rhs);
	virtual ~Model();
	void loadModel(const char* obj,const char* mtl,int vt);
};

#endif /* MODEL_H_ */

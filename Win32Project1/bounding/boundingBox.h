/*
 * boundingBox.h
 *
 *  Created on: 2017-4-1
 *      Author: a
 */

#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "../maths/Maths.h"
#include "../camera/camera.h"
#include <vector>

class BoundingBox {
public:
	VECTOR3D position;

	BoundingBox() :position(VECTOR3D(0, 0, 0)) {}
	BoundingBox(const BoundingBox& rhs) {}
	virtual ~BoundingBox() {}
	virtual BoundingBox* clone()=0;
	virtual bool checkWithCamera(Frustum* frustum,int checkLevel)=0;
	virtual void update(const VECTOR3D& pos)=0;
	virtual void merge(const std::vector<BoundingBox*>& others)=0;
};

#endif /* BOUNDINGBOX_H_ */

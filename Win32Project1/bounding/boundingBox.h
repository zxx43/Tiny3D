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
	vec3 position;
public:
	BoundingBox() :position(vec3(0, 0, 0)) {}
	BoundingBox(const BoundingBox& rhs) {}
public:
	virtual ~BoundingBox() {}
	virtual BoundingBox* clone() = 0;
	virtual bool checkWithCamera(Frustum* frustum, int checkLevel) = 0;
	virtual bool sphereWithCamera(Frustum*) = 0;
	virtual void update(const vec3& pos) = 0;
	virtual void merge(const std::vector<BoundingBox*>& others) = 0;
};

#endif /* BOUNDINGBOX_H_ */

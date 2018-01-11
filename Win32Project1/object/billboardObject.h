/*
 * billboardObject.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef BILLBOARDOBJECT_H_
#define BILLBOARDOBJECT_H_

#include "object.h"

class BillboardObject: public Object {
public:
	BillboardObject(Mesh* mesh);
	BillboardObject(const BillboardObject& rhs);
	virtual ~BillboardObject();
	virtual BillboardObject* clone();
	virtual void caculateLocalAABB(bool looseWidth, bool looseAll);
	virtual void vertexTransform();
	virtual void normalTransform();
	virtual void setPosition(float x, float y, float z);
	virtual void setSize(float sx, float sy, float sz);
	virtual void setRotation(float ax, float ay, float az) {}
};


#endif /* BILLBOARDOBJECT_H_ */

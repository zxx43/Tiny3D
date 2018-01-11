/*
 * staticObject.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef STATICOBJECT_H_
#define STATICOBJECT_H_

#include "object.h"

class StaticObject: public Object {
public:
	float anglex,angley,anglez;

	StaticObject(Mesh* mesh);
	StaticObject(const StaticObject& rhs);
	virtual ~StaticObject();
	virtual StaticObject* clone();
	virtual void vertexTransform();
	virtual void normalTransform();
	virtual void setPosition(float x,float y,float z);
	virtual void setRotation(float ax, float ay, float az);
	virtual void setSize(float sx, float sy, float sz);
};


#endif /* STATICOBJECT_H_ */

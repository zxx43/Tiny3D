/*
 * staticObject.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef STATICOBJECT_H_
#define STATICOBJECT_H_

#include "object.h"

class Scene;
class StaticObject: public Object {
private:
	vec3 positionBefore;
public:
	StaticObject(Mesh* mesh);
	StaticObject(Mesh* mesh, Mesh* meshMid, Mesh* meshLow);
	StaticObject(const StaticObject& rhs);
	virtual ~StaticObject();
	virtual StaticObject* clone();
	virtual void vertexTransform();
	virtual void normalTransform();
	virtual void setPosition(float x,float y,float z);
	virtual void setRotation(float ax, float ay, float az);
	virtual void setSize(float sx, float sy, float sz);
	void translateAtWorld(const vec3& position);
	void rotateAtWorld(const vec4& q);
	void standOnGround(Scene* scene);
	void setDynamic(bool dyn) { dynamic = dyn; if (dynamic) setMass(100.0); }
};


#endif /* STATICOBJECT_H_ */

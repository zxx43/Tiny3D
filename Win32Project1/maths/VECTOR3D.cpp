//////////////////////////////////////////////////////////////////////////////////////////
//	VECTOR3D.cpp
//	Function definitions for 3d vector class
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//	Modified:	8th November 2002	-	Changed Constructor layout
//									-	Some speed Improvements
//									-	Corrected Lerp
//				17th December 2002	-	Converted from radians to degrees
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#include "Maths.h"

void VECTOR3D::Normalize()
{
	float length=GetLength();

	if(length==1 || length==0)			//return if length is 1 or 0
		return;

	float scalefactor = 1.0f/length;
	x *= scalefactor;
	y *= scalefactor;
	z *= scalefactor;
}

VECTOR3D VECTOR3D::GetNormalized() const
{
	VECTOR3D result(*this);

	result.Normalize();

	return result;
}

VECTOR3D VECTOR3D::GetRotatedX(double angle) const
{
	if(angle==0.0)
		return (*this);

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);

	return VECTOR3D(	x,
						y*cosAngle - z*sinAngle,
						y*sinAngle + z*cosAngle);
}

void VECTOR3D::RotateX(double angle)
{
	(*this)=GetRotatedX(angle);
}

VECTOR3D VECTOR3D::GetRotatedY(double angle) const
{
	if(angle==0.0)
		return (*this);

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);

	return VECTOR3D(	x*cosAngle + z*sinAngle,
						y,
						-x*sinAngle + z*cosAngle);
}

void VECTOR3D::RotateY(double angle)
{
	(*this)=GetRotatedY(angle);
}

VECTOR3D VECTOR3D::GetRotatedZ(double angle) const
{
	if(angle==0.0)
		return (*this);

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);
	
	return VECTOR3D(x*cosAngle - y*sinAngle,
					x*sinAngle + y*cosAngle,
					z);
}

void VECTOR3D::RotateZ(double angle)
{
	(*this)=GetRotatedZ(angle);
}

VECTOR3D VECTOR3D::GetRotatedAxis(double angle, const VECTOR3D & axis) const
{
	if(angle==0.0)
		return (*this);

	VECTOR3D u=axis.GetNormalized();

	VECTOR3D rotMatrixRow0, rotMatrixRow1, rotMatrixRow2;

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);
	float oneMinusCosAngle=1.0f-cosAngle;

	rotMatrixRow0.x=(u.x)*(u.x) + cosAngle*(1-(u.x)*(u.x));
	rotMatrixRow0.y=(u.x)*(u.y)*(oneMinusCosAngle) - sinAngle*u.z;
	rotMatrixRow0.z=(u.x)*(u.z)*(oneMinusCosAngle) + sinAngle*u.y;

	rotMatrixRow1.x=(u.x)*(u.y)*(oneMinusCosAngle) + sinAngle*u.z;
	rotMatrixRow1.y=(u.y)*(u.y) + cosAngle*(1-(u.y)*(u.y));
	rotMatrixRow1.z=(u.y)*(u.z)*(oneMinusCosAngle) - sinAngle*u.x;
	
	rotMatrixRow2.x=(u.x)*(u.z)*(oneMinusCosAngle) - sinAngle*u.y;
	rotMatrixRow2.y=(u.y)*(u.z)*(oneMinusCosAngle) + sinAngle*u.x;
	rotMatrixRow2.z=(u.z)*(u.z) + cosAngle*(1-(u.z)*(u.z));

	return VECTOR3D(	this->DotProduct(rotMatrixRow0),
						this->DotProduct(rotMatrixRow1),
						this->DotProduct(rotMatrixRow2));
}

void VECTOR3D::RotateAxis(double angle, const VECTOR3D & axis)
{
	(*this)=GetRotatedAxis(angle, axis);
}


void VECTOR3D::PackTo01()
{
	(*this)=GetPackedTo01();	
}

VECTOR3D VECTOR3D::GetPackedTo01() const
{
	VECTOR3D temp(*this);

	temp.Normalize();

	temp=temp*0.5f+VECTOR3D(0.5f, 0.5f, 0.5f);
	
	return temp;
}

VECTOR3D operator*(float scaleFactor, const VECTOR3D & rhs)
{
	return rhs*scaleFactor;
}

bool VECTOR3D::operator==(const VECTOR3D & rhs) const
{
	if(x==rhs.x && y==rhs.y && z==rhs.z)
		return true;

	return false;
}


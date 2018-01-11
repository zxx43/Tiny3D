//////////////////////////////////////////////////////////////////////////////////////////
//	VECTOR2D.cpp
//	Function definitions for 2d vector class
//	Downloaded from: www.paulsprojects.net
//	Created:	8th November 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#include "Maths.h"

void VECTOR2D::Normalize()
{
	float length;
	float scalefactor;
	length=GetLength();

	if(length==1 || length==0)			//return if length is 1 or 0
		return;

	scalefactor = 1.0f/length;
	x *= scalefactor;
	y *= scalefactor;
}

VECTOR2D VECTOR2D::GetNormalized() const
{
	VECTOR2D result(*this);

	result.Normalize();

	return result;
}

VECTOR2D operator*(float scaleFactor, const VECTOR2D & rhs)
{
	return rhs*scaleFactor;
}

bool VECTOR2D::operator==(const VECTOR2D & rhs) const
{
	if(x==rhs.x && y==rhs.y)
		return true;

	return false;
}




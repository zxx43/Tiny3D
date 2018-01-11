//////////////////////////////////////////////////////////////////////////////////////////
//	COLOR.cpp
//	function definitions for an RGBA color class
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//	Modified:	7th November 2002	-	Some speed improvements
//									-	Removed clamping after adds etc. Do it yourself!
//										To enable use with floating point color buffers
//									-	Corrected lerp (reversed f and 1-f)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#include "Maths.h"

void COLOR::ClampTo01()
{
	if(r>1.0f)
		r=1.0f;
	if(r<0.0f)
		r=0.0f;

	if(g>1.0f)
		g=1.0f;
	if(g<0.0f)
		g=0.0f;

	if(b>1.0f)
		b=1.0f;
	if(b<0.0f)
		b=0.0f;

	if(a>1.0f)
		a=1.0f;
	if(a<0.0f)
		a=0.0f;
}

COLOR operator*(float scaleFactor, const COLOR & rhs)
{
	return rhs*scaleFactor;
}

bool COLOR::operator ==(const COLOR & rhs) const
{
	if(r != rhs.r)
		return false;
	if(g != rhs.g)
		return false;
	if(b != rhs.b)
		return false;
	if(a != rhs.a)
		return false;

	return true;
}

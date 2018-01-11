//////////////////////////////////////////////////////////////////////////////////////////
//	Maths.h
//	Include maths headers
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//	Modified:	17th December 2002	-	Removed QUATERNION
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#include <math.h>

//VC++ math.h (and others) do not define M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//An epsilon value for comparisons
#ifndef EPSILON
#define EPSILON 0.01f
#endif

#include "VECTOR2D.h"
#include "VECTOR3D.h"
#include "VECTOR4D.h"
#include "MATRIX4X4.h"
#include "PLANE.h"

#include "COLOR.h"
//////////////////////////////////////////////////////////////////////////////////////////
//	MATRIX4X4.cpp
//	function definitions for 4x4 matrix class
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//	Updated:	19th August 2002	-	Corrected 2nd SetPerspective for n!=1.0f
//				26th September 2002	-	Added nudge to prevent artifacts with infinite far plane
//									-	Improved speed
//				7th November 2002	-	Added Affine Inverse functions
//									-	Changed constructors
//									-	Added special cases for row3 = (0, 0, 0, 1)
//				17th December 2002	-	Converted from radians to degrees for consistency
//										with OpenGL. Should have been done a long time ago...
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#include <memory.h>
#include "Maths.h"

MATRIX4X4::MATRIX4X4(float e0, float e1, float e2, float e3,
					float e4, float e5, float e6, float e7,
					float e8, float e9, float e10, float e11,
					float e12, float e13, float e14, float e15)
{
	entries[0]=e0;
	entries[1]=e1;
	entries[2]=e2;
	entries[3]=e3;
	entries[4]=e4;
	entries[5]=e5;
	entries[6]=e6;
	entries[7]=e7;
	entries[8]=e8;
	entries[9]=e9;
	entries[10]=e10;
	entries[11]=e11;
	entries[12]=e12;
	entries[13]=e13;
	entries[14]=e14;
	entries[15]=e15;
}

MATRIX4X4::MATRIX4X4(const MATRIX4X4 & rhs)
{
	memcpy(entries, rhs.entries, 16*sizeof(float));
}

MATRIX4X4::MATRIX4X4(const float * rhs)
{
	memcpy(entries, rhs, 16*sizeof(float));
}

void MATRIX4X4::SetEntry(int position, float value)
{
	if(position>=0 && position<=15)
		entries[position]=value;
}
	
float MATRIX4X4::GetEntry(int position) const
{
	if(position>=0 && position<=15)
		return entries[position];

	return 0.0f;
}

VECTOR4D MATRIX4X4::GetRow(int position) const
{
	if(position==0)
		return VECTOR4D(entries[0], entries[4], entries[8], entries[12]);
	
	if(position==1)
		return VECTOR4D(entries[1], entries[5], entries[9], entries[13]);
	
	if(position==2)
		return VECTOR4D(entries[2], entries[6], entries[10], entries[14]);
	
	if(position==3)
		return VECTOR4D(entries[3], entries[7], entries[11], entries[15]);

	return VECTOR4D(0.0f, 0.0f, 0.0f, 0.0f);
}

VECTOR4D MATRIX4X4::GetColumn(int position) const
{
	if(position==0)
		return VECTOR4D(entries[0], entries[1], entries[2], entries[3]);
	
	if(position==1)
		return VECTOR4D(entries[4], entries[5], entries[6], entries[7]);
	
	if(position==2)
		return VECTOR4D(entries[8], entries[9], entries[10], entries[11]);
	
	if(position==3)
		return VECTOR4D(entries[12], entries[13], entries[14], entries[15]);

	return VECTOR4D(0.0f, 0.0f, 0.0f, 0.0f);
}

void MATRIX4X4::LoadIdentity(void)
{
	memset(entries, 0, 16*sizeof(float));
	entries[0]=1.0f;
	entries[5]=1.0f;
	entries[10]=1.0f;
	entries[15]=1.0f;
}

void MATRIX4X4::LoadZero(void)
{
	memset(entries, 0, 16*sizeof(float));
}

MATRIX4X4 MATRIX4X4::operator+(const MATRIX4X4 & rhs) const		//overloaded operators
{
	return MATRIX4X4(	entries[0]+rhs.entries[0],
						entries[1]+rhs.entries[1],
						entries[2]+rhs.entries[2],
						entries[3]+rhs.entries[3],
						entries[4]+rhs.entries[4],
						entries[5]+rhs.entries[5],
						entries[6]+rhs.entries[6],
						entries[7]+rhs.entries[7],
						entries[8]+rhs.entries[8],
						entries[9]+rhs.entries[9],
						entries[10]+rhs.entries[10],
						entries[11]+rhs.entries[11],
						entries[12]+rhs.entries[12],
						entries[13]+rhs.entries[13],
						entries[14]+rhs.entries[14],
						entries[15]+rhs.entries[15]);
}

MATRIX4X4 MATRIX4X4::operator-(const MATRIX4X4 & rhs) const		//overloaded operators
{
	return MATRIX4X4(	entries[0]-rhs.entries[0],
						entries[1]-rhs.entries[1],
						entries[2]-rhs.entries[2],
						entries[3]-rhs.entries[3],
						entries[4]-rhs.entries[4],
						entries[5]-rhs.entries[5],
						entries[6]-rhs.entries[6],
						entries[7]-rhs.entries[7],
						entries[8]-rhs.entries[8],
						entries[9]-rhs.entries[9],
						entries[10]-rhs.entries[10],
						entries[11]-rhs.entries[11],
						entries[12]-rhs.entries[12],
						entries[13]-rhs.entries[13],
						entries[14]-rhs.entries[14],
						entries[15]-rhs.entries[15]);
}

MATRIX4X4 MATRIX4X4::operator*(const MATRIX4X4 & rhs) const
{
	//Optimise for matrices in which bottom row is (0, 0, 0, 1) in both matrices
	if(	entries[3]==0.0f && entries[7]==0.0f && entries[11]==0.0f && entries[15]==1.0f	&&
		rhs.entries[3]==0.0f && rhs.entries[7]==0.0f &&
		rhs.entries[11]==0.0f && rhs.entries[15]==1.0f)
	{
		return MATRIX4X4(	entries[0]*rhs.entries[0]+entries[4]*rhs.entries[1]+entries[8]*rhs.entries[2],
							entries[1]*rhs.entries[0]+entries[5]*rhs.entries[1]+entries[9]*rhs.entries[2],
							entries[2]*rhs.entries[0]+entries[6]*rhs.entries[1]+entries[10]*rhs.entries[2],
							0.0f,
							entries[0]*rhs.entries[4]+entries[4]*rhs.entries[5]+entries[8]*rhs.entries[6],
							entries[1]*rhs.entries[4]+entries[5]*rhs.entries[5]+entries[9]*rhs.entries[6],
							entries[2]*rhs.entries[4]+entries[6]*rhs.entries[5]+entries[10]*rhs.entries[6],
							0.0f,
							entries[0]*rhs.entries[8]+entries[4]*rhs.entries[9]+entries[8]*rhs.entries[10],
							entries[1]*rhs.entries[8]+entries[5]*rhs.entries[9]+entries[9]*rhs.entries[10],
							entries[2]*rhs.entries[8]+entries[6]*rhs.entries[9]+entries[10]*rhs.entries[10],
							0.0f,
							entries[0]*rhs.entries[12]+entries[4]*rhs.entries[13]+entries[8]*rhs.entries[14]+entries[12],
							entries[1]*rhs.entries[12]+entries[5]*rhs.entries[13]+entries[9]*rhs.entries[14]+entries[13],
							entries[2]*rhs.entries[12]+entries[6]*rhs.entries[13]+entries[10]*rhs.entries[14]+entries[14],
							1.0f);
	}

	//Optimise for when bottom row of 1st matrix is (0, 0, 0, 1)
	if(	entries[3]==0.0f && entries[7]==0.0f && entries[11]==0.0f && entries[15]==1.0f)
	{
		return MATRIX4X4(	entries[0]*rhs.entries[0]+entries[4]*rhs.entries[1]+entries[8]*rhs.entries[2]+entries[12]*rhs.entries[3],
							entries[1]*rhs.entries[0]+entries[5]*rhs.entries[1]+entries[9]*rhs.entries[2]+entries[13]*rhs.entries[3],
							entries[2]*rhs.entries[0]+entries[6]*rhs.entries[1]+entries[10]*rhs.entries[2]+entries[14]*rhs.entries[3],
							rhs.entries[3],
							entries[0]*rhs.entries[4]+entries[4]*rhs.entries[5]+entries[8]*rhs.entries[6]+entries[12]*rhs.entries[7],
							entries[1]*rhs.entries[4]+entries[5]*rhs.entries[5]+entries[9]*rhs.entries[6]+entries[13]*rhs.entries[7],
							entries[2]*rhs.entries[4]+entries[6]*rhs.entries[5]+entries[10]*rhs.entries[6]+entries[14]*rhs.entries[7],
							rhs.entries[7],
							entries[0]*rhs.entries[8]+entries[4]*rhs.entries[9]+entries[8]*rhs.entries[10]+entries[12]*rhs.entries[11],
							entries[1]*rhs.entries[8]+entries[5]*rhs.entries[9]+entries[9]*rhs.entries[10]+entries[13]*rhs.entries[11],
							entries[2]*rhs.entries[8]+entries[6]*rhs.entries[9]+entries[10]*rhs.entries[10]+entries[14]*rhs.entries[11],
							rhs.entries[11],
							entries[0]*rhs.entries[12]+entries[4]*rhs.entries[13]+entries[8]*rhs.entries[14]+entries[12]*rhs.entries[15],
							entries[1]*rhs.entries[12]+entries[5]*rhs.entries[13]+entries[9]*rhs.entries[14]+entries[13]*rhs.entries[15],
							entries[2]*rhs.entries[12]+entries[6]*rhs.entries[13]+entries[10]*rhs.entries[14]+entries[14]*rhs.entries[15],
							rhs.entries[15]);
	}

	//Optimise for when bottom row of 2nd matrix is (0, 0, 0, 1)
	if(	rhs.entries[3]==0.0f && rhs.entries[7]==0.0f &&
		rhs.entries[11]==0.0f && rhs.entries[15]==1.0f)
	{
		return MATRIX4X4(	entries[0]*rhs.entries[0]+entries[4]*rhs.entries[1]+entries[8]*rhs.entries[2],
							entries[1]*rhs.entries[0]+entries[5]*rhs.entries[1]+entries[9]*rhs.entries[2],
							entries[2]*rhs.entries[0]+entries[6]*rhs.entries[1]+entries[10]*rhs.entries[2],
							entries[3]*rhs.entries[0]+entries[7]*rhs.entries[1]+entries[11]*rhs.entries[2],
							entries[0]*rhs.entries[4]+entries[4]*rhs.entries[5]+entries[8]*rhs.entries[6],
							entries[1]*rhs.entries[4]+entries[5]*rhs.entries[5]+entries[9]*rhs.entries[6],
							entries[2]*rhs.entries[4]+entries[6]*rhs.entries[5]+entries[10]*rhs.entries[6],
							entries[3]*rhs.entries[4]+entries[7]*rhs.entries[5]+entries[11]*rhs.entries[6],
							entries[0]*rhs.entries[8]+entries[4]*rhs.entries[9]+entries[8]*rhs.entries[10],
							entries[1]*rhs.entries[8]+entries[5]*rhs.entries[9]+entries[9]*rhs.entries[10],
							entries[2]*rhs.entries[8]+entries[6]*rhs.entries[9]+entries[10]*rhs.entries[10],
							entries[3]*rhs.entries[8]+entries[7]*rhs.entries[9]+entries[11]*rhs.entries[10],
							entries[0]*rhs.entries[12]+entries[4]*rhs.entries[13]+entries[8]*rhs.entries[14]+entries[12],
							entries[1]*rhs.entries[12]+entries[5]*rhs.entries[13]+entries[9]*rhs.entries[14]+entries[13],
							entries[2]*rhs.entries[12]+entries[6]*rhs.entries[13]+entries[10]*rhs.entries[14]+entries[14],
							entries[3]*rhs.entries[12]+entries[7]*rhs.entries[13]+entries[11]*rhs.entries[14]+entries[15]);
	}	
	
	return MATRIX4X4(	entries[0]*rhs.entries[0]+entries[4]*rhs.entries[1]+entries[8]*rhs.entries[2]+entries[12]*rhs.entries[3],
						entries[1]*rhs.entries[0]+entries[5]*rhs.entries[1]+entries[9]*rhs.entries[2]+entries[13]*rhs.entries[3],
						entries[2]*rhs.entries[0]+entries[6]*rhs.entries[1]+entries[10]*rhs.entries[2]+entries[14]*rhs.entries[3],
						entries[3]*rhs.entries[0]+entries[7]*rhs.entries[1]+entries[11]*rhs.entries[2]+entries[15]*rhs.entries[3],
						entries[0]*rhs.entries[4]+entries[4]*rhs.entries[5]+entries[8]*rhs.entries[6]+entries[12]*rhs.entries[7],
						entries[1]*rhs.entries[4]+entries[5]*rhs.entries[5]+entries[9]*rhs.entries[6]+entries[13]*rhs.entries[7],
						entries[2]*rhs.entries[4]+entries[6]*rhs.entries[5]+entries[10]*rhs.entries[6]+entries[14]*rhs.entries[7],
						entries[3]*rhs.entries[4]+entries[7]*rhs.entries[5]+entries[11]*rhs.entries[6]+entries[15]*rhs.entries[7],
						entries[0]*rhs.entries[8]+entries[4]*rhs.entries[9]+entries[8]*rhs.entries[10]+entries[12]*rhs.entries[11],
						entries[1]*rhs.entries[8]+entries[5]*rhs.entries[9]+entries[9]*rhs.entries[10]+entries[13]*rhs.entries[11],
						entries[2]*rhs.entries[8]+entries[6]*rhs.entries[9]+entries[10]*rhs.entries[10]+entries[14]*rhs.entries[11],
						entries[3]*rhs.entries[8]+entries[7]*rhs.entries[9]+entries[11]*rhs.entries[10]+entries[15]*rhs.entries[11],
						entries[0]*rhs.entries[12]+entries[4]*rhs.entries[13]+entries[8]*rhs.entries[14]+entries[12]*rhs.entries[15],
						entries[1]*rhs.entries[12]+entries[5]*rhs.entries[13]+entries[9]*rhs.entries[14]+entries[13]*rhs.entries[15],
						entries[2]*rhs.entries[12]+entries[6]*rhs.entries[13]+entries[10]*rhs.entries[14]+entries[14]*rhs.entries[15],
						entries[3]*rhs.entries[12]+entries[7]*rhs.entries[13]+entries[11]*rhs.entries[14]+entries[15]*rhs.entries[15]);
}

MATRIX4X4 MATRIX4X4::operator*(const float rhs) const
{
	return MATRIX4X4(	entries[0]*rhs,
						entries[1]*rhs,
						entries[2]*rhs,
						entries[3]*rhs,
						entries[4]*rhs,
						entries[5]*rhs,
						entries[6]*rhs,
						entries[7]*rhs,
						entries[8]*rhs,
						entries[9]*rhs,
						entries[10]*rhs,
						entries[11]*rhs,
						entries[12]*rhs,
						entries[13]*rhs,
						entries[14]*rhs,
						entries[15]*rhs);
}

MATRIX4X4 MATRIX4X4::operator/(const float rhs) const
{
	if (rhs==0.0f || rhs==1.0f)
		return (*this);
		
	float temp=1/rhs;

	return (*this)*temp;
}

MATRIX4X4 operator*(float scaleFactor, const MATRIX4X4 & rhs)
{
	return rhs*scaleFactor;
}

bool MATRIX4X4::operator==(const MATRIX4X4 & rhs) const
{
	for(int i=0; i<16; i++)
	{
		if(entries[i]!=rhs.entries[i])
			return false;
	}
	return true;
}

bool MATRIX4X4::operator!=(const MATRIX4X4 & rhs) const
{
	return !((*this)==rhs);
}

void MATRIX4X4::operator+=(const MATRIX4X4 & rhs)
{
	(*this)=(*this)+rhs;
}

void MATRIX4X4::operator-=(const MATRIX4X4 & rhs)
{
	(*this)=(*this)-rhs;
}

void MATRIX4X4::operator*=(const MATRIX4X4 & rhs)
{
	(*this)=(*this)*rhs;
}

void MATRIX4X4::operator*=(const float rhs)
{
	(*this)=(*this)*rhs;
}

void MATRIX4X4::operator/=(const float rhs)
{
	(*this)=(*this)/rhs;
}

MATRIX4X4 MATRIX4X4::operator-(void) const
{
	MATRIX4X4 result(*this);

	for(int i=0; i<16; i++)
		result.entries[i]=-result.entries[i];

	return result;
}

VECTOR4D MATRIX4X4::operator*(const VECTOR4D rhs) const
{
	//Optimise for matrices in which bottom row is (0, 0, 0, 1)
	if(entries[3]==0.0f && entries[7]==0.0f && entries[11]==0.0f && entries[15]==1.0f)
	{
		return VECTOR4D(entries[0]*rhs.x
					+	entries[4]*rhs.y
					+	entries[8]*rhs.z
					+	entries[12]*rhs.w,

						entries[1]*rhs.x
					+	entries[5]*rhs.y
					+	entries[9]*rhs.z
					+	entries[13]*rhs.w,

						entries[2]*rhs.x
					+	entries[6]*rhs.y
					+	entries[10]*rhs.z
					+	entries[14]*rhs.w,

						rhs.w);
	}
	
	return VECTOR4D(	entries[0]*rhs.x
					+	entries[4]*rhs.y
					+	entries[8]*rhs.z
					+	entries[12]*rhs.w,

						entries[1]*rhs.x
					+	entries[5]*rhs.y
					+	entries[9]*rhs.z
					+	entries[13]*rhs.w,

						entries[2]*rhs.x
					+	entries[6]*rhs.y
					+	entries[10]*rhs.z
					+	entries[14]*rhs.w,

						entries[3]*rhs.x
					+	entries[7]*rhs.y
					+	entries[11]*rhs.z
					+	entries[15]*rhs.w);
}

VECTOR3D MATRIX4X4::GetRotatedVector3D(const VECTOR3D & rhs) const
{
	return VECTOR3D(entries[0]*rhs.x + entries[4]*rhs.y + entries[8]*rhs.z,
					entries[1]*rhs.x + entries[5]*rhs.y + entries[9]*rhs.z,
					entries[2]*rhs.x + entries[6]*rhs.y + entries[10]*rhs.z);
}

VECTOR3D MATRIX4X4::GetInverseRotatedVector3D(const VECTOR3D & rhs) const
{
	//rotate by transpose:
	return VECTOR3D(entries[0]*rhs.x + entries[1]*rhs.y + entries[2]*rhs.z,
					entries[4]*rhs.x + entries[5]*rhs.y + entries[6]*rhs.z,
					entries[8]*rhs.x + entries[9]*rhs.y + entries[10]*rhs.z);
}

VECTOR3D MATRIX4X4::GetTranslatedVector3D(const VECTOR3D & rhs) const
{
	return VECTOR3D(rhs.x+entries[12], rhs.y+entries[13], rhs.z+entries[14]);
}

VECTOR3D MATRIX4X4::GetInverseTranslatedVector3D(const VECTOR3D & rhs) const
{
	return VECTOR3D(rhs.x-entries[12], rhs.y-entries[13], rhs.z-entries[14]);
}

void MATRIX4X4::Invert(void)
{
	*this=GetInverse();
}

MATRIX4X4 MATRIX4X4::GetInverse(void) const
{
	MATRIX4X4 result=GetInverseTranspose();

	result.Transpose();

	return result;
}


void MATRIX4X4::Transpose(void)
{
	*this=GetTranspose();
}

MATRIX4X4 MATRIX4X4::GetTranspose(void) const
{
	return MATRIX4X4(	entries[ 0], entries[ 4], entries[ 8], entries[12],
						entries[ 1], entries[ 5], entries[ 9], entries[13],
						entries[ 2], entries[ 6], entries[10], entries[14],
						entries[ 3], entries[ 7], entries[11], entries[15]);
}

void MATRIX4X4::InvertTranspose(void)
{
	*this=GetInverseTranspose();
}

MATRIX4X4 MATRIX4X4::GetInverseTranspose(void) const
{
	MATRIX4X4 result;

	float tmp[12];												//temporary pair storage
	float det;													//determinant

	//calculate pairs for first 8 elements (cofactors)
	tmp[0] = entries[10] * entries[15];
	tmp[1] = entries[11] * entries[14];
	tmp[2] = entries[9] * entries[15];
	tmp[3] = entries[11] * entries[13];
	tmp[4] = entries[9] * entries[14];
	tmp[5] = entries[10] * entries[13];
	tmp[6] = entries[8] * entries[15];
	tmp[7] = entries[11] * entries[12];
	tmp[8] = entries[8] * entries[14];
	tmp[9] = entries[10] * entries[12];
	tmp[10] = entries[8] * entries[13];
	tmp[11] = entries[9] * entries[12];

	//calculate first 8 elements (cofactors)
	result.SetEntry(0,		tmp[0]*entries[5] + tmp[3]*entries[6] + tmp[4]*entries[7]
					-	tmp[1]*entries[5] - tmp[2]*entries[6] - tmp[5]*entries[7]);

	result.SetEntry(1,		tmp[1]*entries[4] + tmp[6]*entries[6] + tmp[9]*entries[7]
					-	tmp[0]*entries[4] - tmp[7]*entries[6] - tmp[8]*entries[7]);

	result.SetEntry(2,		tmp[2]*entries[4] + tmp[7]*entries[5] + tmp[10]*entries[7]
					-	tmp[3]*entries[4] - tmp[6]*entries[5] - tmp[11]*entries[7]);

	result.SetEntry(3,		tmp[5]*entries[4] + tmp[8]*entries[5] + tmp[11]*entries[6]
					-	tmp[4]*entries[4] - tmp[9]*entries[5] - tmp[10]*entries[6]);

	result.SetEntry(4,		tmp[1]*entries[1] + tmp[2]*entries[2] + tmp[5]*entries[3]
					-	tmp[0]*entries[1] - tmp[3]*entries[2] - tmp[4]*entries[3]);

	result.SetEntry(5,		tmp[0]*entries[0] + tmp[7]*entries[2] + tmp[8]*entries[3]
					-	tmp[1]*entries[0] - tmp[6]*entries[2] - tmp[9]*entries[3]);

	result.SetEntry(6,		tmp[3]*entries[0] + tmp[6]*entries[1] + tmp[11]*entries[3]
					-	tmp[2]*entries[0] - tmp[7]*entries[1] - tmp[10]*entries[3]);

	result.SetEntry(7,		tmp[4]*entries[0] + tmp[9]*entries[1] + tmp[10]*entries[2]
					-	tmp[5]*entries[0] - tmp[8]*entries[1] - tmp[11]*entries[2]);

	//calculate pairs for second 8 elements (cofactors)
	tmp[0] = entries[2]*entries[7];
	tmp[1] = entries[3]*entries[6];
	tmp[2] = entries[1]*entries[7];
	tmp[3] = entries[3]*entries[5];
	tmp[4] = entries[1]*entries[6];
	tmp[5] = entries[2]*entries[5];
	tmp[6] = entries[0]*entries[7];
	tmp[7] = entries[3]*entries[4];
	tmp[8] = entries[0]*entries[6];
	tmp[9] = entries[2]*entries[4];
	tmp[10] = entries[0]*entries[5];
	tmp[11] = entries[1]*entries[4];

	//calculate second 8 elements (cofactors)
	result.SetEntry(8,		tmp[0]*entries[13] + tmp[3]*entries[14] + tmp[4]*entries[15]
					-	tmp[1]*entries[13] - tmp[2]*entries[14] - tmp[5]*entries[15]);

	result.SetEntry(9,		tmp[1]*entries[12] + tmp[6]*entries[14] + tmp[9]*entries[15]
					-	tmp[0]*entries[12] - tmp[7]*entries[14] - tmp[8]*entries[15]);

	result.SetEntry(10,		tmp[2]*entries[12] + tmp[7]*entries[13] + tmp[10]*entries[15]
					-	tmp[3]*entries[12] - tmp[6]*entries[13] - tmp[11]*entries[15]);

	result.SetEntry(11,		tmp[5]*entries[12] + tmp[8]*entries[13] + tmp[11]*entries[14]
					-	tmp[4]*entries[12] - tmp[9]*entries[13] - tmp[10]*entries[14]);

	result.SetEntry(12,		tmp[2]*entries[10] + tmp[5]*entries[11] + tmp[1]*entries[9]
					-	tmp[4]*entries[11] - tmp[0]*entries[9] - tmp[3]*entries[10]);

	result.SetEntry(13,		tmp[8]*entries[11] + tmp[0]*entries[8] + tmp[7]*entries[10]
					-	tmp[6]*entries[10] - tmp[9]*entries[11] - tmp[1]*entries[8]);

	result.SetEntry(14,		tmp[6]*entries[9] + tmp[11]*entries[11] + tmp[3]*entries[8]
					-	tmp[10]*entries[11] - tmp[2]*entries[8] - tmp[7]*entries[9]);

	result.SetEntry(15,		tmp[10]*entries[10] + tmp[4]*entries[8] + tmp[9]*entries[9]
					-	tmp[8]*entries[9] - tmp[11]*entries[10] - tmp[5]*entries[8]);

	// calculate determinant
	det	=	 entries[0]*result.GetEntry(0)
			+entries[1]*result.GetEntry(1)
			+entries[2]*result.GetEntry(2)
			+entries[3]*result.GetEntry(3);

	if(det==0.0f)
	{
		MATRIX4X4 id;
		return id;
	}
	
	result=result/det;

	return result;
}

//Invert if only composed of rotations & translations
void MATRIX4X4::AffineInvert(void)
{
	(*this)=GetAffineInverse();
}

MATRIX4X4 MATRIX4X4::GetAffineInverse(void) const
{
	//return the transpose of the rotation part
	//and the negative of the inverse rotated translation part
	return MATRIX4X4(	entries[0],
						entries[4],
						entries[8],
						0.0f,
						entries[1],
						entries[5],
						entries[9],
						0.0f,
						entries[2],
						entries[6],
						entries[10],
						0.0f,
						-(entries[0]*entries[12]+entries[1]*entries[13]+entries[2]*entries[14]),
						-(entries[4]*entries[12]+entries[5]*entries[13]+entries[6]*entries[14]),
						-(entries[8]*entries[12]+entries[9]*entries[13]+entries[10]*entries[14]),
						1.0f);
}

void MATRIX4X4::AffineInvertTranspose(void)
{
	(*this)=GetAffineInverseTranspose();
}

MATRIX4X4 MATRIX4X4::GetAffineInverseTranspose(void) const
{
	//return the transpose of the rotation part
	//and the negative of the inverse rotated translation part
	//transposed
	return MATRIX4X4(	entries[0],
						entries[1],
						entries[2],
						-(entries[0]*entries[12]+entries[1]*entries[13]+entries[2]*entries[14]),
						entries[4],
						entries[5],
						entries[6],
						-(entries[4]*entries[12]+entries[5]*entries[13]+entries[6]*entries[14]),
						entries[8],
						entries[9],
						entries[10],
						-(entries[8]*entries[12]+entries[9]*entries[13]+entries[10]*entries[14]),
						0.0f, 0.0f, 0.0f, 1.0f);
}

void MATRIX4X4::SetTranslation(const VECTOR3D & translation)
{
	LoadIdentity();

	SetTranslationPart(translation);
}

void MATRIX4X4::SetScale(const VECTOR3D & scaleFactor)
{
	LoadIdentity();

	entries[0]=scaleFactor.x;
	entries[5]=scaleFactor.y;
	entries[10]=scaleFactor.z;
}

void MATRIX4X4::SetUniformScale(const float scaleFactor)
{
	LoadIdentity();

	entries[0]=entries[5]=entries[10]=scaleFactor;
}

void MATRIX4X4::SetRotationAxis(const double angle, const VECTOR3D & axis)
{
	VECTOR3D u=axis.GetNormalized();

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);
	float oneMinusCosAngle=1.0f-cosAngle;

	LoadIdentity();

	entries[0]=(u.x)*(u.x) + cosAngle*(1-(u.x)*(u.x));
	entries[4]=(u.x)*(u.y)*(oneMinusCosAngle) - sinAngle*u.z;
	entries[8]=(u.x)*(u.z)*(oneMinusCosAngle) + sinAngle*u.y;

	entries[1]=(u.x)*(u.y)*(oneMinusCosAngle) + sinAngle*u.z;
	entries[5]=(u.y)*(u.y) + cosAngle*(1-(u.y)*(u.y));
	entries[9]=(u.y)*(u.z)*(oneMinusCosAngle) - sinAngle*u.x;
	
	entries[2]=(u.x)*(u.z)*(oneMinusCosAngle) - sinAngle*u.y;
	entries[6]=(u.y)*(u.z)*(oneMinusCosAngle) + sinAngle*u.x;
	entries[10]=(u.z)*(u.z) + cosAngle*(1-(u.z)*(u.z));
}

void MATRIX4X4::SetRotationX(const double angle)
{
	LoadIdentity();

	entries[5]=(float)cos(M_PI*angle/180);
	entries[6]=(float)sin(M_PI*angle/180);

	entries[9]=-entries[6];
	entries[10]=entries[5];
}

void MATRIX4X4::SetRotationY(const double angle)
{
	LoadIdentity();

	entries[0]=(float)cos(M_PI*angle/180);
	entries[2]=-(float)sin(M_PI*angle/180);

	entries[8]=-entries[2];
	entries[10]=entries[0];
}

void MATRIX4X4::SetRotationZ(const double angle)
{
	LoadIdentity();

	entries[0]=(float)cos(M_PI*angle/180);
	entries[1]=(float)sin(M_PI*angle/180);

	entries[4]=-entries[1];
	entries[5]=entries[0];
}

void MATRIX4X4::SetRotationEuler(const double angleX, const double angleY, const double angleZ)
{
	LoadIdentity();

	SetRotationPartEuler(angleX, angleY, angleZ);
}

void MATRIX4X4::SetPerspective(	float left, float right, float bottom,
								float top, float n, float f)
{
	float nudge=0.999f;		//prevent artifacts with infinite far plane

	LoadZero();

	//check for division by 0
	if(left==right || top==bottom || n==f)
		return;

	entries[0]=(2*n)/(right-left);

	entries[5]=(2*n)/(top-bottom);

	entries[8]=(right+left)/(right-left);
	entries[9]=(top+bottom)/(top-bottom);

	if(f!=-1)
	{
		entries[10]=-(f+n)/(f-n);
	}
	else		//if f==-1, use an infinite far plane
	{
		entries[10]=-nudge;
	}

	entries[11]=-1;

	if(f!=-1)
	{
		entries[14]=-(2*f*n)/(f-n);
	}
	else		//if f==-1, use an infinite far plane
	{
		entries[14]=-2*n*nudge;
	}
}

void MATRIX4X4::SetPerspective(float fovy, float aspect, float n, float f)
{
	float left, right, top, bottom;

	//convert fov from degrees to radians
	fovy*=(float)M_PI/180;

	top=n*tanf(fovy/2.0f);
	bottom=-top;

	left=aspect*bottom;
	right=aspect*top;

	SetPerspective(left, right, bottom, top, n, f);
}

void MATRIX4X4::SetOrtho(	float left, float right, float bottom,
							float top, float n, float f)
{
	LoadIdentity();

	entries[0]=2.0f/(right-left);

	entries[5]=2.0f/(top-bottom);

	entries[10]=-2.0f/(f-n);

	entries[12]=-(right+left)/(right-left);
	entries[13]=-(top+bottom)/(top-bottom);
	entries[14]=-(f+n)/(f-n);
}

void MATRIX4X4::SetTranslationPart(const VECTOR3D & translation)
{
	entries[12]=translation.x;
	entries[13]=translation.y;
	entries[14]=translation.z;
}

void MATRIX4X4::SetRotationPartEuler(const double angleX, const double angleY, const double angleZ)
{
	double cr = cos( M_PI*angleX/180 );
	double sr = sin( M_PI*angleX/180 );
	double cp = cos( M_PI*angleY/180 );
	double sp = sin( M_PI*angleY/180 );
	double cy = cos( M_PI*angleZ/180 );
	double sy = sin( M_PI*angleZ/180 );

	entries[0] = ( float )( cp*cy );
	entries[1] = ( float )( cp*sy );
	entries[2] = ( float )( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	entries[4] = ( float )( srsp*cy-cr*sy );
	entries[5] = ( float )( srsp*sy+cr*cy );
	entries[6] = ( float )( sr*cp );

	entries[8] = ( float )( crsp*cy+sr*sy );
	entries[9] = ( float )( crsp*sy-sr*cy );
	entries[10] = ( float )( cr*cp );
}

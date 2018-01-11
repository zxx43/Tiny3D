//////////////////////////////////////////////////////////////////////////////////////////
//	MATRIX4X4.h
//	Class declaration for a 4x4 matrix
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

#ifndef MATRIX4X4_H
#define MATRIX4X4_H

class MATRIX4X4
{
public:
	MATRIX4X4()
	{	LoadIdentity();	}
	MATRIX4X4(	float e0, float e1, float e2, float e3,
				float e4, float e5, float e6, float e7,
				float e8, float e9, float e10, float e11,
				float e12, float e13, float e14, float e15);
	MATRIX4X4(const float * rhs);
	MATRIX4X4(const MATRIX4X4 & rhs);
	~MATRIX4X4() {}	//empty

	void SetEntry(int position, float value);
	float GetEntry(int position) const;
	VECTOR4D GetRow(int position) const;
	VECTOR4D GetColumn(int position) const;
	
	void LoadIdentity(void);
	void LoadZero(void);
	
	//binary operators
	MATRIX4X4 operator+(const MATRIX4X4 & rhs) const;
	MATRIX4X4 operator-(const MATRIX4X4 & rhs) const;
	MATRIX4X4 operator*(const MATRIX4X4 & rhs) const;
	MATRIX4X4 operator*(const float rhs) const;
	MATRIX4X4 operator/(const float rhs) const;
	friend MATRIX4X4 operator*(float scaleFactor, const MATRIX4X4 & rhs);

	bool operator==(const MATRIX4X4 & rhs) const;
	bool operator!=(const MATRIX4X4 & rhs) const;

	//self-add etc
	void operator+=(const MATRIX4X4 & rhs);
	void operator-=(const MATRIX4X4 & rhs);
	void operator*=(const MATRIX4X4 & rhs);
	void operator*=(const float rhs);
	void operator/=(const float rhs);

	//unary operators
	MATRIX4X4 operator-(void) const;
	MATRIX4X4 operator+(void) const {return (*this);}
	
	//multiply a vector by this matrix
	VECTOR4D operator*(const VECTOR4D rhs) const;

	//rotate a 3d vector by rotation part
	void RotateVector3D(VECTOR3D & rhs) const
	{rhs=GetRotatedVector3D(rhs);}

	void InverseRotateVector3D(VECTOR3D & rhs) const
	{rhs=GetInverseRotatedVector3D(rhs);}

	VECTOR3D GetRotatedVector3D(const VECTOR3D & rhs) const;
	VECTOR3D GetInverseRotatedVector3D(const VECTOR3D & rhs) const;

	//translate a 3d vector by translation part
	void TranslateVector3D(VECTOR3D & rhs) const
	{rhs=GetTranslatedVector3D(rhs);}

	void InverseTranslateVector3D(VECTOR3D & rhs) const
	{rhs=GetInverseTranslatedVector3D(rhs);}
	
	VECTOR3D GetTranslatedVector3D(const VECTOR3D & rhs) const;
	VECTOR3D GetInverseTranslatedVector3D(const VECTOR3D & rhs) const;

	//Other methods
	void Invert(void);
	MATRIX4X4 GetInverse(void) const;
	void Transpose(void);
	MATRIX4X4 GetTranspose(void) const;
	void InvertTranspose(void);
	MATRIX4X4 GetInverseTranspose(void) const;

	//Inverse of a rotation/translation only matrix
	void AffineInvert(void);
	MATRIX4X4 GetAffineInverse(void) const;
	void AffineInvertTranspose(void);
	MATRIX4X4 GetAffineInverseTranspose(void) const;

	//set to perform an operation on space - removes other entries
	void SetTranslation(const VECTOR3D & translation);
	void SetScale(const VECTOR3D & scaleFactor);
	void SetUniformScale(const float scaleFactor);
	void SetRotationAxis(const double angle, const VECTOR3D & axis);
	void SetRotationX(const double angle);
	void SetRotationY(const double angle);
	void SetRotationZ(const double angle);
	void SetRotationEuler(const double angleX, const double angleY, const double angleZ);
	void SetPerspective(float left, float right, float bottom, float top, float n, float f);
	void SetPerspective(float fovy, float aspect, float n, float f);
	void SetOrtho(float left, float right, float bottom, float top, float n, float f);

	//set parts of the matrix
	void SetTranslationPart(const VECTOR3D & translation);
	void SetRotationPartEuler(const double angleX, const double angleY, const double angleZ);
	void SetRotationPartEuler(const VECTOR3D & rotations)
	{
		SetRotationPartEuler((double)rotations.x, (double)rotations.y, (double)rotations.z);
	}

	//cast to pointer to a (float *) for glGetFloatv etc
	operator float* () const {return (float*) this;}
	operator const float* () const {return (const float*) this;}
	
	//member variables
	float entries[16];
};

#endif	//MATRIX4X4_H
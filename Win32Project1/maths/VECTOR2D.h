//////////////////////////////////////////////////////////////////////////////////////////
//	VECTOR2D.h
//	Class declaration for a 2d vector
//	Downloaded from: www.paulsprojects.net
//	Created:	6th November 2002
//	Modified:	7th January 2003	-	Added QuadraticInterpolate
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef VECTOR2D_H
#define VECTOR2D_H

class VECTOR2D
{
public:
	//constructors
	VECTOR2D(void)	:	x(0.0f), y(0.0f)
	{}

	VECTOR2D(float newX, float newY):	x(newX), y(newY)
	{}

	VECTOR2D(const float * rhs)		:	x(*rhs), y((*rhs)+1)
	{}

	VECTOR2D(const VECTOR2D & rhs)	:	x(rhs.x), y(rhs.y)
	{}

	~VECTOR2D() {}	//empty

	void Set(float newX, float newY)
	{	x=newX;	y=newY;	}
	
	//Accessors kept for compatibility
	void SetX(float newX) {x = newX;}
	void SetY(float newY) {y = newY;}

	float GetX() const {return x;}	//public accessor functions
	float GetY() const {return y;}	//inline, const

	void LoadZero(void);
	void LoadOne(void);					//fill with (1, 1)
	
	void Normalize();
	VECTOR2D GetNormalized() const;
	
	float GetLength() const
	{	return (float)sqrt((x*x)+(y*y));	}
	
	float GetSquaredLength() const
	{	return (x*x)+(y*y);	}

	//linear interpolate
	VECTOR2D lerp(const VECTOR2D & v2, float factor) const
	{	return (*this)*(1.0f-factor) + v2*factor;	}

	VECTOR2D QuadraticInterpolate(const VECTOR2D & v2, const VECTOR2D & v3, float factor) const
	{	return (*this)*(1.0f-factor)*(1.0f-factor) + 2*v2*factor*(1.0f-factor) + v3*factor*factor;}

	//overloaded operators
	//binary operators
	VECTOR2D operator+(const VECTOR2D & rhs) const
	{	return VECTOR2D(x + rhs.x, y + rhs.y);	}

	VECTOR2D operator-(const VECTOR2D & rhs) const
	{	return VECTOR2D(x - rhs.x, y - rhs.y);	}

	VECTOR2D operator*(const float rhs) const
	{	return VECTOR2D(x*rhs, y*rhs);	}
	
	VECTOR2D operator/(const float rhs) const
	{	return (rhs==0) ? VECTOR2D(0.0f, 0.0f) : VECTOR2D(x / rhs, y / rhs);	}

	//multiply by a float, eg 3*v
	friend VECTOR2D operator*(float scaleFactor, const VECTOR2D & rhs);

	bool operator==(const VECTOR2D & rhs) const;
	bool operator!=(const VECTOR2D & rhs) const
	{	return !((*this)==rhs);	}

	//self-add etc
	void operator+=(const VECTOR2D & rhs)
	{	x+=rhs.x;	y+=rhs.y;}

	void operator-=(const VECTOR2D & rhs)
	{	x-=rhs.x;	y-=rhs.y;}

	void operator*=(const float rhs)
	{	x*=rhs;	y*=rhs;	}

	void operator/=(const float rhs)
	{	if(rhs==0.0f)
			return;
		else
		{	x/=rhs; y/=rhs;	}
	}


	//unary operators
	VECTOR2D operator-(void) const {return VECTOR2D(-x, -y);}
	VECTOR2D operator+(void) const {return *this;}

	//cast to pointer to a (float *) for glVertex3fv etc
	operator float* () const {return (float*) this;}
	operator const float* () const {return (const float*) this;}

	//member variables
	float x;
	float y;
};

#endif	//VECTOR2D_H
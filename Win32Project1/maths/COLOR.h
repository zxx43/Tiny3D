//////////////////////////////////////////////////////////////////////////////////////////
//	COLOR.h
//	Class declaration for an RGBA color
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//	Modified:	7th November 2002	-	Some speed improvements
//									-	Removed clamping after adds etc. Do it yourself!
//										To enable use with floating point color buffers
//									-	Corrected lerp (reversed f and 1-f)
//				13th December 2002	-	Added default parameter to alpha of Set()
//									-	Added red, green, blue constant COLORs
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef COLOR_H
#define COLOR_H

class COLOR
{
public:
	//constructors
	COLOR()
	{	r=g=b=a=0.0f;	}

	COLOR(float newR, float newG, float newB, float newA=0.0f)
	{	r=newR;	g=newG;	b=newB;	a=newA;	}

	COLOR(const float * rhs)
	{	r=*rhs;	g=*(rhs+1);	b=*(rhs+2); a=*(rhs+3);	}

	COLOR(const COLOR & rhs)
	{	r=rhs.r;	g=rhs.g;	b=rhs.b;	a=rhs.a;}

	~COLOR() {}	//empty

	void Set(float newR, float newG, float newB, float newA=0.0f)
	{	r=newR;	g=newG;	b=newB;	a=newA;	}
	
	//accessors kept for compatability
	void SetR(float newR) {r = newR;}
	void SetG(float newG) {g = newG;}
	void SetB(float newB) {b = newB;}
	void SetA(float newA) {a = newA;}
	
	float GetR() const {return r;}	//public accessor functions
	float GetG() const {return g;}	//inline, const
	float GetB() const {return b;}
	float GetA() const {return a;}

	void ClampTo01(void);			//clamp all components to [0,1]

	void SetBlack(void) {r=g=b=a=1.0f;}
	void SetWhite(void) {r=g=b=a=0.0f;}
	void SetGrey(float shade) {r=g=b=a=shade;}

	//linear interpolate
	COLOR lerp(const COLOR & c2, float factor)
	{	return (*this)*(1.0f-factor) + c2*factor;	}

	//binary operators
	COLOR operator+(const COLOR & rhs) const
	{	return COLOR(r+rhs.r, g+rhs.g, b+rhs.b, a+rhs.a);	}

	COLOR operator-(const COLOR & rhs) const
	{	return COLOR(r-rhs.r, g-rhs.g, b-rhs.b, a-rhs.a);	}

	COLOR operator*(const COLOR & rhs) const
	{	return COLOR(r*rhs.r, g*rhs.g, b*rhs.b, a*rhs.a);	}

	COLOR operator/(const COLOR & rhs) const
	{	return COLOR(r/rhs.r, g/rhs.g, b/rhs.b, a/rhs.a);	}

	COLOR operator*(const float rhs) const
	{	return COLOR(r*rhs, g*rhs, b*rhs, a*rhs);	}

	COLOR operator/(const float rhs) const
	{	return COLOR(r/rhs, g/rhs, b/rhs, a/rhs);	}

	//multiply by a float, eg 3*c
	friend COLOR operator*(float scaleFactor, const COLOR & rhs);

	bool operator==(const COLOR & rhs) const;
	bool operator!=(const COLOR & rhs) const
	{	return !((*this)==rhs);	}

	//self-add etc
	COLOR operator+=(const COLOR & rhs)
	{	(*this)=(*this)+rhs;	return (*this);	}

	COLOR operator-=(const COLOR & rhs)
	{	(*this)=(*this)-rhs;	return (*this);	}

	COLOR operator*=(const COLOR & rhs)
	{	(*this)=(*this)*rhs;	return (*this);	}

	COLOR operator/=(const COLOR & rhs)
	{	(*this)=(*this)/rhs;	return (*this);	}

	COLOR operator*=(const float rhs)
	{	(*this)=(*this)*rhs;	return (*this);	}

	COLOR operator/=(const float rhs)
	{	(*this)=(*this)/rhs;	return (*this);	}

	//unary operators
	COLOR operator-(void) const {return COLOR(-r,-g, -b, -a);}
	COLOR operator+(void) const {return (*this);}

	//cast to pointer to float for glColor4fv etc
	operator float* () const {return (float*) this;}
	operator const float* () const {return (const float*) this;}

	//member variables
	float r;
	float g;
	float b;
	float a;
};

const COLOR white(1.0f, 1.0f, 1.0f, 1.0f);
const COLOR black(0.0f, 0.0f, 0.0f, 0.0f);

const COLOR red(1.0f, 0.0f, 0.0f, 1.0f);
const COLOR green(0.0f, 1.0f, 0.0f, 1.0f);
const COLOR blue(0.0f, 0.0f, 1.0f, 1.0f);

const COLOR cyan(0.0f, 1.0f, 1.0f, 1.0f);
const COLOR magenta(1.0f, 0.0f, 1.0f, 1.0f);
const COLOR yellow(1.0f, 1.0f, 0.0f, 1.0f);

#endif	//COLOR_H
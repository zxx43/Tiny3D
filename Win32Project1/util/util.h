/*
 * util.h
 *
 *  Created on: 2014-8-17
 *      Author: a
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "../maths/Maths.h"
#include "../constants/constants.h"
#include <stdio.h>

typedef ushort half;
typedef float buff;

mat4 lookAt(float eyeX, float eyeY, float eyeZ,
		float centerX, float centerY, float centerZ,
		float upX, float upY, float upZ);

mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up);

mat4 perspective(float fovy,float aspect,float zNear,float zFar);

mat4 ortho(float left, float right, float bottom, float top, float n, float f);

int project(float objX, float objY, float objZ,
		const float* model, const float* proj, const int* view,
		float* winX, float* winY, float* winZ);

mat4 rotateX(float angle);

mat4 rotateY(float angle);

mat4 rotateZ(float angle);

mat4 scale(float size);

mat4 scale(float sx, float sy, float sz);

mat4 scaleX(float size);

mat4 scaleY(float size);

mat4 scaleZ(float size);

mat4 translate(float tx,float ty,float tz);

mat4 translate(const vec3& t);

vec4 mul(const vec4& a,const vec4& b);

vec3 mul(const vec3& a, const vec3& b);

inline float angleToRadian(float angle) {
	return angle*A2R;
}

inline float radianToAngle(float radian) {
	return radian*R2A;
}

struct Line {
	vec3 dir;
	vec3 origin;
	Line(const vec3& ld, const vec3& lo) :dir(ld), origin(lo) {}
};

struct Plane {
	vec3 normal;
	float d;
	Plane() {}
	Plane(const vec3& pn, const float pd) :normal(pn), d(pd) {}
	void update(const vec3& pn, const float pd) {
		normal = pn;
		d = pd;
	}
};

bool CaculateIntersect(const Line* line, const Plane* plane, const float lineDistance, vec3& result);

vec3 CaculateTangent(const vec3& p0, const vec3& p1, const vec3& p2, const vec2& t0, const vec2& t1, const vec2& t2);
inline vec3 CaculateTangent(const vec4& p0, const vec4& p1, const vec4& p2, const vec2& t0, const vec2& t1, const vec2& t2) {
	vec3 p03(p0.x, p0.y, p0.z), p13(p1.x, p1.y, p1.z), p23(p2.x, p2.y, p2.z);
	return CaculateTangent(p03, p13, p23, t0, t1, t2);
}

inline void RestrictAngle(float& angle) {
	if (angle > 360.0) angle -= 360.0;
	else if (angle < 0.0) angle += 360.0;
}

inline float GetVec2(const vec2* vec2, int i) {
	float ret;
	switch (i) {
		case 0:
			ret = vec2->GetX();
			break;
		case 1:
			ret = vec2->GetY();
			break;
	}
	return ret;
}

inline float GetVec3(const vec3* vec3, int i) {
	float ret;
	switch (i) {
		case 0:
			ret = vec3->GetX();
			break;
		case 1:
			ret = vec3->GetY();
			break;
		case 2:
			ret = vec3->GetZ();
			break;
	}
	return ret;
}

inline float GetVec4(const vec4* vec4, int i) {
	float ret;
	switch (i) {
		case 0:
			ret = vec4->GetX();
			break;
		case 1:
			ret = vec4->GetY();
			break;
		case 2:
			ret = vec4->GetZ();
			break;
		case 3:
			ret = vec4->GetW();
			break;
	}
	return ret;
}

inline vec4 MatrixToQuat(const mat4& mat) {
	float tr = mat.entries[0] + mat.entries[5] + mat.entries[10];
	float qw, qx, qy, qz;
	if (tr > 0) {
		float S = sqrtf(tr + 1.0) * 2.0; // S=4*qw 
		float invS = 1.0 / S;
		qw = 0.25 * S;
		qx = (mat.entries[6] - mat.entries[9]) * invS;
		qy = (mat.entries[8] - mat.entries[2]) * invS;
		qz = (mat.entries[1] - mat.entries[4]) * invS;
	} else if ((mat.entries[0] > mat.entries[5])&(mat.entries[0] > mat.entries[10])) {
		float S = sqrtf(1.0 + mat.entries[0] - mat.entries[5] - mat.entries[10]) * 2.0; // S=4*qx 
		float invS = 1.0 / S;
		qw = (mat.entries[6] - mat.entries[9]) * invS;
		qx = 0.25 * S;
		qy = (mat.entries[4] + mat.entries[1]) * invS;
		qz = (mat.entries[8] + mat.entries[2]) * invS;
	} else if (mat.entries[5] > mat.entries[10]) {
		float S = sqrtf(1.0 + mat.entries[5] - mat.entries[0] - mat.entries[10]) * 2.0; // S=4*qy
		float invS = 1.0 / S;
		qw = (mat.entries[8] - mat.entries[2]) * invS;
		qx = (mat.entries[4] + mat.entries[1]) * invS;
		qy = 0.25 * S;
		qz = (mat.entries[9] + mat.entries[6]) * invS;
	} else {
		float S = sqrtf(1.0 + mat.entries[10] - mat.entries[0] - mat.entries[5]) * 2.0; // S=4*qz
		float invS = 1.0 / S;
		qw = (mat.entries[1] - mat.entries[4]) * invS;
		qx = (mat.entries[8] + mat.entries[2]) * invS;
		qy = (mat.entries[9] + mat.entries[6]) * invS;
		qz = 0.25 * S;
	}
	return vec4(qx, qy, qz, qw);
}

#define F16_EXPONENT_BITS 0x1F
#define F16_EXPONENT_SHIFT 10
#define F16_EXPONENT_BIAS 15
#define F16_MANTISSA_BITS 0x3ff
#define F16_MANTISSA_SHIFT (23 - F16_EXPONENT_SHIFT)
#define F16_MAX_EXPONENT (F16_EXPONENT_BITS << F16_EXPONENT_SHIFT)

inline half Float2Half(float value) {
	uint f32 = (*(uint *)&value);
	half f16 = 0;
	/* Decode IEEE 754 little-endian 32-bit floating-point value */
	int sign = (f32 >> 16) & 0x8000;
	/* Map exponent to the range [-127,128] */
	int exponent = ((f32 >> 23) & 0xff) - 127;
	int mantissa = f32 & 0x007fffff;
	if (exponent == 128) { /* Infinity or NaN */
		f16 = sign | F16_MAX_EXPONENT;
		if (mantissa) f16 |= (mantissa & F16_MANTISSA_BITS);
	}
	else if (exponent > 15) { /* Overflow - flush to Infinity */
		f16 = sign | F16_MAX_EXPONENT;
	}
	else if (exponent > -15) { /* Representable value */
		exponent += F16_EXPONENT_BIAS;
		mantissa >>= F16_MANTISSA_SHIFT;
		f16 = sign | exponent << F16_EXPONENT_SHIFT | mantissa;
	}
	else {
		f16 = sign;
	}
	return f16;
}

inline void Float2Halfv(float* value, half* hv, uint size) {
	for (uint i = 0; i < size; i++)
		hv[i] = Float2Half(value[i]);
}

inline void PushFloat(float v, float* array, uint& index) {
	array[index++] = v;
}

inline void PushVec3(const vec3& v, float* array, uint& index) {
	array[index++] = v.x;
	array[index++] = v.y;
	array[index++] = v.z;
}

inline void PushVec4(const vec4& v, float* array, uint& index) {
	array[index++] = v.x;
	array[index++] = v.y;
	array[index++] = v.z;
	array[index++] = v.w;
}

template <typename T>
struct TBuffer {
	T* tdata;
	uint size;
	TBuffer(uint n) {
		size = n;
		tdata = (T*)malloc(size * sizeof(T));
		memset(tdata, 0, size * sizeof(T));
	}
	~TBuffer() {
		if(tdata) free(tdata);
		tdata = NULL;
	}
	void resize(uint n) {
		if (n <= size) {
			size = n;
			return;
		}
		T* tmp = (T*)malloc(n * sizeof(T));
		memset(tmp, 0, (n * sizeof(T)));
		memcpy(tmp, tdata, size * sizeof(T));
		free(tdata);
		tdata = tmp;
		size = n;
	}
	void set(T v, uint i) {
		asset(i > size - 1);
		tdata[i] = v;
	}
	T get(uint i) {
		return tdata[i];
	}
	T* data() {
		return tdata;
	}
};

struct Indirect {
	uint count;
	uint primCount;
	uint firstIndex;
	uint baseVertex;
	uint baseInstance;
};

#endif /* UTIL_H_ */

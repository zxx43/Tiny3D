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

mat4 viewMat(const vec3& x, const vec3& y, const vec3& z, const vec3& position);

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

inline vec3 GetAxisX(const mat4& mat) {
	vec3 res = vec3(mat.entries[0], mat.entries[1], mat.entries[2]);
	return res;
}

inline vec3 GetAxisY(const mat4& mat) {
	vec3 res = vec3(mat.entries[4], mat.entries[5], mat.entries[6]);
	return res;
}

inline vec3 GetAxisZ(const mat4& mat) {
	vec3 res = vec3(mat.entries[8], mat.entries[9], mat.entries[10]);
	return res;
}

inline float angleToRadian(float angle) {
	return angle * A2R;
}

inline vec3 angleToRadian(const vec3& angle) {
	return angle * A2R;
}

inline float radianToAngle(float radian) {
	return radian * R2A;
}

inline vec3 radianToAngle(const vec3& radian) {
	return radian * R2A;
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
	if (angle >= 360.0) angle -= 360.0;
	else if (angle < 0.0) angle += 360.0;
}

inline void RestrictYAngle(float& angle) {
	if (angle > 180.0) angle -= 360.0;
	else if (angle < -180.0) angle += 360.0;
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

inline mat4 QuatToMatrix(const vec4& q) {
	vec4 m0 = vec4(
		1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z,
		2.0 * q.x * q.y + 2.0 * q.w * q.z,
		2.0 * q.x * q.z - 2.0 * q.w * q.y,
		0.0);
	vec4 m1 = vec4(
		2.0 * q.x * q.y - 2.0 * q.w * q.z,
		1.0 - 2.0 * q.x * q.x - 2.0 * q.z * q.z,
		2.0 * q.y * q.z + 2.0 * q.w * q.x,
		0.0);
	vec4 m2 = vec4(
		2.0 * q.x * q.z + 2.0 * q.w * q.y,
		2.0 * q.y * q.z - 2.0 * q.w * q.x,
		1.0 - 2.0 * q.x * q.x - 2.0 * q.y * q.y,
		0.0);
	vec4 m3 = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 mat = mat4(m0, m1, m2, m3);
	return mat;
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

inline void SetVec2(const vec2& v, float* array, uint index) {
	array[index * 2 + 0] = v.x;
	array[index * 2 + 1] = v.y;
}

inline void SetVec3(const vec3& v, float* array, uint index) {
	array[index * 3 + 0] = v.x;
	array[index * 3 + 1] = v.y;
	array[index * 3 + 2] = v.z;
}

inline void SetVec4(const vec4& v, float* array, uint index) {
	array[index * 4 + 0] = v.x;
	array[index * 4 + 1] = v.y;
	array[index * 4 + 2] = v.z;
	array[index * 4 + 3] = v.w;
}

inline void SetUVec4(const vec4& v, byte* array, uint index) {
	array[index * 4 + 0] = (byte)v.x;
	array[index * 4 + 1] = (byte)v.y;
	array[index * 4 + 2] = (byte)v.z;
	array[index * 4 + 3] = (byte)v.w;
}

inline vec3 GetTranslate(const mat4& mat) {
	return vec3(mat.entries[12], mat.entries[13], mat.entries[14]);
}

inline mat4 GetRotateAndScale(const mat4& mat) {
	mat4 res = mat;
	res.entries[12] = 0.0, res.entries[13] = 0.0, res.entries[14] = 0.0;
	return res;
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

template <typename T>
struct CirQueue {
	T* data;
	int size, max;
	int front, rear;
	CirQueue(int s) {
		max = s, size = 0;
		front = 0, rear = -1;
		data = (T*)malloc(max * sizeof(T));
		memset(data, 0, max * sizeof(T));
	}
	~CirQueue() {
		free(data);
	}
	void push(T value) {
		if (size < max) {
			size++;
			rear++;
		} else {
			size = max;
			front = (front + 1) % size;
			rear = (rear + 1) % size;
		}
		data[rear] = value;
	}
};

struct Indirect {
	uint count;
	uint primCount;
	uint firstIndex;
	uint baseVertex;
	uint baseInstance;
};

struct ConfigArg {
	int width;
	int height;
	bool fullscreen;
	bool dualthread;
	bool dualqueue;
	int smoothframe;
	int graphQuality;
	int shadowQuality;
	bool ssr;
	bool dof;
	bool fxaa;
	bool bloom;
	bool dynsky;
	bool cartoon;
	bool debug;
};

#endif /* UTIL_H_ */

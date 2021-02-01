#extension GL_ARB_bindless_texture : enable 

const float PI = 3.1415926;
const float INV_PI = 0.318309891613572;
const vec4 RAND_FACTOR = vec4(12.9898, 78.233, 45.164, 94.673);
const vec4 DefaultRM = vec4(0.0, 0.0, 0.0, 1.0);
const vec3 MatScale = vec3(0.6, 1.2, 1.0);
const vec3 COLOR_SCALE = vec3(0.004, 0.008, 0.005);
const vec3 GAMMA = vec3(2.2);
const vec3 INV_GAMMA = vec3(0.4546);
const float LOG2 = float(1.442695);
const vec4 CenterPosition = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 FAIL_COLOR = vec4(1.0, 1.0, 1.0, 0.0);
const vec3 UP_VEC3 = vec3(0.0, 1.0, 0.0);
const vec3 ZERO_VEC3 = vec3(0.0);
const vec4 BoardMat = vec4(0.3, 0.0, 0.0, 1.0);
const vec4 BoardRM = vec4(0.0, 0.0, 0.0, 1.0);
const uint MAX_TEX = 64;

mat3 RotY(float r) {
	float cosR = cos(r);
	float sinR = sin(r);
	return mat3(
		cosR, 0.0, -sinR,
		0.0,  1.0, 0.0,
		sinR, 0.0, cosR
	);
}

mat4 RotY4(float r) {
	float cosR = cos(r);
	float sinR = sin(r);
	return mat4(
		cosR, 0.0, -sinR, 0.0,
		0.0,  1.0, 0.0,   0.0,
		sinR, 0.0, cosR,  0.0,
		0.0,  0.0, 0.0,   1.0
	);
}

mat4 M3ToM4(mat3 m) {
	return mat4(
		vec4(m[0], 0.0),
		vec4(m[1], 0.0),
		vec4(m[2], 0.0),
		vec4(0.0, 0.0, 0.0, 1.0)
	);
}

mat4 convertMat(mat3x4 srcMat) {
	mat4x3 transMat = transpose(srcMat);
	return mat4(transMat[0], 0.0, 
				transMat[1], 0.0, 
				transMat[2], 0.0, 
				transMat[3], 1.0);
}

float random(vec3 seed, float i){
	vec4 seed4 = vec4(seed, i);
	float dotProduct = dot(seed4, RAND_FACTOR);
	return fract(sin(dotProduct) * 43758.5453);
}

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

vec2 saturate(vec2 value) {
	return clamp(value, 0.0, 1.0);
}

float BlendVal(float val, float val0, float val1, float res0, float res1) {
	if (val <= val0) return res0;
	if (val >= val1) return res1;
	return res0 + (val - val0) * (res1 - res0) / (val1 - val0);
}

mat3 GetTBN(vec3 normal, vec3 tangent) {
	vec3 bitangent = cross(normal, tangent);
	return mat3(tangent, bitangent, normal);
}

mat3 GetIdentity() {
	return mat3(
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0);
}

mat3 QuatToMat3(vec4 q) {
	vec3 m0 = vec3(
				1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z, 
				2.0 * q.x * q.y + 2.0 * q.w * q.z,
				2.0 * q.x * q.z - 2.0 * q.w * q.y);
	vec3 m1 = vec3(
				2.0 * q.x * q.y - 2.0 * q.w * q.z,
				1.0 - 2.0 * q.x * q.x - 2.0 * q.z * q.z,
				2.0 * q.y * q.z + 2.0 * q.w * q.x);
	vec3 m2 = vec3(
				2.0 * q.x * q.z + 2.0 * q.w * q.y,
				2.0 * q.y * q.z - 2.0 * q.w * q.x, 
				1.0 - 2.0 * q.x * q.x - 2.0 * q.y * q.y);
	
	return mat3(m0, m1, m2);
}

mat4 QuatToMat4(vec4 q) {
	mat3 m3 = QuatToMat3(q);
	return mat4(vec4(m3[0], 0.0), 
				vec4(m3[1], 0.0), 
				vec4(m3[2], 0.0), 
				vec4(0.0, 0.0, 0.0, 1.0)
			);
}

mat4 Scale(float size) {
	return mat4(
		size, 0.0, 0.0, 0.0,
		0.0, size, 0.0, 0.0,
		0.0, 0.0, size, 0.0,
		0.0, 0.0,  0.0, 1.0
	);
}

mat4 Scale(vec3 s) {
	return mat4(
		s.x, 0.0, 0.0, 0.0,
		0.0, s.y, 0.0, 0.0,
		0.0, 0.0, s.z, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
}

mat4 Translate(vec3 t) {
	return mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		t.x, t.y, t.z, 1.0
	);
}

// ivd = 1 / (f - n)
//#define USE_LINEAR_DEPTH 1.0
vec4 DepthToLinear(mat4 vpp, mat4 lpp, mat4 lvp, float n, float ivd, vec4 mv) {
#ifndef USE_LINEAR_DEPTH
	return vpp * mv;
#else
	vec4 lv = lvp * mv;
	vec4 pv = lpp * lv;
	pv.z = -(lv.z + n) * ivd * pv.w;
	return pv;
#endif
}

#define START_H float(0.0)
#define END_H float(1500.0)
#ifndef USE_CARTOON
	#define FOG_COLOR vec3(0.9)
#else 
	#define FOG_COLOR vec3(1.0)
#endif
vec3 GenFogColor(float factor, vec4 worldPos, float depthView, float udotl, vec3 sceneColor) {
	float worldH = worldPos.y / worldPos.w;
	float heightFactor = smoothstep(START_H, END_H, worldH);
	float fogFactor = exp2(factor * depthView * depthView * LOG2);

	fogFactor = mix(fogFactor, 1.0, heightFactor);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	return mix(FOG_COLOR * udotl, sceneColor, fogFactor);
}

struct Indirect {
	uint count;
	uint primCount;
	uint firstIndex;
	uint baseVertex;
	uint baseInstance;
};

#define BindlessSampler2D layout(bindless_sampler) sampler2D
#define BindlessSamplerCube layout(bindless_sampler) samplerCube

#define AllBuf    0
#define NormalBuf 1
#define SingleBuf 2
#define BillBuf   3
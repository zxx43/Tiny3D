#extension GL_ARB_bindless_texture : enable 

const float PI = 3.1415926;
const float INV_PI = 0.318309891613572;
const vec4 RAND_FACTOR = vec4(12.9898, 78.233, 45.164, 94.673);
const vec4 DefaultRM = vec4(0.0, 0.0, 0.0, 1.0);
const vec3 MatScale = vec3(0.6, 1.2, 1.0);
const vec3 COLOR_SCALE = vec3(0.003, 0.006, 0.005);
const vec3 GAMMA = vec3(2.2);
const vec3 INV_GAMMA = vec3(0.4546);
const float LOG2 = float(1.442695);
const vec4 CenterPosition = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 FAIL_COLOR = vec4(1.0, 1.0, 1.0, 0.0);
const vec3 UP_VEC3 = vec3(0.0, 1.0, 0.0);
const vec3 ZERO_VEC3 = vec3(0.0);

#define MAX_TEX 256
#define MAX_BONE 100

mat3 RotY(float r) {
	float cosR = cos(r);
	float sinR = sin(r);
	return mat3(
		cosR, 0.0, -sinR,
		0.0,  1.0, 0.0,
		sinR, 0.0, cosR
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

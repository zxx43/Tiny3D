#extension GL_ARB_bindless_texture : enable 

const float PI = 3.1415926;
const float INV_PI = 0.318309891613572;
const vec4 RAND_FACTOR = vec4(12.9898, 78.233, 45.164, 94.673);
const vec4 DefaultRM = vec4(1.0, 0.0, 0.0, 1.0);
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
const vec4 BoardRM = vec4(1.0, 0.0, 0.0, 1.0);
const float NEG_CHECK = 5.0;
const uint MAX_TEX = 128;

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
#ifndef NO_FOG
	float worldH = worldPos.y / worldPos.w;
	float heightFactor = smoothstep(START_H, END_H, worldH);
	float fogFactor = exp2(factor * depthView * depthView * LOG2);

	fogFactor = mix(fogFactor, 1.0, heightFactor);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	return mix(FOG_COLOR * udotl, sceneColor, fogFactor);
#else
	return sceneColor;
#endif
}

float RevertNegative(float value) {
	return value - NEG_CHECK * 2.0;
}

vec3 DecodeNormal(vec2 vNormal2) {
	vec2 normal2 = vNormal2;
	float signNormal = 1.0;
	if(normal2.x > NEG_CHECK) {
		normal2.x = RevertNegative(normal2.x);
		signNormal = -1.0;
	}
	float z = signNormal * sqrt(1.0 - dot(normal2, normal2));
	return vec3(normal2, z);
}

vec4 DecodeQuat(vec3 vQuat3) {
	vec3 quat3 = vQuat3;
	float signQuat = 1.0;
	if(quat3.z > NEG_CHECK) {
		quat3.z = RevertNegative(quat3.z);
		signQuat = -1.0;
	}
	float w = signQuat * sqrt(1.0 - dot(quat3, quat3));
	return vec4(quat3, w);
}

vec3 UnpackFloat2Vec( float v ) {
	int iv = int(v);
	int z = iv >> 20;
	int y = iv - (z << 20); y = y >> 10;
	float x = v - float((z << 20) + (y << 10));
	return vec3(x, vec2(ivec2(y, z)));
}

bool CheckCull(vec4 cp0, vec4 cp1, vec4 cp2, vec4 cp3, 
			vec4 cp4, vec4 cp5, vec4 cp6, vec4 cp7) {
	if(cp0.w < 0.001 || cp1.w < 0.001 || cp2.w < 0.001 || cp3.w < 0.001 || 
		cp4.w < 0.001 || cp5.w < 0.001 || cp6.w < 0.001 || cp7.w < 0.001) return false;

	vec3 p0 = cp0.xyz / cp0.w, p1 = cp1.xyz / cp1.w, p2 = cp2.xyz / cp2.w, p3 = cp3.xyz / cp3.w;
	vec3 p4 = cp4.xyz / cp4.w, p5 = cp5.xyz / cp5.w, p6 = cp6.xyz / cp6.w, p7 = cp7.xyz / cp7.w;

	float thr = 1.0, thz = 1.0;
	vec3 maxP = max(p7, max(p6, max(p5, max(p4, max(p3, max(p2, max(p0, p1)))))));		
	vec3 minP = min(p7, min(p6, min(p5, min(p4, min(p3, min(p2, min(p0, p1)))))));	
	return any(greaterThan(minP, vec3(thr, thr, thz))) || any(lessThan(maxP, vec3(-thr, -thr, -thz)));
}

float Linearize(float near, float far, float depth) {
	return (2.0 * near) / (far + near - depth * (far - near));
}

vec4 Linearize(float near, float far, vec4 depth) {
	return vec4(2.0 * near) / (vec4(far + near) - depth * (far - near));
}

bool CompMat(float flag, float base) {
	return (flag > (base - 0.01) && flag < (base + 0.01));
}

vec3 GetNormalFromMap(sampler2D tex, vec2 coord, mat3 tbn) {
	return tbn * (2.0 * texture(tex, coord).rgb - 1.0);
}

#define LEVEL_1 1
#define LEVEL_2 3
#define LEVEL_3 7

uint GetChunkLevel(ivec2 ref, ivec2 target) {
	ivec2 ref2Target = abs(target - ref);
	if(any(greaterThan(ref2Target, ivec2(LEVEL_3)))) return 8;
	else if(any(greaterThan(ref2Target, ivec2(LEVEL_2)))) return 4;
	else if(any(greaterThan(ref2Target, ivec2(LEVEL_1)))) return 2;
	else return 1;
}

uint GetLevel(ivec2 ref, int targetChunk, int lineChunks) {
	ivec2 target = ivec2((targetChunk % lineChunks), (targetChunk / lineChunks));
	return GetChunkLevel(ref, target);
}

struct Material {
	vec4 texids;
	vec4 params;
};

struct ChunkBuff {
	vec4 center;
	vec4 size;
};

struct Indirect {
	uint count;
	uint primCount;
	uint firstIndex;
	uint baseVertex;
	uint baseInstance;
};

#define BindlessSampler2D layout(bindless_sampler) sampler2D
#define BindlessSamplerCube layout(bindless_sampler) samplerCube
#define BindlessSamplerBuffer layout(bindless_sampler) samplerBuffer

#define AllBuf    0
#define NormalBuf 1
#define SingleBuf 2
#define BillBuf   3

// Material Flags
#define WaterFlag      0.0
#define WaterThreshold 0.1
#define LeafFlag       0.5
#define GrassFlag      0.7
#define TerrainFlag    0.8
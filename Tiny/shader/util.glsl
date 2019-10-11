#define PI 3.1415926
#define RAND_FACTOR vec4(12.9898, 78.233, 45.164, 94.673)
#define DefaultRM vec4(0.0, 0.0, 0.0, 1.0)
#define MatScale vec3(0.6, 1.2, 1.0)
#define GAMMA vec3(2.2)
#define INV_GAMMA vec3(0.4546)
#define LOG2 float(1.442695)

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

mat3 GetTBN(vec3 normal, vec3 tangent) {
	vec3 bitangent = cross(normal, tangent);
	return mat3(tangent, bitangent, normal);
}
#version 450
#extension GL_ARB_bindless_texture : enable 
 
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(bindless_sampler) uniform sampler2D texBlds[256];
uniform mat4 viewProjectMatrix;
uniform mat4 viewMatrix;
uniform float time;
uniform float distortionId;

in vec3 tePosition[];

out float vHeight;

#define RAND_FACTOR vec4(12.9898, 78.233, 45.164, 94.673)
#define PI 3.1415926
#define WindStrength 0.6
#define WindFrequency vec2(0.05, 0.05)

mat3 rotY(float r) {
	float cosR = cos(r);
	float sinR = sin(r);
	return mat3(
		cosR, 0.0, -sinR,
		0.0,  1.0, 0.0,
		sinR, 0.0, cosR
	);
}

mat3 AngleAxis3x3(float angle, vec3 axis) {
	float c = cos(angle), s = sin(angle);

	float t = 1 - c;
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	
	return mat3(
		t * x * x + c, t * x * y + s * z, t * x * z - s * y,
		t * x * y - s * z, t * y * y + c, t * y * z + s * x,
		t * x * z + s * y, t * y * z - s * x, t * z * z + c
	);
}

mat3 GetWindMat(vec2 pos) {
	vec2 uv = pos * 0.01 + WindFrequency * time;
	vec2 windSample = texture(texBlds[int(distortionId)], uv).xy * 2.0 - 1.0;
	windSample *= WindStrength;
	vec3 wind = normalize(vec3(windSample.x, windSample.y, 0.0));
	mat3 windRotation = AngleAxis3x3(PI * windSample.x, wind);
	return windRotation;
}

float random(vec3 seed, float i){
	vec4 seed4 = vec4(seed,i);
	float dotProduct = dot(seed4, RAND_FACTOR);
	return fract(sin(dotProduct) * 43758.5453);
}

void main() {
	vec3 vert0 = tePosition[0];
	vec3 vert1 = tePosition[1];
	vec3 vert2 = tePosition[2];
	vec3 trans = (vert0 + vert1 + vert2) * 0.333;
	vec2 randTrans = vec2(random(trans, 0.1), random(trans.xzy, 0.2)); 
	trans.xz += randTrans;

	float viewz = (viewMatrix * vec4(trans, 1.0)).z;
	if(viewz < 10.0) {
		float rand = random(trans, randTrans.x);
		float gw = rand * 0.3 + 0.1;
		float gh = rand * 2.0 + 0.5;

		vec3 vertA = vec3(-gw, 0.0, 0.0);
		vec3 vertB = vec3(gw, 0.0, 0.0);
		vec3 vertC = vec3(0.0, gh, 0.0);
		float hA = vertA.y, hB = vertB.y, hC = vertC.y;

		mat3 rotMat = rotY(rand * PI);
		mat3 windMat = GetWindMat(trans.xz + vec2(viewz));
		rotMat = rotMat * windMat;
		vertA = rotMat * vertA + trans;
		vertB = rotMat * vertB + trans;
		vertC = rotMat * vertC + trans;

		float invHeight = 1.0 / (hC - hA);

		vHeight = hA * invHeight;
		gl_Position = viewProjectMatrix * vec4(vertA, 1.0);
		EmitVertex();
	
		vHeight = hB * invHeight;
		gl_Position = viewProjectMatrix * vec4(vertB, 1.0);
		EmitVertex();
	
		vHeight = hC * invHeight;
		gl_Position = viewProjectMatrix * vec4(vertC, 1.0);
		EmitVertex();
	
		EndPrimitive(); 
	}
}
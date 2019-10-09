#extension GL_ARB_bindless_texture : enable 
 
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(bindless_sampler) uniform sampler2D texBlds[256];
uniform mat4 viewProjectMatrix;
uniform mat4 viewMatrix;
uniform float time;
uniform float distortionId;

in vec3 tePosition[];
in vec3 teNormal[];

out vec4 vNormalHeight;

#define RAND_FACTOR vec4(12.9898, 78.233, 45.164, 94.673)
#define PI 3.1415926
#define WindStrength 0.6
#define WindFrequency vec2(0.05, 0.05)

mat3 RotY(float r) {
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

	float t = 1.0 - c;
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
	vec3 trans = tePosition[1];
	vec2 randTrans = vec2(random(trans, 0.1), random(trans.xzy, 0.2)); 
	trans.xz += randTrans;

	vec4 projPos = viewProjectMatrix * vec4(trans, 1.0);
	if(projPos.z > 0.0 && 
			projPos.x > -projPos.w && projPos.x < projPos.w && 
			projPos.y < projPos.w) {
		float rand = randTrans.x + randTrans.y;
		float gw = rand * 0.5 + 0.1;
		float gh = rand * 2.5 + 0.5;

		vec3 vertA = vec3(-gw, 0.0, 0.0), vertB = -vertA;
		vec3 vertC = vec3(0.0, gh, 0.0);
		vec2 hbt = vec2(vertA.y, vertC.y);
		float invh = 1.0 / (hbt.y - hbt.x);
		hbt *= invh;

		mat3 rotMat = RotY(rand * PI);
		float viewz = (viewMatrix * vec4(trans, 1.0)).z;
		mat3 windMat = GetWindMat(trans.xz + randTrans + vec2(viewz));
		rotMat = rotMat * windMat;
		vertA = rotMat * vertA + trans;
		vertB = rotMat * vertB + trans;
		vertC = rotMat * vertC + trans;

		vec3 normal = (teNormal[0] + teNormal[1] + teNormal[2]) * 0.333;

		vNormalHeight = vec4(normal, hbt.x);
		gl_Position = viewProjectMatrix * vec4(vertA, 1.0);
		EmitVertex();
	
		vNormalHeight = vec4(normal, hbt.x);
		gl_Position = viewProjectMatrix * vec4(vertB, 1.0);
		EmitVertex();
	
		vNormalHeight = vec4(normal, hbt.y);
		gl_Position = viewProjectMatrix * vec4(vertC, 1.0);
		EmitVertex();
	
		EndPrimitive(); 
	}
}
#version 330

uniform mat4 viewProjectMatrix;
uniform mat4 viewMatrix;
uniform float time;
uniform vec3 eyePos;
uniform vec3 boundPos;
uniform vec3 boundScl;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec3 color;
layout (location = 4) in vec4 modelTrans;

out VertexData {
	vec4 vTexcoord;
	vec3 vNormal;
	flat vec3 vColor;
	//flat float vOutBound;
} vertOut;

#define COLOR_SCALE vec3(0.003, 0.006, 0.005)
#define INVALID_POINT vec4(2.0, 2.0, 2.0, 1.0)

float CheckOut(vec3 pos, vec3 scl) {
	vec4 bb[8];
	bb[0] = viewProjectMatrix * vec4(pos + vec3( scl.x, scl.y, scl.z), 1.0);
	bb[1] = viewProjectMatrix * vec4(pos + vec3(-scl.x, scl.y, scl.z), 1.0);
	bb[2] = viewProjectMatrix * vec4(pos + vec3( scl.x,-scl.y, scl.z), 1.0);
	bb[3] = viewProjectMatrix * vec4(pos + vec3(-scl.x,-scl.y, scl.z), 1.0);
	bb[4] = viewProjectMatrix * vec4(pos + vec3( scl.x, scl.y,-scl.z), 1.0);
	bb[5] = viewProjectMatrix * vec4(pos + vec3(-scl.x, scl.y,-scl.z), 1.0);
	bb[6] = viewProjectMatrix * vec4(pos + vec3( scl.x,-scl.y,-scl.z), 1.0);
	bb[7] = viewProjectMatrix * vec4(pos + vec3(-scl.x,-scl.y,-scl.z), 1.0);

	vec4 outOfBound = vec4(0.0);
	for (int i = 0; i < 8; i++) {
		if		(bb[i].x >  bb[i].w) outOfBound.x += 1.0;
		else if (bb[i].x < -bb[i].w) outOfBound.y += 1.0;
		
		if		(bb[i].y >  bb[i].w) outOfBound.z += 1.0;
		else if (bb[i].y < -bb[i].w) outOfBound.w += 1.0;
	}
    
	outOfBound = step(vec4(7.9), outOfBound);
	return dot(outOfBound, vec4(1.0));
}

void main() {
	vertOut.vColor = COLOR_SCALE * color;

	vec4 worldVertex = vec4(modelTrans.w * vertex + modelTrans.xyz, 1.0);

	vec3 bbPos = boundPos + modelTrans.xyz;
	vec3 bbView = (viewMatrix * vec4(bbPos, 1.0)).xyz;
	//vec3 bbScl = boundScl * modelTrans.w * 0.7;
	//CheckOut(bbPos, bbScl) 
	//vertOut.vOutBound = step(1400.0, -bbView.z) + step(10.0, bbView.z);
	float outBound = step(1400.0, -bbView.z) + step(10.0, bbView.z);
	if(outBound > 0.1) 
		gl_Position = INVALID_POINT;
	else {
		float wave = step(0.9, texcoord.y) * sin(time + dot(modelTrans.xz, vec2(1.0))) * 2.0;
		worldVertex.xz += vec2(wave);

		vertOut.vNormal = normal;
		vertOut.vTexcoord = texcoord;
		gl_Position = viewProjectMatrix * worldVertex;
	}
}
uniform mat4 viewProjectMatrix;
uniform mat4 viewMatrix;
uniform float time;
uniform vec3 eyePos;
uniform vec3 boundPos;
uniform vec3 boundScl;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec2 texid;
layout (location = 4) in vec3 color;
layout (location = 5) in vec3 tangent;
layout (location = 6) in vec4 modelTrans;

out vec2 vTexcoord;
flat out vec4 vTexid;
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;
//flat out float vOutBound;

#define COLOR_SCALE vec3(0.003, 0.006, 0.005)
#define INVALID_POINT vec4(1.1, 1.1, 1.1, 1.0)

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

float Wave(vec4 tex, vec4 pos) {
	return step(0.9, tex.y) * sin(time + dot(pos.xz, vec2(1.0))) * 2.0;
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

void main() {
	vec4 worldVertex = vec4(modelTrans.w * vertex + modelTrans.xyz, 1.0);

#ifndef ShadowPass
		//vec3 bbPos = boundPos + modelTrans.xyz;
		//vec3 bbView = (viewMatrix * vec4(bbPos, 1.0)).xyz;
		////vec3 bbScl = boundScl * modelTrans.w * 0.7;
		////CheckOut(bbPos, bbScl) 
		////vOutBound = step(1400.0, -bbView.z) + step(10.0, bbView.z);
		//float outBound = step(1400.0, -bbView.z) + step(10.0, bbView.z);
		//if(outBound > 0.1) 
		//	gl_Position = INVALID_POINT;
		//else {
			worldVertex.xz += vec2(Wave(texcoord, modelTrans));

			vColor = COLOR_SCALE * color;
			vNormal = normal;
			vTBN = GetIdentity();
			vTexcoord = texcoord.xy;
			vTexid = vec4(texcoord.zw, -1.0, -1.0);
			gl_Position = viewProjectMatrix * worldVertex;
		//}
#else
		worldVertex.xz += vec2(Wave(texcoord, modelTrans));

		vTexcoord = texcoord.xy;
		vTexid = vec4(texcoord.zw, -1.0, -1.0);
		gl_Position = viewProjectMatrix * worldVertex;
#endif
}
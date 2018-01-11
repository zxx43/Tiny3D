#version 330

uniform mat4 viewMatrix;
uniform mat4 projectMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 boneMats[100];
uniform int shadowPass;
uniform mat4 lightViewProjNear, lightViewProjMid, lightViewProjFar;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 color;
layout (location = 4) in float texid;
layout (location = 5) in vec4 boneids;
layout (location = 6) in vec4 weights;

out vec2 vTexcoord;
flat out vec3 vColor;
flat out float vTexid;
out vec3 vNormal;
out vec4 projPosition;
out vec4 viewPosition;
out vec4 lightNearPosition,lightMidPosition,lightFarPosition;

void main() {	
	mat4 boneMat = boneMats[int(boneids.x)] * weights.x;
    boneMat += boneMats[int(boneids.y)] * weights.y;
    boneMat += boneMats[int(boneids.z)] * weights.z;
    boneMat += boneMats[int(boneids.w)] * weights.w;
    
    vec4 position = boneMat * vec4(vertex, 1.0);
    vec3 normal3 = (boneMat * vec4(normal, 0.0)).xyz;
    normal3 = uNormalMatrix * normal3;

	vColor = color * 0.004;
	vNormal = normal3;
	vTexid = texid;
	
	vTexcoord = texcoord; 
	vec4 modelPosition = uModelMatrix * position;
	viewPosition = viewMatrix * modelPosition;
	gl_Position = projectMatrix * viewPosition;
	projPosition = gl_Position;
	if(shadowPass == 0) {
		lightNearPosition = lightViewProjNear * modelPosition;
		lightMidPosition = lightViewProjMid * modelPosition;
		lightFarPosition = lightViewProjFar * modelPosition;
	}
}

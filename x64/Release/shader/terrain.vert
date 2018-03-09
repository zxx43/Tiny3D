#version 330

uniform mat4 viewMatrix;
uniform mat4 projectMatrix;
uniform mat4 lightViewProjNear, lightViewProjMid, lightViewProjFar;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec3 color;

out vec4 vTexcoord;
flat out vec3 vColor;
out vec3 vNormal;
out vec4 worldPosition;
out vec4 projPosition;
out vec4 viewPosition;
out vec4 lightNearPosition,lightMidPosition,lightFarPosition;

void main() {
	vColor = color * 0.004;
	
	vec4 worldVertex = vec4(vertex, 1.0);
	vNormal = normal;

	worldPosition = worldVertex;
	
	vTexcoord = texcoord; 
	viewPosition = viewMatrix * worldVertex;
	gl_Position = projectMatrix * viewPosition;
	projPosition = gl_Position;

	lightNearPosition = lightViewProjNear * worldVertex;
	lightMidPosition = lightViewProjMid * worldVertex;
	lightFarPosition = lightViewProjFar * worldVertex;
}
#version 330

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec3 color;

out vec4 vTexcoord;
flat out vec3 vColor;
out vec3 vNormal;
out float worldHeight;

void main() {
	float af = 0.6; float df = 1.2;
	vColor = vec3(color.r * af, color.g * df, color.b) * 0.004;
	
	vec4 worldVertex = vec4(vertex, 1.0);
	vNormal = normal;

	worldHeight = worldVertex.y;
	
	vTexcoord = texcoord; 
	gl_Position = viewProjectMatrix * worldVertex;
}
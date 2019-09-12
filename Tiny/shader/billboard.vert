#version 450

uniform mat4 viewProjectMatrix;
uniform vec3 viewRight;
uniform float shadowPass;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec4 texcoord;
layout (location = 2) in vec3 position;
layout (location = 3) in vec4 board;

out vec2 vTexcoord;
flat out vec2 vTexid;

#define TOP_VEC vec3(0.0, 1.0, 0.0)

void main() {
	vec2 size = vertex.xy * board.xy;
	vec3 right = size.x * viewRight;
	vec3 top = (shadowPass < 0.5) ? (size.y * TOP_VEC) : vec3(0.0, size.y, 0.0);
	vec3 worldVertex = position + right + top;

	vTexcoord = texcoord.xy; 
	vTexid = texcoord.zw;
	gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
}
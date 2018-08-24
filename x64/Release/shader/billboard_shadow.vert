#version 330

uniform mat4 viewProjectMatrix;
uniform vec3 light;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 texcoord;
layout (location = 2) in vec3 position;
layout (location = 3) in vec2 board;

out vec3 vTexcoord;
out vec2 vProjDepth;

void main() {
	vec3 viewRight = vec3(-light.z, 0.0, light.x);
	
	vec2 size = vertex.xy * board;
	vec3 right = size.x * viewRight;
	vec3 top = vec3(0.0, size.y, 0.0);
	vec3 worldVertex = position + right + top;

	vTexcoord = texcoord;
	gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
	vProjDepth = gl_Position.zw;
}
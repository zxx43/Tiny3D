#version 330

uniform mat4 viewProjectMatrix;
uniform vec3 light;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 texcoord;
layout (location = 2) in vec3 position;
layout (location = 3) in vec2 board;

out vec3 vTexcoord;
out vec2 projDepth;

void main() {
	vec3 viewRight = vec3(-light.z, 0.0, light.x);
	vec3 viewTop = vec3(0.0, 1.0, 0.0);
	
	vTexcoord = texcoord;
	vec3 right = vertex.x * board.x * viewRight;
	vec3 top = vertex.y * board.y * viewTop;
	vec3 worldVertex = position + right + top;
	gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
	projDepth = gl_Position.zw;
}
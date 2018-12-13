#version 330

uniform mat4 viewProjectMatrix;
uniform mat4 viewMatrix;
uniform vec4 texPixel;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec4 texcoord;
layout (location = 2) in vec3 position;
layout (location = 3) in vec4 board;

out vec2 vTexcoord;
out vec3 vNormal;

void main() {
	vec3 viewRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	vec3 viewTop = vec3(0.0, 1.0, 0.0);
	
	vec2 size = vertex.xy * board.xy;
	vec3 right = size.x * viewRight;
	vec3 top = size.y * viewTop;
	vec3 worldVertex = position + right + top;

	float tx = (texcoord.x * texPixel.z + board.z) * texPixel.x;
	float ty = (texcoord.y * texPixel.w + board.w) * texPixel.y;

	vTexcoord = vec2(tx, ty);
	vNormal = cross(viewRight, viewTop);
	gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
}
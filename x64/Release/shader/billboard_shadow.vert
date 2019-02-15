#version 330

uniform mat4 viewProjectMatrix;
uniform vec3 viewRight;
uniform vec4 texPixel;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec4 texcoord;
layout (location = 2) in vec3 position;
layout (location = 3) in vec4 board;

out vec2 vTexcoord;

void main() {
	vec2 size = vertex.xy * board.xy;
	vec3 right = size.x * viewRight;
	vec3 top = vec3(0.0, size.y, 0.0);
	vec3 worldVertex = position + right + top;

	//vTexcoord.x = (texcoord.x * texPixel.z + board.z) * texPixel.x;
	//vTexcoord.y = (texcoord.y * texPixel.w + board.w) * texPixel.y;
	vTexcoord = (texcoord.xy * texPixel.zw + board.zw) * texPixel.xy;
	gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
}
#version 330

uniform vec2 pixelSize;

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec4 texcoord;

out vec2 vTexcoord;

void main() {
	vTexcoord = texcoord.xy + pixelSize * 0.5;
	gl_Position = vec4(vertex, 1.0);
}

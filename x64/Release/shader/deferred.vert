#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texcoord;

out vec2 vTexcoord;

void main() {
	vTexcoord = texcoord;
	gl_Position = vec4(vertex, 1.0);
}

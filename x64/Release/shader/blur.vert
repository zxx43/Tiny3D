#version 330

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec2 texcoord;

out vec2 vTexcoord;

void main() {
	vTexcoord = texcoord;
	gl_Position = vec4(vertex * 2.0, 1.0);
}

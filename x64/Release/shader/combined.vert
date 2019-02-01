#version 330

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec4 texcoord;

out vec2 vTexcoord;

void main() {
	vTexcoord = texcoord.xy;
	gl_Position = vec4(vertex, 1.0);
}

#version 450

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;

out vec3 vPosition;

void main() {
	vPosition = vertex;
}
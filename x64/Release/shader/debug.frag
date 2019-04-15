#version 330

flat in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormalGrass;

void main() {
	FragTex = vec4(0.0, 0.0, 0.0, 1.0);
	FragColor = vec4(vColor, 1.0);
	FragNormalGrass = vec4(normalize(vNormal) * 0.5 + 0.5, 0.0);
}
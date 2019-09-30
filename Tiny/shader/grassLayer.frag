#version 450

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormalGrass;
layout (location = 3) out vec4 FragRoughMetal;

in float vHeight;

void main() {
	vec4 bottomColor = vec4(0.01, 0.05, 0.02, 1.0);
	vec4 topColor = vec4(0.15, 0.3, 0.1, 1.0);

	FragTex = mix(bottomColor, topColor, vHeight);
	FragMat = vec4(0.5, 0.0, 0.0, 1.0);
	FragNormalGrass = vec4(0.5, 1.0, 0.5, 0.0);
	FragRoughMetal = vec4(0.0, 0.0, 0.0, 1.0);
}
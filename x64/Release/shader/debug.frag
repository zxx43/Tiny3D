#version 330
#extension GL_EXT_gpu_shader4 : enable 

flat in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;

void main() {
	FragTex = vec4(0.0, 0.0, 0.0, 1.0);
	FragColor = vec4(vColor, 1.0);
	FragNormal = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0);
}
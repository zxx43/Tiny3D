#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec3 vTexcoord;
in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec3 FragColor;
layout (location = 2) out vec3 FragNormal;

void main() {
	FragTex = texture2DArray(texture, vTexcoord);
	FragColor = vColor;
	FragNormal = normalize(vNormal) * 0.5 + 0.5;
}
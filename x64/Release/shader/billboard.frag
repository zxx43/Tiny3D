#version 330
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray texture;

in vec3 vTexcoord;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec3 FragColor;
layout (location = 2) out vec3 FragNormal;

void main() {
	vec4 textureColor = texture2DArray(texture, vTexcoord);
	
	FragTex = textureColor;
	FragColor = vec3(0.3, 0.0, 0.0);
	FragNormal = vec3(1.0);
}
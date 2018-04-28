#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec3 vTexcoord;
flat in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec3 FragColor;
layout (location = 2) out vec3 FragNormal;

void main() {
	vec4 textureColor = vTexcoord.p >= 0.0 ? texture2DArray(texture, vTexcoord) : vec4(1.0);
		
	FragTex = textureColor;
	FragColor = vColor;
	FragNormal = normalize(vNormal) * 0.5 + 0.5;
}
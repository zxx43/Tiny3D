#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec4 vTexcoord;
flat in vec3 vColor;
in vec3 vNormal;
in float worldHeight;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec3 FragColor;
layout (location = 2) out vec3 FragNormal;

void main() {
	vec4 tex1 = texture2DArray(texture, vTexcoord.xyz);
	vec4 tex2 = texture2DArray(texture, vTexcoord.xyw);
	float blendPer = smoothstep(100.0, 200.0, worldHeight);
	vec4 textureColor = mix(tex1, tex2, blendPer);
		
	FragTex = textureColor;
	FragColor = vColor;
	FragNormal = normalize(vNormal) * 0.5 + 0.5;
}
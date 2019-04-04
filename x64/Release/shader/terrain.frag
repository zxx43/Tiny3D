#version 330
#extension GL_EXT_gpu_shader4 : enable 
layout(early_fragment_tests) in;

uniform sampler2DArray texArray;
uniform float waterHeight, isReflect;

in vec4 vTexcoord;
in vec4 vTexOffset;
flat in vec3 vColor;
in vec3 vNormal;
in float worldHeight;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;

void main() {
	if(isReflect > 0.1 && worldHeight < waterHeight - 4.0)
		discard;

	vec4 tex1 = texture2DArray(texArray, vTexcoord.xyz);
	vec4 tex2 = texture2DArray(texArray, vTexcoord.xyw);
	vec4 tex3 = texture2DArray(texArray, vec3(vTexcoord.xy, vTexOffset.x));

	float blendPer = smoothstep(150.0, 250.0, worldHeight);
	vec4 texColor = mix(tex1, tex2, blendPer);

	blendPer = smoothstep(0.0, 70.0, worldHeight);
	texColor = mix(tex3, texColor, blendPer);
		
	FragTex = texColor;
	FragColor = vec4(vColor, 1.0);
	FragNormal = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0);
}
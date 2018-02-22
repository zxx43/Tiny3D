#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec2 vTexcoord;
flat in float vTexid;
in vec3 vColor;
in vec3 vNormal;
in vec4 projPosition;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragDepth;
layout (location = 2) out vec4 FragNormal;

void main() {
	vec3 texcoord = vec3(vTexcoord, vTexid);
	vec4 textureColor = texture2DArray(texture, texcoord);
	if(textureColor.a < 0.1) discard;

	vec3 normal = normalize(vNormal);

	FragColor.rgb = textureColor.rgb * vColor;
	FragColor.a = textureColor.a;

	float depth = projPosition.z / projPosition.w;
	FragDepth = vec4(depth, depth, depth, 1.0);
	vec3 outNormal = normal * 0.5 + 0.5;
	FragNormal = vec4(outNormal, 1.0); 
}
#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec2 vTexcoord;
flat in float vTexid;
flat in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;

void main() {
	vec3 texcoord = vec3(vTexcoord, vTexid);
	vec4 textureColor = texcoord.p >= 0.0 ? texture2DArray(texture, texcoord) : vec4(1.0);
	if(textureColor.a < 0.1) discard;

	vec3 normal = normalize(vNormal);
		
	float ambientFactor = 0.6; float diffuseFactor = 1.2;
	float ambientColor = vColor.x * ambientFactor;
	float diffuseColor = vColor.y * diffuseFactor;
		
	FragTex = textureColor;
	FragColor = vec4(ambientColor, diffuseColor, 0.0, 1.0);

	vec3 outNormal = normal * 0.5 + 0.5;
	FragNormal = vec4(outNormal, 1.0); 
}
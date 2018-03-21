#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec4 vTexcoord;
flat in vec3 vColor;
in vec3 vNormal;
in vec4 worldPosition;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;

void main() {
	vec4 textureColor = vec4(1.0);

	vec3 texcoord1 = vec3(vTexcoord.xy, vTexcoord.z);
	vec4 textureColor1 = texture2DArray(texture, texcoord1);
	
	vec3 texcoord2 = vec3(vTexcoord.xy, vTexcoord.w);
	vec4 textureColor2 = texture2DArray(texture, texcoord2);
		
	float height = worldPosition.y;
	float blendPer = smoothstep(100.0, 200.0, height);
	textureColor = (1.0 - blendPer) * textureColor1 + blendPer * textureColor2;

	vec3 normal = normalize(vNormal);
		
	float ambientFactor = 1.0; float diffuseFactor = 1.0;
	float ambientColor = vColor.x * ambientFactor;
	float diffuseColor = vColor.y * diffuseFactor;
		
	FragColor = vec4(ambientColor, diffuseColor, 0.0, 1.0);
	FragTex = textureColor;

	vec3 outNormal = normal * 0.5 + 0.5;
	FragNormal = vec4(outNormal, 1.0); 
}
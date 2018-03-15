#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;
uniform vec3 light;
uniform vec2 levels;
uniform sampler2D depthBufferNear, depthBufferMid, depthBufferFar;

in vec3 vTexcoord;
flat in vec3 vColor;
in vec3 vNormal;
in vec4 viewPosition;
in vec4 lightNearPosition,lightMidPosition,lightFarPosition;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;

void main() {
	vec3 normal = normalize(vNormal);
	vec3 texcoord = vTexcoord.xyz;
	texcoord.y = 1.0 - texcoord.y;
		
	vec3 reverseLight = normalize(-light);
	float ndotl = dot(reverseLight, normal);
		
	float ambientFactor = 0.6; float diffuseFactor = 1.2;
	vec3 ambientColor = vColor.xxx * ambientFactor;
	vec3 diffuseColor = vColor.yyy * diffuseFactor;
	vec3 textureColor = texture2DArray(texture, texcoord).rgb;

	diffuseColor *= max(ndotl, 0.0);
		
	FragColor.rgb = textureColor * (ambientColor + diffuseColor);
	FragColor.a = 1.0;
	
	vec3 outNormal = normal * 0.5 + 0.5;
	FragNormal = vec4(outNormal, 0.0); 
}
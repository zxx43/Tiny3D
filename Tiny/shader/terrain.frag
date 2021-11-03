#include "shader/util.glsl"

layout(early_fragment_tests) in;

layout(bindless_sampler) uniform sampler2D texBlds[MAX_TEX];
layout(bindless_sampler) uniform sampler2D roadTex;
uniform float waterHeight, isReflect;

in vec2 vTexcoord;
flat in vec2 vRMid;
flat in vec4 vTexid;
flat in vec3 vColor;
in vec3 vNormal;
in mat3 vTBN;
in vec4 vWorldVert;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragRoughMetal;

void main() {
	if(isReflect > 0.1 && vWorldVert.y < waterHeight - 4.0) discard;

	vec4 tex1 = texture(texBlds[int(vTexid.x)], vTexcoord);
	vec4 tex2 = texture(texBlds[int(vTexid.y)], vTexcoord);
	vec4 tex3 = texture(texBlds[int(vTexid.z)], vTexcoord);

	float blendPer = smoothstep(150.0, 250.0, vWorldVert.y);
	vec4 texColor = mix(tex1, tex2, blendPer);

	blendPer = texture(roadTex, vWorldVert.xz).x;
	texColor = mix(texColor, tex3, blendPer);

	vec3 normal = vNormal;
	if(vTexid.w >= 0.0) {
		vec3 texNorm = texture(texBlds[int(vTexid.w)], vTexcoord).rgb;
		texNorm = 2.0 * texNorm - vec3(1.0);
		normal = vTBN * texNorm;
	}
	normal = normalize(normal) * 0.5 + 0.5;
		
	FragTex = texColor;
	FragMat = vec4(vColor, TerrainFlag);

	FragRoughMetal.r = vRMid.x >= 0.0 ? texture(texBlds[int(vRMid.x)], vTexcoord).r : DefaultRM.r;
	FragRoughMetal.g = vRMid.y >= 0.0 ? texture(texBlds[int(vRMid.y)], vTexcoord).r : DefaultRM.g;
	
	FragRoughMetal.ba = normal.xy;
	FragMat.z = normal.z;
}
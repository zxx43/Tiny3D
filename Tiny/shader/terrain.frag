#include "shader/util.glsl"

layout(early_fragment_tests) in;

layout(bindless_sampler) uniform sampler2D texBlds[MAX_TEX];
layout(bindless_sampler) uniform sampler2D roadTex;
uniform float waterHeight, isReflect;

in vec2 vTexcoord;
flat in ivec2 vRMid;
flat in ivec4 vTexid;
flat in vec3 vColor;
in vec3 vNormal;
in mat3 vTBN;
in vec4 vWorldVert;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragRoughMetal;

void main() {
	if(isReflect > 0.1 && vWorldVert.y < waterHeight - 4.0) discard;

	vec4 tex1 = texture(texBlds[vTexid.x], vTexcoord);
	vec4 tex2 = texture(texBlds[vTexid.y], vTexcoord);
	vec4 tex3 = texture(texBlds[vTexid.z], vTexcoord);

	float blendPer = smoothstep(150.0, 250.0, vWorldVert.y);
	vec4 texColor = mix(tex1, tex2, blendPer);

	blendPer = texture(roadTex, vWorldVert.xz).x;
	texColor = mix(texColor, tex3, blendPer);

	vec3 normal = vTexid.w >= 0 ? GetNormalFromMap(texBlds[vTexid.w], vTexcoord, vTBN) : vNormal;
	normal = normalize(normal) * 0.5 + 0.5;
		
	FragTex = texColor;
	FragMat = vec4(vColor, TerrainFlag);

	FragRoughMetal.r = vRMid.x >= 0 ? texture(texBlds[vRMid.x], vTexcoord).r : DefaultRM.r;
	FragRoughMetal.g = vRMid.y >= 0 ? texture(texBlds[vRMid.y], vTexcoord).r : DefaultRM.g;
	
	FragRoughMetal.ba = normal.xy;
	FragMat.z = normal.z;
}
#include "shader/util.glsl"

uniform BindlessSampler2D texBlds[MAX_TEX];

in vec2 vTexcoord;
flat in vec4 vTexid;
flat in vec3 vColor;
in vec3 vNormal;
in mat3 vTBN;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormalGrass;
layout (location = 3) out vec4 FragRoughMetal;

void main() {
	vec3 normal = vNormal;
	if(vTexid.y >= 0.0) {
		vec3 texNorm = texture(texBlds[int(vTexid.y)], vTexcoord).rgb;
		texNorm = 2.0 * texNorm - vec3(1.0);
		normal = vTBN * texNorm;
	}

	FragTex = texture(texBlds[int(vTexid.x)], vTexcoord);
	FragMat = vec4(vColor, 1.0);
	FragNormalGrass = vec4(normalize(normal) * 0.5 + 0.5, 0.0);

	FragRoughMetal.r = vTexid.z >= 0.0 ? texture(texBlds[int(vTexid.z)], vTexcoord).r : DefaultRM.r;
	FragRoughMetal.g = vTexid.w >= 0.0 ? texture(texBlds[int(vTexid.w)], vTexcoord).r : DefaultRM.g;
}
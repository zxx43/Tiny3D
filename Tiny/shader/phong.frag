#include "shader/util.glsl"

uniform BindlessSampler2D texBlds[MAX_TEX];
#ifdef BillPass
uniform vec3 uNormal;
#endif

in vec2 vTexcoord;
flat in vec4 vTexid;
flat in vec3 vColor;
in vec3 vNormal;
in mat3 vTBN;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragRoughMetal;

void main() {
	ivec4 texid = ivec4(vTexid);
	vec4 textureColor = texture(texBlds[texid.x], vTexcoord);
	if(textureColor.a < 0.3) discard;
#ifndef BillPass
	vec3 normal = texid.y > 0.0 ? vTBN * (2.0 * texture(texBlds[texid.y], vTexcoord).rgb - 1.0) : vNormal;
	normal = normalize(normal) * 0.5 + 0.5;

	FragMat = vec4(vColor, 1.0);
	FragRoughMetal.r = texid.z >= 0 ? texture(texBlds[texid.z], vTexcoord).r : DefaultRM.r;
	FragRoughMetal.g = texid.w >= 0 ? texture(texBlds[texid.w], vTexcoord).r : DefaultRM.g;
#else
	vec3 normal = uNormal;

	FragMat = BoardMat;
	FragRoughMetal = BoardRM;
#endif
	FragTex = textureColor;
	FragRoughMetal.ba = normal.xy;
	FragMat.z = normal.z;
}
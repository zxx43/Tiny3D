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
	vec4 textureColor = texture(texBlds[int(vTexid.x)], vTexcoord);
	if(textureColor.a < 0.25) discard;
#ifndef BillPass
		vec3 normal = vTexid.y >= 0.0 ? vTBN * (2.0 * texture(texBlds[int(vTexid.y)], vTexcoord).rgb - 1.0) : vNormal;
		normal = normalize(normal) * 0.5 + 0.5;

		FragMat = vec4(vColor, 1.0);
		FragRoughMetal.r = vTexid.z >= 0.0 ? texture(texBlds[int(vTexid.z)], vTexcoord).r : DefaultRM.r;
		FragRoughMetal.g = vTexid.w >= 0.0 ? texture(texBlds[int(vTexid.w)], vTexcoord).r : DefaultRM.g;
#else
		vec3 normal = uNormal;

		FragMat = BoardMat;
		FragRoughMetal = BoardRM;
#endif
	FragTex = textureColor;
	FragRoughMetal.ba = normal.xy;
	FragMat.z = normal.z;
}
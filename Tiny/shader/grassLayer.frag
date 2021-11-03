#include "shader/util.glsl"

layout(early_fragment_tests) in;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragRoughMetal;

in vec4 vNormalHeight;

#define BottomColor vec4(0.005, 0.04, 0.01, 1.0)
#define TopColor vec4(0.1, 0.2, 0.05, 1.0)
#define GrassMat vec4(0.4, 0.7, 0.0, GrassFlag)
#define GrassRM vec4(1.0, 0.0, 0.0, 1.0)

void main() {
	FragTex = mix(BottomColor, TopColor, vNormalHeight.w);
	FragMat = GrassMat;
	FragRoughMetal = GrassRM;

	vec3 normal = normalize(vNormalHeight.xyz) * 0.5 + 0.5;
	FragRoughMetal.ba = normal.xy;
	FragMat.z = normal.z;
}
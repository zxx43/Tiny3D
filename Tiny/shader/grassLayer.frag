#include "shader/util.glsl"

layout(early_fragment_tests) in;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragRoughMetal;

in vec3 vNormal;
in vec3 vColor;

#define GrassMat vec4(0.4, 0.6, 0.0, GrassFlag)
#define GrassRM vec4(0.0, 0.0, 0.0, 1.0)

void main() {
	FragTex = vec4(vColor, 1.0);
	FragMat = GrassMat;
	FragRoughMetal = GrassRM;

	FragRoughMetal.ba = vNormal.xy;
	FragMat.z = vNormal.z;
}
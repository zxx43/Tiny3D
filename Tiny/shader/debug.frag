#include "shader/util.glsl"

flat in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragRoughMetal;

void main() {
	FragTex = vec4(0.0, 0.0, 0.0, 1.0);
	FragMat = vec4(vColor, 1.0);
	FragRoughMetal = DefaultRM;

	vec3 normal = normalize(vNormal) * 0.5 + 0.5;
	FragRoughMetal.ba = normal.xy;
	FragMat.z = normal.z;
}
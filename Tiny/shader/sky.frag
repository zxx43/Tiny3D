#include "shader/util.glsl"

layout(early_fragment_tests) in;

uniform BindlessSamplerCube texSky;

in vec3 texCoord;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragRoughMetal;

#define SkyMat vec4(1.0)
#define SkyNG vec4(0.0, 0.0, 1.0, 0.0)
#define SkyRM vec4(0.0, 0.0, 0.0, 1.0)

void main() {
	FragTex = texture(texSky, texCoord);
	FragMat = SkyMat;
	FragRoughMetal = SkyRM;

	FragRoughMetal.ba = SkyNG.xy;
	FragMat.z = SkyNG.z;
}
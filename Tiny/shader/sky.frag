layout(early_fragment_tests) in;

layout(bindless_sampler) uniform samplerCube texSky;

in vec3 texCoord;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormalGrass;
layout (location = 3) out vec4 FragRoughMetal;

#define SkyMat vec4(1.0)
#define SkyNG vec4(0.0, 0.0, 1.0, 0.0)
#define SkyRM vec4(0.0, 0.0, 0.0, 1.0)

void main() {
	FragTex = texture(texSky, texCoord);
	FragMat = SkyMat;
	FragNormalGrass = SkyNG;
	FragRoughMetal = SkyRM;
}
#version 450
#extension GL_ARB_bindless_texture : enable 
layout(early_fragment_tests) in;

layout(bindless_sampler) uniform samplerCube texSky;

in vec3 texCoord;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormalGrass;
layout (location = 3) out vec4 FragRoughMetal;

void main() {
	FragTex = texture(texSky, texCoord);
	FragMat = vec4(1.0);
	FragNormalGrass = vec4(0.0, 0.0, 1.0, 0.0);
	FragRoughMetal = vec4(0.0, 0.0, 0.0, 1.0);
}
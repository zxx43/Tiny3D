#version 330

uniform sampler2D texture;
uniform vec4 texPixel;

in vec4 vTexcoord;
flat in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormalGrass;

void main() {
	float tx = (vTexcoord.x * texPixel.z + vTexcoord.z) * texPixel.x;
	float ty = (vTexcoord.y * texPixel.w + vTexcoord.w) * texPixel.y;

	FragTex = texture2D(texture, vec2(tx, ty));
	FragMat = vec4(vColor, 1.0);
	FragNormalGrass = vec4(normalize(vNormal) * 0.5 + 0.5, 0.0);
}
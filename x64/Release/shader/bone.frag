#version 330

uniform sampler2D texture;
uniform vec4 texPixel;

in vec4 vTexcoord;
flat in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;
layout (location = 3) out vec4 FragGrass;

void main() {
	float tx = (vTexcoord.x * texPixel.z + vTexcoord.z) * texPixel.x;
	float ty = (vTexcoord.y * texPixel.w + vTexcoord.w) * texPixel.y;

	FragTex = texture2D(texture, vec2(tx, ty));
	FragColor = vec4(vColor, 1.0);
	FragNormal = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0);
	FragGrass = vec4(0.0, 0.0, 0.0, 1.0);
}
#version 330

uniform sampler2D texture;
uniform vec4 texPixel;

in vec4 vTexcoord;
flat in vec3 vColor;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormalGrass;

void main() {
	vec4 textureColor = texture2D(texture, vTexcoord.zw);
	if(textureColor.a < 0.3) discard;

	FragTex = textureColor;
	FragColor = vec4(vColor, 1.0);
	FragNormalGrass = vec4(normalize(vNormal) * 0.5 + 0.5, 0.0);
}
#version 330

uniform sampler2D texture;
uniform vec4 texPixel;
uniform vec3 uNormal;

in vec2 vTexcoord;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormalGrass;

void main() {
	vec4 textureColor = texture2D(texture, vTexcoord);
	if(textureColor.a < 0.4) discard;
	
	FragTex = textureColor;
	FragMat = vec4(0.3, 0.0, 0.0, 1.0);
	FragNormalGrass = vec4(uNormal, 0.0);
}
#version 330

uniform sampler2D texture;
uniform vec4 texPixel;

in vec2 vTexcoord;
in vec3 vNormal;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;

void main() {
	vec4 textureColor = texture2D(texture, vTexcoord);
	if(textureColor.a < 0.4) discard;
	
	FragTex = textureColor;
	FragColor = vec4(0.3, 0.0, 0.0, 1.0);
	FragNormal = vec4(vNormal, 1.0);
}
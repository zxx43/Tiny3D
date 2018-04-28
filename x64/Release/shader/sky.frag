#version 330

uniform samplerCube textureSky;

in vec3 texCoord;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec3 FragColor;
layout (location = 2) out vec3 FragNormal;

void main() {
	FragTex = texture(textureSky, texCoord);
	FragColor = vec3(1.0);
	FragNormal = vec3(0.0, 0.0, 1.0);
}
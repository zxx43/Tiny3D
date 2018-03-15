#version 330

uniform samplerCube textureSky;

in vec3 texCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;

void main() {
	vec4 texColor = texture(textureSky, texCoord);
	FragColor = vec4(texColor.rgb, 1.0);
	FragNormal = vec4(0.0, 0.0, 1.0, 1.0);
}
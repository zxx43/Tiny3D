#version 330

uniform sampler2D texture;

in VertexData {
	vec4 vTexcoord;
	vec3 vNormal;
	flat vec3 vColor;
} vertIn;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;
layout (location = 3) out vec4 FragGrass;

void main() {
	vec4 textureColor = texture2D(texture, vertIn.vTexcoord.zw);
	if(textureColor.a < 0.3) discard;

	FragTex = textureColor;
	FragColor = vec4(vertIn.vColor, 1.0);
	FragNormal = vec4(normalize(vertIn.vNormal) * 0.5 + 0.5, 1.0);
	FragGrass = vec4(0.0, 0.0, 0.0, 1.0);
}
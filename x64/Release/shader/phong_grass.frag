#version 330

uniform sampler2D texture;

in VertexData {
	vec4 vTexcoord;
	vec3 vNormal;
	flat vec3 vColor;
} vertIn;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormalGrass;

void main() {
	vec4 textureColor = texture2D(texture, vertIn.vTexcoord.zw);
	if(textureColor.a < 0.3) discard;

	FragTex = textureColor;
	FragMat = vec4(vertIn.vColor, 1.0);
	FragNormalGrass = vec4(normalize(vertIn.vNormal) * 0.5 + 0.5, 0.0);
}
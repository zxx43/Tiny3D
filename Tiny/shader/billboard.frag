#version 450
#extension GL_ARB_bindless_texture : enable 

layout(bindless_sampler) uniform sampler2D texBlds[256];
uniform vec3 uNormal;

in vec2 vTexcoord;
flat in vec2 vTexid;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormalGrass;
layout (location = 3) out vec4 FragRoughMetal;

void main() {
	vec4 textureColor = texture2D(texBlds[int(vTexid.x)], vTexcoord.xy);
	if(textureColor.a < 0.4) discard;
	
	FragTex = textureColor;
	FragMat = vec4(0.3, 0.0, 0.0, 1.0);
	FragNormalGrass = vec4(uNormal, 0.0);
	FragRoughMetal = vec4(0.0, 0.0, 0.0, 1.0);
}
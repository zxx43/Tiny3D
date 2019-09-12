#version 450
#extension GL_ARB_bindless_texture : enable 

layout(bindless_sampler) uniform sampler2D texBlds[256];

in vec2 vTexcoord;
flat in vec4 vTexid;

void main() {
	if(texture2D(texBlds[int(vTexid.x)], vTexcoord.xy).a < 0.3) discard;
}
#version 330

uniform sampler2D texture;

in vec4 vTexcoord;

void main() {
	if(texture2D(texture, vTexcoord.zw).a < 0.3) discard;
}
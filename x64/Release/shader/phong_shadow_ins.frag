#version 330

uniform sampler2D texture;
uniform vec4 texPixel;

in vec4 vTexcoord;

void main() {
	float alpha = texture2D(texture, vTexcoord.zw).a;
	if(alpha < 0.3) discard;
}
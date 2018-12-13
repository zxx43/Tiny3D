#version 330

uniform sampler2D texture;
uniform vec4 texPixel;

in vec2 vTexcoord;

void main() {
	float alpha = texture2D(texture, vTexcoord).a;
	if(alpha < 0.25) discard;
}
#version 330

uniform sampler2D texture;
uniform vec4 texPixel;

in vec4 vTexcoord;

void main() {
	float tx = (vTexcoord.x * texPixel.z + vTexcoord.z) * texPixel.x;
	float ty = (vTexcoord.y * texPixel.w + vTexcoord.w) * texPixel.y;

	float alpha = texture2D(texture, vec2(tx, ty)).a;
	if(alpha < 0.25) discard;
}
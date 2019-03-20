#version 330

uniform sampler2D texture;

in VertexData {
	vec4 vTexcoord;
} vertIn;

void main() {
	if(texture2D(texture, vertIn.vTexcoord.zw).a < 0.3) discard;
}
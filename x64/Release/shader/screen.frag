#version 330

uniform sampler2D colorBuffer;
uniform vec2 pixelSize;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
 	FragColor = texture2D(colorBuffer, vTexcoord);	
}
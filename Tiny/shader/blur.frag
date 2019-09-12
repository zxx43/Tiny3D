#version 450
#extension GL_ARB_bindless_texture : enable 

layout(bindless_sampler) uniform sampler2D colorBuffer;
uniform vec2 pixelSize;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
	float pixW = pixelSize.x, pixH = pixelSize.y;
	vec4 sum = texture2D(colorBuffer, vTexcoord) * 0.25;

	vec4 tmp = texture2D(colorBuffer, vTexcoord + vec2(-pixW, -pixH));
	sum += tmp * 0.0625;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(0.0, -pixH));
	sum += tmp * 0.125;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(pixW, -pixH));
	sum += tmp * 0.0625;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(-pixW, 0.0));
	sum += tmp * 0.125;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(pixW, 0.0));
	sum += tmp * 0.125;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(-pixW, pixH));
	sum += tmp * 0.0625;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(0.0, pixH));
	sum += tmp * 0.125;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(pixW, pixH));
	sum += tmp * 0.0625;

 	FragColor = sum;	
}
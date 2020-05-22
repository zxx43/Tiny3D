#include "shader/util.glsl"

layout(bindless_sampler) uniform sampler2D colorBuffer;
uniform vec2 pixelSize;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
	float pixW = pixelSize.x, pixH = pixelSize.y;
	vec4 sum = texture(colorBuffer, vTexcoord) * 0.111;

	vec4 tmp = texture(colorBuffer, vTexcoord + vec2(-pixW, -pixH));
	sum += tmp * 0.111;
	tmp = texture(colorBuffer, vTexcoord + vec2(0.0, -pixH));
	sum += tmp * 0.111;
	tmp = texture(colorBuffer, vTexcoord + vec2(pixW, -pixH));
	sum += tmp * 0.111;
	tmp = texture(colorBuffer, vTexcoord + vec2(-pixW, 0.0));
	sum += tmp * 0.111;
	tmp = texture(colorBuffer, vTexcoord + vec2(pixW, 0.0));
	sum += tmp * 0.111;
	tmp = texture(colorBuffer, vTexcoord + vec2(-pixW, pixH));
	sum += tmp * 0.111;
	tmp = texture(colorBuffer, vTexcoord + vec2(0.0, pixH));
	sum += tmp * 0.111;
	tmp = texture(colorBuffer, vTexcoord + vec2(pixW, pixH));
	sum += tmp * 0.111;

 	FragColor = sum;	
}
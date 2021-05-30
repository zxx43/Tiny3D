#include "shader/util.glsl"

layout(binding = 0) uniform sampler2D tex;
uniform float uLevel;
uniform vec2 uCamParam;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
 	float depth = textureLod(tex, vTexcoord, uLevel).x;
 	depth = Linearize(uCamParam.x, uCamParam.y, depth);	
 	FragColor = vec4(depth);
}
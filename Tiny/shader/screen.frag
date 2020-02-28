#include "shader/util.glsl"

layout(bindless_sampler) uniform sampler2D tex;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
 	FragColor = texture2D(tex, vTexcoord);	
}
#include "shader/util.glsl"

uniform BindlessSampler2D tex;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
 	FragColor = texture(tex, vTexcoord);	
}
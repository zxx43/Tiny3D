#include "shader/util.glsl"

uniform BindlessSampler2D texBlds[MAX_TEX];
uniform float uAlpha;

in vec2 vTexcoord;
flat in ivec4 vTexid;

void main() {
	if(uAlpha < 0.5) {}
	else {
		if(texture(texBlds[vTexid.x], vTexcoord.xy).a < 0.3) 
			discard;
	}
}
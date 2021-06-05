#include "shader/util.glsl"

layout(binding = 0) uniform sampler2D lastMip;
uniform float uOdd;
uniform float uLastLevel;

in vec2 vTexcoord;

#define OP max

void main() {
	ivec2 offsets[] = ivec2[](
      ivec2( 0, 0),
      ivec2( 0, 1),
      ivec2( 1, 1),
      ivec2( 1, 0),
      ivec2(-1,-1),
      ivec2( 0,-1),
      ivec2( 1,-1),
      ivec2(-1, 0),
      ivec2(-1, 1));
	
	vec4 depths = vec4(textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[0]).x, 
					   textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[1]).x, 
					   textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[2]).x, 
					   textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[3]).x);
	float maxDepth = OP(OP(depths.x, depths.y), OP(depths.z, depths.w));

	if (uOdd > 0.5) {
		vec4 extra = vec4(textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[4]).x, 
						  textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[5]).x, 
						  textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[6]).x, 
						  textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[7]).x);
		float ex = textureLodOffset(lastMip, vTexcoord, uLastLevel, offsets[8]).x;
		float maxExtra = OP(OP(OP(extra.x, extra.y), OP(extra.z, extra.w)), ex);
		maxDepth = OP(maxExtra, maxDepth);
	}

	gl_FragDepth = maxDepth;
}

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

	int iLastLevel = int(uLastLevel);
	ivec2 texSize = textureSize(lastMip, iLastLevel);

	float maxDepth = 0.0;
	if (uOdd < 0.5) {
		ivec2 coord = ivec2(gl_FragCoord.xy) * 2;
		for (int i = 0; i < 4; i++) {
			ivec2 uv = clamp(coord + offsets[i], ivec2(0), texSize - ivec2(1));
			maxDepth = OP(maxDepth, texelFetch(lastMip, uv, iLastLevel).r);
		}
	} else {
		vec2 coord = vTexcoord;
		vec2 texel = 1.0 / (vec2(texSize));

		for (int i = 0; i < 9; i++) {
			vec2 pos = coord + offsets[i] * texel;
			ivec2 uv = clamp(ivec2(pos * texSize), ivec2(0), texSize - ivec2(1));
			maxDepth = OP(maxDepth, texelFetch(lastMip, uv, iLastLevel).r);
		}
	}
	
	/*
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
	*/

	gl_FragDepth = maxDepth;
}

#include "shader/util.glsl"

layout(binding = 0) uniform sampler2D lastMip;
uniform ivec2 uLastSize;
uniform float uLastLevel;

in vec2 vTexcoord;

#define OP max

void main() {
	vec4 depths = vec4(
					textureLod(lastMip, vTexcoord, uLastLevel).x, 
					textureLodOffset(lastMip, vTexcoord, uLastLevel, ivec2(-1,  0)).x, 
					textureLodOffset(lastMip, vTexcoord, uLastLevel, ivec2(-1, -1)).x, 
					textureLodOffset(lastMip, vTexcoord, uLastLevel, ivec2( 0, -1)).x
				);
	
	float maxDepth = OP(OP(depths.x, depths.y), OP(depths.z, depths.w));
	
	bvec2 oddEdge;
	// gl_FragCoord right or top edge is LastSize * 0.5 - 0.5
	oddEdge.x = ((uLastSize.x & 1) != 0) && ((int(gl_FragCoord.x) * 2 + 1) == (uLastSize.x - 2));
	oddEdge.y = ((uLastSize.y & 1) != 0) && ((int(gl_FragCoord.y) * 2 + 1) == (uLastSize.y - 2));
	
	if(oddEdge.x && oddEdge.y) {
		// Get right top edge pixel
		float edgeDepth = textureLodOffset(lastMip, vTexcoord, uLastLevel, ivec2(1, 1)).x;
		maxDepth = OP(maxDepth, edgeDepth);
	}
	
	vec2 edge;
	if(oddEdge.x) {
		// Get right & right down edge pixels
		edge.x = textureLodOffset(lastMip, vTexcoord, uLastLevel, ivec2(1,  0)).x;
		edge.y = textureLodOffset(lastMip, vTexcoord, uLastLevel, ivec2(1, -1)).x;
		maxDepth = OP(maxDepth, OP(edge.x, edge.y));
	}
	if(oddEdge.y) {
		// Get top & top left edge pixels
		edge.x = textureLodOffset(lastMip, vTexcoord, uLastLevel, ivec2( 0, 1)).x;
		edge.y = textureLodOffset(lastMip, vTexcoord, uLastLevel, ivec2(-1, 1)).x;
		maxDepth = OP(maxDepth, OP(edge.x, edge.y));
	}
	
	gl_FragDepth = maxDepth;
}

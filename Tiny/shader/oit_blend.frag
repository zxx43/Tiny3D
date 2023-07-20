#include "shader/oit.glsl"

layout(binding = 0, r32ui) uniform uimage2D headPointers;
layout(binding = 0, std430) buffer LinkedList {
	uvec4 nodes[];
};

layout(location = 0) out vec4 FragColor;

vec4 blend(vec4 baseColor) {
	uvec4 frags[MAX_LAYER];
	uint layer = 0, next = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;
	while (next != 0 && layer < MAX_LAYER) {
		frags[layer] = nodes[next - 1];
		next = frags[layer].w;
		++layer;
	}
	
	if (layer > 0) Sort(frags, layer);

	vec4 resColor = baseColor;
	for (uint i = 0; i < layer; ++i) {  
		vec4 color = UnpackColor(frags[i].xy);
		resColor.rgb = mix(resColor.rgb, color.rgb, color.a);
		resColor.a = uintBitsToFloat(frags[i].z);
	}
	return resColor;
}

void main() {
	// todo read base color
	vec4 baseColor;
	FragColor = blend(baseColor);
}
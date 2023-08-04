#include "shader/oit.glsl"

layout (early_fragment_tests) in;

layout(bindless_image, r32ui) uniform uimage2D headPointers; // clear to 0 each frame
layout(binding = 0, offset = 0) uniform atomic_uint indexDispenser; // clear to 0 each frame
layout(binding = 0, std430) buffer LinkedList {
	uvec4 nodes[];
};
uniform uint uMaxNodes; // 1920 * 1200 * MAX_LAYER

void render(vec4 color) {
	uint index = atomicCounterIncrement(indexDispenser);
	uint newHead = atomicCounter(indexDispenser);
	if (index < uMaxNodes) {
		uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), newHead);
		uvec2 packedColor = PackColor(color);
		nodes[index].x = packedColor.x;
		nodes[index].y = packedColor.y;
		nodes[index].z = floatBitsToUint(gl_FragCoord.z);
		nodes[index].w = prevHead;
	}
}

void main() {
	// todo calculate transparent color
	vec4 transColor;
	render(transColor);
}
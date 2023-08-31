#include "shader/util.glsl"
#include "shader/oit.glsl"

layout (early_fragment_tests) in;

layout(bindless_image, r32ui) uniform uimage2D headPointers; // clear to 0 each frame
layout(binding = 0, offset = 0) uniform atomic_uint indexDispenser; // clear to 0 each frame
layout(binding = 1, std430) buffer LinkedList {
	uvec4 nodes[];
};
uniform uint uMaxNodes; // 1920 * 1200 * MAX_LAYER
uniform BindlessSampler2D texBlds[MAX_TEX];
uniform float udotl;
uniform vec3 eyePos;

in vec2 vTexcoord;
flat in ivec4 vTexid;
in vec4 vWorldVertex;

const vec3 KCool = vec3(0.15, 0.15, 0.35);
const vec3 KWarm = vec3(0.9, 0.9, 0.25);

void render(vec4 color) {
	uint index = atomicCounterIncrement(indexDispenser);
	uint newHead = index + 1;
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
	vec4 textureColor = vTexid.x >= 0 ? texture(texBlds[vTexid.x], vTexcoord) : vec4(0.5);

	vec4 transColor = textureColor;
	transColor.rgb *= udotl;

	float depthView = length(vWorldVertex.xyz - eyePos);
#ifdef USE_CARTOON
	float fogFactor = -0.0000025;
	float darkness = 1.0;
	float threshold = 0.15;
	float cwFactor = step(darkness, threshold);
	vec3 kd = KCool * cwFactor + KWarm * (1.0 - cwFactor);
	transColor.rgb *= kd;
#else
	float fogFactor = -0.00000075;
#endif
	transColor.rgb = GenFogColor(fogFactor, vWorldVertex, depthView, udotl, transColor.rgb);
	transColor.a = 0.5;

	render(transColor);
}
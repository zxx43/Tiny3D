#include "shader/oit.glsl"
#include "shader/util.glsl"

layout (early_fragment_tests) in;

layout(bindless_image, r32ui) uniform uimage2D headPointers; // clear to 0 each frame
layout(binding = 0, offset = 0) uniform atomic_uint indexDispenser; // clear to 0 each frame
layout(binding = 0, std430) buffer LinkedList {
	uvec4 nodes[];
};
uniform uint uMaxNodes; // 1920 * 1200 * MAX_LAYER
uniform BindlessSampler2D texBlds[MAX_TEX];
uniform float udotl;
uniform vec3 eyePos;

in vec2 vTexcoord;
flat in ivec4 vTexid;
in vec4 vWorldVertex;
in vec3 vNormal;
in mat3 vTBN;

out vec4 FragOut;

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
	vec4 textureColor = vTexid.x >= 0 ? texture(texBlds[vTexid.x], vTexcoord) : vec4(0.5);

	vec4 transColor = vec4(1.0);
	transColor.a = textureColor.a;

	float depthView = length(vWorldVertex.xyz - eyePos);
	transColor.rgb = GenFogColor(-0.00000075, vWorldVertex, depthView, udotl, textureColor.rgb);

	render(transColor);

	//vec3 normal = vTexid.y >= 0 ? GetNormalFromMap(texBlds[vTexid.y], vTexcoord, vTBN) : vNormal;
	//normal = normalize(normal) * 0.5 + 0.5;
	FragOut = vec4(vec3(0.0), 1.0);
}
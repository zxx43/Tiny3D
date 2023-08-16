#include "shader/util.glsl"
#include "shader/oit.glsl"

layout(bindless_image, r32ui) uniform uimage2D headPointers;
layout(binding = 1, std430) buffer LinkedList {
	uvec4 nodes[];
};
uniform BindlessSampler2D colorBuffer;

in vec2 vTexcoord;

out vec4 FragColor;

vec4 blend(vec4 baseColor) {
	uvec4 frags[MAX_LAYER_STORED];
	int layer = 0;
	uint next = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;
	while (next != 0 && layer < MAX_LAYER_STORED) {
		frags[layer] = nodes[next - 1];
		next = frags[layer].w;
		++layer;
	}
	
	if (layer > 1) Sort(frags, layer);

	vec4 resColor = baseColor;
	for (int i = 0; i < layer; ++i) {  
		vec4 color = UnpackColor(frags[i].xy);
		resColor.rgb = mix(resColor.rgb, color.rgb, color.a);
		resColor.a = uintBitsToFloat(frags[i].z);
	}
	return resColor;
}

void main() {
	vec4 baseColor = texture(colorBuffer, vTexcoord);
	FragColor = blend(baseColor);

	// gamma correction
	#ifdef HIGH_QUALITY
		FragColor.rgb = vec3(1.0) - exp(-FragColor.rgb * 2.5);
	#endif
	FragColor.rgb = pow(FragColor.rgb, INV_GAMMA);
}
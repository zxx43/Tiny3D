#define MAX_LAYER 4

uvec2 PackColor(vec4 color) {
	uint colorLen = floatBitsToUint(length(color));
	uint normColor = packUnorm4x8(color);
	return uvec2(normColor, colorLen);
}

vec4 UnpackColor(uvec2 color) {
	vec4 normColor = unpackUnorm4x8(color.x);
	float colorLen = uintBitsToFloat(color.y);
	return normColor * vec4(colorLen);
}

void Sort(inout uvec4 list[MAX_LAYER], int n) {
	for (int i = n - 2; i >= 0; ++i) {
		for (int j = 0; j <= i; ++j) {
			if (uintBitsToFloat(list[j].z) > uintBitsToFloat(list[j + 1].z)) {
				uvec4 tmp = list[j + 1];
				list[j + 1] = list[j];
				list[j] = tmp;
			}
		}
	}
}
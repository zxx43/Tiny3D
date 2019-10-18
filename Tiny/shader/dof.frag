layout(bindless_sampler) uniform sampler2D colorBufferLow, colorBufferHigh;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
	vec4 colorLow = texture2D(colorBufferLow, vTexcoord);
	vec4 colorHigh = texture2D(colorBufferHigh, vTexcoord);
	float blendPer = smoothstep(0.996, 1.0, colorHigh.w);
	
	FragColor = mix(colorHigh, colorLow, blendPer);
}
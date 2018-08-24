#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec3 vTexcoord;
in vec2 projDepth;

layout (location = 0) out vec4 FragColor;

void main() {
	//float alpha = vTexcoord.p >= 0.0 ? texture2DArray(texture, vTexcoord).a : 1.0;
	float alpha = texture2DArray(texture, vTexcoord).a;
	if(alpha < 0.1) discard;

	float depth = (projDepth.x / projDepth.y) * 0.5 + 0.5;
	FragColor = vec4(depth);
}
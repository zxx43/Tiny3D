#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec3 vTexcoord;
in vec2 vProjDepth;

layout (location = 0) out vec4 FragColor;

void main() {
	float alpha = texture2DArray(texture, vTexcoord).r;
	if(alpha < 0.1) discard;

	float depth = (vProjDepth.x / vProjDepth.y) * 0.5 + 0.5;
	FragColor = vec4(depth);
}
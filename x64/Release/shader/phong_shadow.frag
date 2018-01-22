#version 330
#extension GL_EXT_gpu_shader4 : enable 

uniform sampler2DArray texture;

in vec2 vTexcoord;
flat in float vTexid;
in vec4 projPosition;

layout (location = 0) out vec4 FragColor;

void main() {
	vec3 texcoord = vec3(vTexcoord, vTexid);
	if(texcoord.p >= 0.0) {
		float alpha = texture2DArray(texture, texcoord).a;
		if(alpha < 0.1) discard;
	}

	float depth = projPosition.z / projPosition.w;
	depth = depth * 0.5 + 0.5;
	float depth2 = depth * depth;
	FragColor = vec4(depth, depth2, 0.0, 1.0);
}
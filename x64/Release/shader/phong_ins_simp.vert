#version 330

uniform mat4 viewProjectMatrix;
uniform float time, isGrass;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec3 color;
layout (location = 4) in vec4 modelTrans;

out vec4 vTexcoord;
flat out vec3 vColor;
out vec3 vNormal;

#define COLOR_SCALE vec3(0.003, 0.006, 0.005)

void main() {
	vColor = COLOR_SCALE * color;

	vec4 worldVertex = vec4(modelTrans.w * vertex + modelTrans.xyz, 1.0);
	if(isGrass > 0.5 && vertex.y > 0.1)
		worldVertex.x += sin(time);

	vNormal = normal;
	vTexcoord = texcoord;
	gl_Position = viewProjectMatrix * worldVertex;
}
#version 330

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec4 texcoord;
layout (location = 4) in vec4 modelTrans;

out VertexData {
	vec4 vTexcoord;
} vertOut;

void main() {
	vertOut.vTexcoord = texcoord;
	
	vec4 worldVertex = vec4(modelTrans.w * vertex + modelTrans.xyz, 1.0);
	gl_Position = viewProjectMatrix * worldVertex;
}
#version 330

uniform mat4 viewProjectMatrix;
uniform mat3x4 modelMatrices[100];

layout (location = 0) in vec3 vertex;
layout (location = 4) in float objectid;

out vec2 projDepth;

mat4 convertMat(mat3x4 srcMat) {
	mat4x3 transMat = transpose(srcMat);
	return mat4(transMat[0], 0.0, 
				transMat[1], 0.0, 
				transMat[2], 0.0, 
				transMat[3], 1.0);
}

void main() {
	vec4 worldVertex = convertMat(modelMatrices[int(objectid)]) * vec4(vertex, 1.0);
	gl_Position = viewProjectMatrix * worldVertex;
	projDepth = gl_Position.zw;
}
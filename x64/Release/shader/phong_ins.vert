#version 330

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec3 color;
layout (location = 4) in mat3x4 modelMatrix;

out vec4 vTexcoord;
flat out vec3 vColor;
out vec3 vNormal;

#define COLOR_SCALE vec3(0.003, 0.006, 0.005)

mat4 convertMat(mat3x4 srcMat) {
	mat4x3 transMat = transpose(srcMat);
	return mat4(transMat[0], 0.0, 
				transMat[1], 0.0, 
				transMat[2], 0.0, 
				transMat[3], 1.0);
}

void main() {
	vColor = COLOR_SCALE * color;

	mat4x3 tranMat = transpose(modelMatrix);
	mat3 matRot = mat3(tranMat);
	vec4 worldVertex = vec4(matRot * vertex + tranMat[3], 1.0);
	vNormal = matRot * normal;

	vTexcoord = texcoord;
	gl_Position = viewProjectMatrix * worldVertex;
}
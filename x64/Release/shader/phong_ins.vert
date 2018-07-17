#version 330

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;
layout (location = 3) in vec3 color;
layout (location = 4) in mat3x4 modelMatrix;

out vec3 vTexcoord;
out vec3 vColor;
out vec3 vNormal;

mat4 convertMat(mat3x4 srcMat) {
	mat4x3 transMat = transpose(srcMat);
	return mat4(transMat[0], 0.0, 
				transMat[1], 0.0, 
				transMat[2], 0.0, 
				transMat[3], 1.0);
}

void main() {
	vColor = vec3(0.6, 1.2, 1.0) * color * 0.005;

	mat4 matModel = convertMat(modelMatrix);
	vec4 worldVertex = matModel * vec4(vertex, 1.0);
	vNormal = mat3(matModel) * normal;
	
	vTexcoord = texcoord;
	gl_Position = viewProjectMatrix * worldVertex;
}
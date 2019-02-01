#version 330

uniform mat3x4 modelMatrices[100];
uniform mat4 viewProjectMatrix;
uniform vec4 texPixel;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 4) in vec3 color;
layout (location = 5) in float objectid;

out vec2 vTexcoord;
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
	
	mat4 matModel = convertMat(modelMatrices[int(objectid)]);
	vec4 worldVertex = matModel * vec4(vertex, 1.0);
	vNormal = mat3(matModel) * normal;
	
	float tx = (texcoord.x * texPixel.z + texcoord.z) * texPixel.x;
	float ty = (texcoord.y * texPixel.w + texcoord.w) * texPixel.y;

	vTexcoord = vec2(tx, ty);
	gl_Position = viewProjectMatrix * worldVertex;
}
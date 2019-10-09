uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec2 texid;
layout (location = 4) in vec3 color;
layout (location = 5) in vec3 tangent;
layout (location = 6) in mat3x4 modelMatrix;

out vec2 vTexcoord;
flat out vec4 vTexid;
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;

#define COLOR_SCALE vec3(0.003, 0.006, 0.005)

mat4 convertMat(mat3x4 srcMat) {
	mat4x3 transMat = transpose(srcMat);
	return mat4(transMat[0], 0.0, 
				transMat[1], 0.0, 
				transMat[2], 0.0, 
				transMat[3], 1.0);
}

mat3 GetTBN(vec3 normal, vec3 tangent) {
	vec3 bitangent = cross(normal, tangent);
	return mat3(tangent, bitangent, normal);
}

void main() {
	mat4x3 tranMat = transpose(modelMatrix);
	mat3 matRot = mat3(tranMat);
	vec4 worldVertex = vec4(matRot * vertex + tranMat[3], 1.0);
#ifndef ShadowPass
	vNormal = matRot * normal;
	vTBN = matRot * GetTBN(normalize(normal), normalize(tangent));
	vColor = COLOR_SCALE * color;
#endif
#ifndef LowPass
	vTexcoord = texcoord.xy;
	vTexid = vec4(texcoord.zw, texid);
#endif
	gl_Position = viewProjectMatrix * worldVertex;
}
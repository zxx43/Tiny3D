#include "shader/util.glsl"

uniform mat3x4 modelMatrices[100];
uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 4) in vec3 color;
layout (location = 6) in float objectid;

flat out vec3 vColor;
out vec3 vNormal;

void main() {
	vColor = vec3(0.6, 1.2, 1.0) * color.rgb * 0.005;
	
	mat4 matModel = convertMat(modelMatrices[int(objectid)]);
	vec4 worldVertex = matModel * vec4(vertex, 1.0);
	vNormal = mat3(matModel) * normal;
	
	gl_Position = viewProjectMatrix * worldVertex;
}
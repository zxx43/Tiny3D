#include "shader/util.glsl"

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec2 vertex;
layout (location = 2) in mat4 trans;

out vec4 vNormalHeight;

void main() {
	mat3x4 transM = mat3x4(trans[0], trans[1], trans[2]);
	mat4 transMat = convertMat(transM);

	vNormalHeight = vec4(trans[3].xyz, vertex.y);
	gl_Position = viewProjectMatrix * transMat * vec4(vertex, 0.0, 1.0);
}
uniform mat4 viewMatrix;
uniform vec3 eyePos;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

out vec4 vPosition;
out vec3 vNormal;
out float vBack;

void main() {
	vPosition.xyz = vertex;
	vPosition.w = (viewMatrix * vec4(vertex, 1.0)).z;
	vNormal = normal;
	vBack = dot(normal, normalize(eyePos - vertex));
}
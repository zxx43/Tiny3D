#include "shader/util.glsl"

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec2 vertex;
layout (location = 1) in mat4 trans;
layout (location = 2) in vec4 normal;

out vec3 vNormal;
out vec3 vColor;

#define BottomColor vec3(0.005, 0.04, 0.01)
#define TopColor vec3(0.1, 0.2, 0.05)

void main() {
	vNormal = normalize(normal.xyz) * 0.5 + 0.5;
	vColor = mix(BottomColor, TopColor, vertex.y);
	gl_Position = trans * vec4(vertex, 0.0, 1.0);
}
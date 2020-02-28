#include "shader/util.glsl"

uniform mat4 viewMatrix;
uniform mat4 projectMatrix;

layout (location = 0) in vec3 vertex;

out vec3 texCoord;

void main() {
	mat4 skyView = viewMatrix;
	skyView[3] = CenterPosition;
	mat4 vp = projectMatrix * skyView;
	gl_Position = (vp * vec4(vertex, 1.0)).xyww;
	texCoord = vec3(vertex.x, -vertex.yz);
}
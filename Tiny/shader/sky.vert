#include "shader/util.glsl"

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;

out vec3 texCoord;

void main() {
	gl_Position = (viewProjectMatrix * vec4(vertex, 1.0)).xyww;
	texCoord = vec3(vertex.x, -vertex.yz);
}
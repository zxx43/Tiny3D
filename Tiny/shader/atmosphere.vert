uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;

out vec3 vViewDir;

void main() {
	vViewDir = vertex;
	gl_Position = viewProjectMatrix * vec4(vertex, 1.0);
}
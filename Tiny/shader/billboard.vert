uniform mat4 viewProjectMatrix;
uniform vec3 viewRight;

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec4 texcoord;
layout (location = 6) in mat4 modelMatrix;

out vec2 vTexcoord;
flat out vec4 vTexid;

void main() {
	vec3 position = modelMatrix[0].xyz;
	vec3 board = modelMatrix[1].xyz;

	vec2 size = vertex.xy * board.xy;
	vec3 right = size.x * viewRight;
#ifdef ShadowPass
	vec3 top = vec3(0.0, size.y, 0.0);
#else
	vec3 top = size.y * UP_VEC3;
#endif
	vec3 worldVertex = position + right + top;

	vTexcoord = texcoord.xy; 
	vTexid = vec4(texcoord.zw, 0.0, 0.0);
	gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
}
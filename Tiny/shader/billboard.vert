uniform mat4 viewProjectMatrix;
uniform vec3 viewRight;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec4 texcoord;
layout (location = 2) in vec3 position;
layout (location = 3) in vec4 board;

out vec2 vTexcoord;
flat out vec4 vTexid;

#define TOP_VEC vec3(0.0, 1.0, 0.0)

void main() {
	vec2 size = vertex.xy * board.xy;
	vec3 right = size.x * viewRight;

#ifdef ShadowPass
	vec3 top = vec3(0.0, size.y, 0.0);
#else
	vec3 top = size.y * TOP_VEC;
#endif

	vec3 worldVertex = position + right + top;

	vTexcoord = texcoord.xy; 
	vTexid = vec4(texcoord.zw, 0.0, 0.0);
	gl_Position = viewProjectMatrix * vec4(worldVertex, 1.0);
}
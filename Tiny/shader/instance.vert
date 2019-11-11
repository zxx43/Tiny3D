uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec2 texid;
layout (location = 4) in vec3 color;
layout (location = 5) in vec3 tangent;
layout (location = 6) in mat4 modelMatrix;

#ifndef LowPass
out vec2 vTexcoord;
flat out vec4 vTexid;
#endif
#ifndef ShadowPass
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;
#endif

void main() {
	mat3 matRot = mat3(modelMatrix);
	vec4 worldVertex = modelMatrix * vec4(vertex, 1.0);
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
uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec2 texid;
layout (location = 4) in vec3 color;
layout (location = 5) in vec3 tangent;
layout (location = 6) in vec4 modelTrans;

out vec2 vTexcoord;
flat out vec4 vTexid;
#ifndef ShadowPass
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;
#endif

void main() {
	vec4 worldVertex = vec4(modelTrans.w * vertex + modelTrans.xyz, 1.0);
#ifndef ShadowPass
	vColor = COLOR_SCALE * color;
	vNormal = normal;
	vTBN = GetTBN(normalize(normal), normalize(tangent));
#endif
	vTexcoord = texcoord.xy;
	vTexid = vec4(texcoord.zw, texid);
	gl_Position = viewProjectMatrix * worldVertex;
}
#include "shader/util.glsl"

#ifdef UseDebug
layout(binding = 1, std430) buffer InMaterial {
	Material inMaterials[];
};
#endif

uniform mat4 viewProjectMatrix;
uniform vec3 mapTrans, mapScale;
uniform vec4 mapInfo;

#ifdef UseDebug
uniform int uDebugMid;
#endif

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec4 texid;
layout (location = 4) in vec3 color;
layout (location = 5) in vec3 tangent;

out vec2 vTexcoord;
flat out vec2 vRMid;
flat out vec4 vTexid;
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;
out vec4 vWorldVert;

void main() {
	vColor = vec3(0.1, 1.8, 1.0) * color.rgb * 0.005;

	vec4 worldVertex = vec4(vertex, 1.0);
	vec2 coord = (worldVertex.xz - mapTrans.xz) / (mapScale.xz * mapInfo.zw);

	vWorldVert = vec4(coord.x, worldVertex.y, coord.y, worldVertex.w);
	vNormal = normal;
	vTBN = GetTBN(normalize(normal), normalize(tangent));
	
	vTexcoord = texcoord.xy;
	vRMid = texcoord.zw;

#ifndef UseDebug
	vTexid = texid;
#else
	vTexid = inMaterials[uDebugMid].texids.xxxx;
#endif

	gl_Position = viewProjectMatrix * worldVertex;
}
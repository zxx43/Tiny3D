#include "shader/util.glsl"

uniform mat3x4 modelMatrices[100];
uniform mat4 viewProjectMatrix;

#ifdef ShadowPass
uniform mat4 projectMatrix, viewMatrix;
uniform vec2 camPara;
#endif

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec4 texid;
layout (location = 4) in vec3 color;
layout (location = 5) in vec3 tangent;
layout (location = 6) in float objectid;

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
	mat4 matModel = convertMat(modelMatrices[int(objectid)]);
	vec4 worldVertex = matModel * vec4(vertex, 1.0);
#ifndef ShadowPass 
	mat3 normalMat = mat3(matModel);
	vNormal = normalMat * normal;
	vTBN = normalMat * GetTBN(normalize(normal), normalize(tangent));
	vColor = COLOR_SCALE * color;
#endif 
#ifndef LowPass
	vTexcoord = texcoord.xy;
	vTexid = texid;
#endif

#ifdef ShadowPass
	gl_Position = DepthToLinear(viewProjectMatrix, projectMatrix, viewMatrix, camPara.x, camPara.y, worldVertex);
#else
	gl_Position = viewProjectMatrix * worldVertex;
#endif
}
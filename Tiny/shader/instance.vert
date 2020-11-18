#include "shader/util.glsl"

uniform mat4 viewProjectMatrix;
#ifdef BillPass
uniform vec3 viewRight;
#endif

#ifdef ShadowPass
uniform mat4 projectMatrix, viewMatrix;
uniform vec2 camPara;
#endif

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec2 texid;
layout (location = 4) in vec3 color;
layout (location = 5) in vec3 tangent;
layout (location = 8) in mat4 modelMatrix;

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
#ifndef BillPass
		#ifndef ShadowPass
			mat3 matRot = mat3(modelMatrix);
			vNormal = matRot * normal;
			vTBN = matRot * GetTBN(normalize(normal), normalize(tangent));
			vColor = COLOR_SCALE * color;
		#endif
		#ifndef LowPass
			vTexcoord = texcoord.xy;
			vTexid = vec4(texcoord.zw, texid);
		#endif
		vec4 worldVertex = modelMatrix * vec4(vertex, 1.0);
#else
		vec3 position = modelMatrix[0].xyz;
		vec3 board = modelMatrix[1].xyz;
		vec2 size = vertex.xy * board.xy;
		vec3 right = size.x * viewRight;
		#ifdef ShadowPass
			vec3 top = vec3(0.0, size.y, 0.0);
		#else
			vec3 top = size.y * UP_VEC3;
		#endif
		#ifndef LowPass
			vTexcoord = texcoord.xy; 
			vTexid = vec4(texcoord.zw, 0.0, 0.0);
		#endif
		vec4 worldVertex = vec4(position + right + top, 1.0);
#endif

#ifdef ShadowPass
	gl_Position = DepthToLinear(viewProjectMatrix, projectMatrix, viewMatrix, camPara.x, camPara.y, worldVertex);
#else
	gl_Position = viewProjectMatrix * worldVertex;
#endif
}
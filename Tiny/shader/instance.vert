#include "shader/util.glsl"

layout(binding = 1, std140) uniform InMaterial {
	Material inMaterials[MAX_MAT];
};

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
layout (location = 3) in vec3 tangent;
layout (location = 6) in mat4 modelMatrix;

#ifndef LowPass
out vec2 vTexcoord;
flat out ivec4 vTexid;
#endif
#ifndef ShadowPass
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;
#endif

void main() {
	float insMatid = modelMatrix[3].w;
	int mid = (insMatid.x > -0.001) ? int(insMatid.x) : int(texcoord.z);
	Material material = inMaterials[mid];

	mat4 worldMatrix = modelMatrix;
	worldMatrix[3].w = 1.0;

#ifndef BillPass
		#ifndef ShadowPass
			mat3 matRot = mat3(worldMatrix);
			if(material.texids.y < -0.01) {
				vNormal = matRot * normal;
				vTBN = mat3(1.0);
			} else {
				vNormal = vec3(0.0, 1.0, 0.0);
				vTBN = matRot * GetTBN(normalize(normal), normalize(tangent));
			}
			vColor = material.params.rgb;
		#endif
		#ifndef LowPass
			vTexcoord = texcoord.xy;
			vTexid = ivec4(material.texids);
		#endif
		vec4 worldVertex = worldMatrix * vec4(vertex, 1.0);
#else
		vec3 position = worldMatrix[0].xyz;
		vec3 board = worldMatrix[1].xyz;
		vec2 size = vertex.xy * board.xy;
		vec3 right = size.x * viewRight;
		#ifdef ShadowPass
			vec3 top = vec3(0.0, size.y, 0.0);
		#else
			vec3 top = size.y * UP_VEC3;
		#endif
		#ifndef LowPass
			vTexcoord = texcoord.xy; 
			vTexid = ivec4(ivec2(material.texids.xy), 0, 0);
		#endif
		vec4 worldVertex = vec4(position + right + top, 1.0);
#endif

#ifdef ShadowPass
	gl_Position = DepthToLinear(viewProjectMatrix, projectMatrix, viewMatrix, camPara.x, camPara.y, worldVertex);
#else
	gl_Position = viewProjectMatrix * worldVertex;
#endif
}
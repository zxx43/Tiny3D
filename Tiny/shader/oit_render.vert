#include "shader/util.glsl"

layout(binding = 1, std140) uniform InMaterial {
	Material inMaterials[MAX_MAT];
};

uniform mat4 viewProjectMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 6) in mat4 modelMatrix;

out vec2 vTexcoord;
flat out ivec4 vTexid;
flat out vec3 vColor;
out vec4 vWorldVertex;
out vec3 vNormal;
out mat3 vTBN;

void main() {
	float insMatid = modelMatrix[3].w;
	int mid = (insMatid.x > -0.001) ? int(insMatid.x) : int(texcoord.z);
	Material material = inMaterials[mid];

	mat4 worldMatrix = modelMatrix;
	worldMatrix[3].w = 1.0;

	mat3 matRot = mat3(worldMatrix);
	if(material.texids.y < -0.01) {
		vNormal = matRot * normal;
		vTBN = mat3(1.0);
	} else {
		vNormal = vec3(0.0, 1.0, 0.0);
		vTBN = matRot * GetTBN(normalize(normal), normalize(tangent));
	}
	vColor = material.params.rgb;
	vTexcoord = texcoord.xy;
	vTexid = ivec4(material.texids);
	vWorldVertex = worldMatrix * vec4(vertex, 1.0);

	gl_Position = viewProjectMatrix * vWorldVertex;
}
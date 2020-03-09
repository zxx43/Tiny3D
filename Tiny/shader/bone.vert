#include "shader/util.glsl"
#include "shader/vtf.glsl"

uniform mat4 viewProjectMatrix;
layout(bindless_sampler) uniform sampler2D boneTex[MAX_BONE_TEX];

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec2 texid;
layout (location = 4) in vec3 color;
layout (location = 5) in vec3 tangent;
layout (location = 6) in vec4 boneids;
layout (location = 7) in vec4 weights;
layout (location = 8) in mat4 modelMatrix;

#ifndef ShadowPass
out vec2 vTexcoord;
flat out vec4 vTexid;
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;
#endif

void main() {	
	int aid = int(floor(modelMatrix[3].x));
	sampler2D bone = boneTex[aid];
	vec2 boneSize = vec2(textureSize(bone, 0));
	vec4 size = vec4(boneSize, vec2(0.25) / boneSize);
	float curr = floor(modelMatrix[3].y) / size.y;
	size.x = 1.0 / size.x;

	mat3x4 m0 = GetBoneTex(bone, boneids.x, size, curr);
	mat3x4 m1 = GetBoneTex(bone, boneids.y, size, curr);
	mat3x4 m2 = GetBoneTex(bone, boneids.z, size, curr);
	mat3x4 m3 = GetBoneTex(bone, boneids.w, size, curr);

	mat4 boneMat  = convertMat(m0) * weights.x;
		 boneMat += convertMat(m1) * weights.y;
		 boneMat += convertMat(m2) * weights.z;
		 boneMat += convertMat(m3) * weights.w;
    
    vec4 position = boneMat * vec4(vertex, 1.0);
	mat4 modelMat = convertMat(mat3x4(modelMatrix[0], modelMatrix[1], modelMatrix[2]));

#ifndef ShadowPass
	vColor = MatScale * color * 0.005;
	mat3 matRot = mat3(modelMat);
	mat3 normalMat = matRot * mat3(boneMat);
	vNormal = normalMat * normal;
	vTBN = normalMat * GetTBN(normal, tangent);
	vTexcoord = texcoord.xy;
	vTexid = vec4(texcoord.zw, texid);
#endif

	vec4 modelPosition = modelMat * position;
	gl_Position = viewProjectMatrix * modelPosition;
}

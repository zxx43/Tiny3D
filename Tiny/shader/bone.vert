uniform mat4 viewProjectMatrix;
uniform mat4 uModelMatrix;
uniform mat3x4 boneMats[MAX_BONE];

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 texcoord;
layout (location = 3) in vec2 texid;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 color;
layout (location = 6) in vec4 boneids;
layout (location = 7) in vec4 weights;

#ifndef ShadowPass
out vec2 vTexcoord;
flat out vec4 vTexid;
flat out vec3 vColor;
out vec3 vNormal;
out mat3 vTBN;
#endif

void main() {	
	mat4 boneMat = convertMat(boneMats[int(boneids.x)]) * weights.x;
    boneMat += convertMat(boneMats[int(boneids.y)]) * weights.y;
    boneMat += convertMat(boneMats[int(boneids.z)]) * weights.z;
    boneMat += convertMat(boneMats[int(boneids.w)]) * weights.w;
    
    vec4 position = boneMat * vec4(vertex, 1.0);

	vec3 translate = uModelMatrix[0].xyz;
	float scale = uModelMatrix[0].w;
	vec4 rotate = uModelMatrix[1];
	mat4 modelMat = Translate(translate) * QuatToMat4(rotate) * Scale(scale);

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

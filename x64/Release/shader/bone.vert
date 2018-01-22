#version 330

uniform mat4 viewMatrix;
uniform mat4 projectMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform mat3x4 boneMats[100];
uniform mat4 lightViewProjNear, lightViewProjMid, lightViewProjFar;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;
layout (location = 3) in vec3 color;
layout (location = 4) in vec4 boneids;
layout (location = 5) in vec4 weights;

out vec3 vTexcoord;
flat out vec3 vColor;
out vec3 vNormal;
out vec4 projPosition;
out vec4 viewPosition;
out vec4 lightNearPosition,lightMidPosition,lightFarPosition;

mat4 convertMat(mat3x4 srcMat) {
	vec4 col1 = srcMat[0];
	vec4 col2 = srcMat[1];
	vec4 col3 = srcMat[2];
	vec4 row1 = vec4(col1.x, col2.x, col3.x, 0);
	vec4 row2 = vec4(col1.y, col2.y, col3.y, 0);
	vec4 row3 = vec4(col1.z, col2.z, col3.z, 0);
	vec4 row4 = vec4(col1.w, col2.w, col3.w, 1);
	return mat4(row1, row2, row3, row4);
}

void main() {	
	mat4 boneMat = convertMat(boneMats[int(boneids.x)]) * weights.x;
    boneMat += convertMat(boneMats[int(boneids.y)]) * weights.y;
    boneMat += convertMat(boneMats[int(boneids.z)]) * weights.z;
    boneMat += convertMat(boneMats[int(boneids.w)]) * weights.w;
    
    vec4 position = boneMat * vec4(vertex, 1.0);
    vec3 normal3 = (boneMat * vec4(normal, 0.0)).xyz;
    normal3 = uNormalMatrix * normal3;

	vColor = color * 0.004;
	vNormal = normal3;
	
	vTexcoord = texcoord; 
	vec4 modelPosition = uModelMatrix * position;
	viewPosition = viewMatrix * modelPosition;
	gl_Position = projectMatrix * viewPosition;
	projPosition = gl_Position;

	lightNearPosition = lightViewProjNear * modelPosition;
	lightMidPosition = lightViewProjMid * modelPosition;
	lightFarPosition = lightViewProjFar * modelPosition;
}

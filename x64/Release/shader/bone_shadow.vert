#version 330

uniform mat4 viewProjectMatrix;
uniform mat4 uModelMatrix;
uniform mat3x4 boneMats[100];

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec4 boneids;
layout (location = 2) in vec4 weights;

out vec2 projDepth;

mat4 convertMat(mat3x4 srcMat) {
	vec4 col1 = srcMat[0];
	vec4 col2 = srcMat[1];
	vec4 col3 = srcMat[2];
	vec4 row1 = vec4(col1.x, col2.x, col3.x, 0.0);
	vec4 row2 = vec4(col1.y, col2.y, col3.y, 0.0);
	vec4 row3 = vec4(col1.z, col2.z, col3.z, 0.0);
	vec4 row4 = vec4(col1.w, col2.w, col3.w, 1.0);
	return mat4(row1, row2, row3, row4);
}

void main() {	
	mat4 boneMat = convertMat(boneMats[int(boneids.x)]) * weights.x;
    boneMat += convertMat(boneMats[int(boneids.y)]) * weights.y;
    boneMat += convertMat(boneMats[int(boneids.z)]) * weights.z;
    boneMat += convertMat(boneMats[int(boneids.w)]) * weights.w;
    
    vec4 position = boneMat * vec4(vertex, 1.0);
	
	vec4 modelPosition = uModelMatrix * position;
	gl_Position = viewProjectMatrix * modelPosition;
	projDepth = gl_Position.zw;
}

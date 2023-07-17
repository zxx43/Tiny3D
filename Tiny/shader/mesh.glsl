struct MeshGroup {
	ivec4 normalMeshs; // high/middle/low/animat mid
	ivec4 singleMeshs; // high/middle/low/billbd mid
	ivec4 transpMeshs; // high/middle/low/ex mid
};

struct CountBuff {
	uvec4 countBuff;
	uvec4 exCountBuff;
};

bool MidValid(int mid) {
	return mid >= 0;
}

struct Bounding {
	vec4 bv[8];
};

struct GroupObject {
	vec4 trans;  // translate x,y,z scale x
	vec3 rotat;  // encoded quat x,y,z
	float extra; // billboardid | uniform scale
	vec4 bound;  // boundbox
	vec4 mesh;   // scale y,z groupid mtlid | aid aframes groupid mtlid 
};

float GetGroupId(GroupObject gObject) {
	return gObject.mesh.z;
}

float GetMtl(GroupObject gObject) {
	return gObject.mesh.w;
}

float GetScaleMain(GroupObject gObject) {
	return gObject.trans.w;
}

vec2 GetScaleOther(GroupObject gObject) {
	return gObject.mesh.xy;
}

vec2 GetAnimInfo(GroupObject gObject) {
	return gObject.mesh.xy;
}

float GetBillid(GroupObject gObject) {
	return gObject.extra;
}

bool UniformScale(GroupObject gObject) {
	return gObject.extra > 0.0;
}

struct MeshObject {
	vec3 position; // translate x,y,z
	float billid;  // billboardid
	vec4 quat;     // quat x,y,z,w
	vec3 scale;    // scale
	float mtl;     // mtlid
	vec4 meshid;   // nmeshid smeshid bmeshid ameshid | aid aframes bmeshid ameshid
	vec4 exMeshid; // tmeshid
};

int GetNormalMid(MeshObject object) {
	return int(object.meshid.x);
}

int GetSingleMid(MeshObject object) {
	return int(object.meshid.y);
}

int GetBillbdMid(MeshObject object) {
	return int(object.meshid.z);
}

int GetAnimatMid(MeshObject object) {
	return int(object.meshid.w);
}

int GetTranspMid(MeshObject object) {
	return int(object.exMeshid.x);
}

vec2 GetAnimatInfo(MeshObject object) {
	return object.meshid.xy;
}

struct DispatchIndirect {
	uint numx;
	uint numy;
	uint numz;
};

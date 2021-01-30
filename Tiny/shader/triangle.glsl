ivec2 CaculateBlock(vec2 pos, vec2 offset, vec2 offsize, float invStepSize) {
	vec2 off = pos - offset;
	off /= offsize;
	ivec2 block = ivec2(off * invStepSize);
	return block;
}

int CauculateTrIndex(ivec2 block, float lineSize) {
	int ib = block.y * int(lineSize) + block.x;
	int it = ib * 2;
	return it;
}

bool CheckIn(vec2 point, vec3 pa, vec3 pb, vec3 pc) {
	vec2 v0 = pc.xz - pb.xz;
	vec2 v1 = pa.xz - pb.xz;
	vec2 v2 = point - pb.xz;
	
	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot02 = dot(v0, v2);
	float dot11 = dot(v1, v1);
	float dot12 = dot(v1, v2);
	float inverDeno = 1.0 / (dot00 * dot11 - dot01 * dot01);
	
	float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
	if (u < 0.0 || u > 1.0) return false;
	
	float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
	if (v < 0.0 || v > 1.0) return false;
	
	return u + v <= 1.0;
}

vec3 CaculateNormal(vec3 pa, vec3 pb, vec3 pc) {
	vec3 ba = pa - pb;
	vec3 bc = pc - pb;
	return cross(bc, ba);
}

float CaculateY(vec2 pose, vec3 pa, vec3 pb, vec3 pc) {
	vec3 normal = CaculateNormal(pa, pb, pc);
	float pd = -dot(normal, pa);
	return -(pd + normal.x * pose.x + normal.z * pose.y) / normal.y;
}

float CaculateY(vec2 pose, vec4 plan) {
	return -(plan.w + plan.x * pose.x + plan.z * pose.y) / plan.y;
}

vec3 CaculateBary(vec3 point, vec3 pa, vec3 pb, vec3 pc) {
	vec3 v0 = pc - pb;
	vec3 v1 = pa - pb;
	vec3 v2 = point - pb;

	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot02 = dot(v0, v2);
	float dot11 = dot(v1, v1);
	float dot12 = dot(v1, v2);
	float inverDeno = 1.0 / (dot00 * dot11 - dot01 * dot01);
	
	float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
	float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
	return vec3(u, v, 1.0 - (u + v));
}

vec3 CaculateBary(vec2 point, vec2 pa, vec2 pb, vec2 pc) {
	vec2 v0 = pc - pb;
	vec2 v1 = pa - pb;
	vec2 v2 = point - pb;

	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot02 = dot(v0, v2);
	float dot11 = dot(v1, v1);
	float dot12 = dot(v1, v2);
	float inverDeno = 1.0 / (dot00 * dot11 - dot01 * dot01);
	
	float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
	float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
	return vec3(u, v, 1.0 - (u + v));
}
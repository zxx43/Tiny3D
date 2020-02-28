const uint MAX_BONE_TEX = 50;

mat3x4 GetBoneTex(sampler2D bone, float boneid, vec4 size, float curr) {	
	vec2 t0 = vec2((boneid * 3.0 + 0.0) * size.x, curr) + size.zw;
	vec2 t1 = vec2((boneid * 3.0 + 1.0) * size.x, curr) + size.zw;
	vec2 t2 = vec2((boneid * 3.0 + 2.0) * size.x, curr) + size.zw;

	vec4 f0 = texture(bone, t0);
	vec4 f1 = texture(bone, t1);
	vec4 f2 = texture(bone, t2);
	
	return mat3x4(f0, f1, f2);
}
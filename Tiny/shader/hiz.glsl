bool HizQuery(mat4 viewProjectMat, sampler2D depthTex, vec2 size, vec2 camParam, float maxLevel, 
	vec4 bv0, vec4 bv1, vec4 bv2, vec4 bv3, vec4 bv4, vec4 bv5, vec4 bv6, vec4 bv7) {
		vec4 b0 = viewProjectMat * bv0;
		vec4 b1 = viewProjectMat * bv1;
		vec4 b2 = viewProjectMat * bv2;
		vec4 b3 = viewProjectMat * bv3;
		vec4 b4 = viewProjectMat * bv4;
		vec4 b5 = viewProjectMat * bv5;
		vec4 b6 = viewProjectMat * bv6;
		vec4 b7 = viewProjectMat * bv7;
		vec4 inv1 = 1.0 / vec4(b0.w, b1.w, b2.w, b3.w);
		vec4 inv2 = 1.0 / vec4(b4.w, b5.w, b6.w, b7.w);
		
		vec3 ndc0 = b0.xyz * inv1.x;
		vec3 ndc1 = b1.xyz * inv1.y;
		vec3 ndc2 = b2.xyz * inv1.z;
		vec3 ndc3 = b3.xyz * inv1.w;
		vec3 ndc4 = b4.xyz * inv2.x;
		vec3 ndc5 = b5.xyz * inv2.y;
		vec3 ndc6 = b6.xyz * inv2.z;
		vec3 ndc7 = b7.xyz * inv2.w;

		vec3 maxNDC = max(ndc7, max(ndc6, max(ndc5, max(ndc4, max(ndc3, max(ndc2, max(ndc0, ndc1)))))));
		vec3 minNDC = min(ndc7, min(ndc6, min(ndc5, min(ndc4, min(ndc3, min(ndc2, min(ndc0, ndc1)))))));
		
		vec3 maxClip = clamp(maxNDC * 0.5 + 0.5, vec3(0.0), vec3(1.0));
		vec3 minClip = clamp(minNDC * 0.5 + 0.5, vec3(0.0), vec3(1.0));

		vec3 bound = maxClip - minClip;
		float edge = max(1.0, max(bound.x, bound.y) * max(size.x, size.y));
		float mip = min(ceil(log2(edge)), maxLevel);
		
		vec4 occ = vec4(textureLod(depthTex, maxClip.xy, mip).x, 
						textureLod(depthTex, minClip.xy, mip).x, 
						textureLod(depthTex, vec2(maxClip.x, minClip.y), mip).x, 
						textureLod(depthTex, vec2(minClip.x, maxClip.y), mip).x);

		float occDepth = max(occ.w, max(occ.z, max(occ.x, occ.y)));

		occDepth = Linearize(camParam.x, camParam.y, occDepth);	
		minClip.z = Linearize(camParam.x, camParam.y, minClip.z);

		return minClip.z > occDepth + 0.01;
}

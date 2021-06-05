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

		vec3 maxNDC, minNDC;
		maxNDC.x = max(ndc7.x, max(ndc6.x, max(ndc5.x, max(ndc4.x, max(ndc3.x, max(ndc2.x, max(ndc0.x, ndc1.x)))))));	
		maxNDC.y = max(ndc7.y, max(ndc6.y, max(ndc5.y, max(ndc4.y, max(ndc3.y, max(ndc2.y, max(ndc0.y, ndc1.y)))))));		
		minNDC.x = min(ndc7.x, min(ndc6.x, min(ndc5.x, min(ndc4.x, min(ndc3.x, min(ndc2.x, min(ndc0.x, ndc1.x)))))));	
		minNDC.y = min(ndc7.y, min(ndc6.y, min(ndc5.y, min(ndc4.y, min(ndc3.y, min(ndc2.y, min(ndc0.y, ndc1.y)))))));	
		minNDC.z = min(ndc7.z, min(ndc6.z, min(ndc5.z, min(ndc4.z, min(ndc3.z, min(ndc2.z, min(ndc0.z, ndc1.z)))))));
		
		vec3 maxClip, minClip;
		maxClip.xy = clamp(maxNDC.xy * 0.5 + 0.5, vec2(0.0), vec2(1.0));
		minClip = clamp(minNDC * 0.5 + 0.5, vec3(0.0), vec3(1.0));

		vec2 bound = maxClip.xy - minClip.xy;
		float edge = max(1.0, max(bound.x, bound.y) * max(size.x, size.y));
		float mip = min(ceil(log2(edge)), maxLevel);
		
		float occ1 = textureLod(depthTex, maxClip.xy, mip).x;
		float occ2 = textureLod(depthTex, minClip.xy, mip).x;
		float occ3 = textureLod(depthTex, vec2(maxClip.x, minClip.y), mip).x;
		float occ4 = textureLod(depthTex, vec2(minClip.x, maxClip.y), mip).x;

		float occDepth = max(occ4, max(occ3, max(occ1, occ2)));

		occDepth = Linearize(camParam.x, camParam.y, occDepth);	
		minClip.z = Linearize(camParam.x, camParam.y, minClip.z);

		return minClip.z > occDepth + 0.01;
}

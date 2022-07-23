struct ShadowParam {
	float pcount;
	float inv;
	float minLength;
	float size;
};

const ShadowParam sparams[3] = ShadowParam[3](
	ShadowParam(2.0, 1.0 / ((2.0 * 2.0 + 1.0) * (2.0 * 2.0 + 1.0)), 0.0, 1.0),
	ShadowParam(3.0, 1.0 / ((3.0 * 2.0 + 1.0) * (3.0 * 2.0 + 1.0)), 0.0, 1.0),
	ShadowParam(4.0, 1.0 / ((4.0 * 2.0 + 1.0) * (4.0 * 2.0 + 1.0)), 120.0, 10.0)
);

float genPCF(sampler2D shadowMap, vec3 shadowCoord, float bias, vec2 pixSize, ShadowParam sparam) {
	float shadowFactor = 0.0, biasDepth = shadowCoord.z + bias, i = 0.0, rand = 0.0;

	i += 1.0;
	rand = random(vec3(shadowCoord.xy + vec2(-sparam.pcount, -sparam.pcount), i), i);
	shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(-sparam.pcount, -sparam.pcount) * pixSize * rand).r);
	
	i += 1.0;
	rand = random(vec3(shadowCoord.xy + vec2(sparam.pcount, -sparam.pcount), i), i);
	shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(sparam.pcount, -sparam.pcount) * pixSize * rand).r);

	i += 1.0;
	rand = random(vec3(shadowCoord.xy + vec2(sparam.pcount, sparam.pcount), i), i);
	shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(sparam.pcount, sparam.pcount) * pixSize * rand).r);
	
	i += 1.0;
	rand = random(vec3(shadowCoord.xy + vec2(-sparam.pcount, sparam.pcount), i), i);
	shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(-sparam.pcount, sparam.pcount) * pixSize * rand).r);

	float preFactor = shadowFactor * 0.25;
	if(preFactor * (1.0 - preFactor) < 0.01) return preFactor;
	else {
		float pcount2 = sparam.pcount * sparam.pcount;
		for(float offx = -sparam.pcount; offx <= sparam.pcount; offx += 1.0) {
			for(float offy = -sparam.pcount; offy <= sparam.pcount; offy += 1.0) {
				float off2 = abs(offx * offy);
				if(off2 != pcount2) {
					i += 1.0;
					rand = random(vec3(shadowCoord.xy + vec2(offx, offy), i), i);
					shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(offx, offy) * pixSize * rand).r);
				}
				else continue;
			}
		}

		return shadowFactor * sparam.inv;
	}
}


float genShadow(sampler2D shadowMap, vec3 shadowCoord, float bias) {
	return step(shadowCoord.z + bias, texture(shadowMap, shadowCoord.xy).r);
}

float getShadowLen(sampler2D shadowMap, vec3 coord, float bias) {
	float stored = texture(shadowMap, coord.xy).r;
	float scene = coord.z + bias;
	return max(scene - stored, 0.0);
}

float genPCFShadowLen(sampler2D shadowMap, vec3 shadowCoord, float bias, vec2 pixSize, ShadowParam sparam) {
	float len = 0.0, i = 0.0, rand = 0.0;
	for(float offx = -sparam.pcount; offx <= sparam.pcount; offx += 1.0) {
		for(float offy = -sparam.pcount; offy <= sparam.pcount; offy += 1.0) {
			i += 1.0;
			rand = random(vec3(shadowCoord.xy + vec2(offx, offy), i * i), 1.0 / i);
			float l = getShadowLen(shadowMap, vec3(shadowCoord.xy + vec2(offx, offy) * pixSize * rand, shadowCoord.z), bias);
			len = l > len ? l : len;
		}
	}
	return len;
}

float genPCSS(sampler2D shadowMap, vec3 shadowCoord, float bias, vec2 pixSize, vec3 camPara) {
	float dLength = genPCFShadowLen(shadowMap, shadowCoord, bias, pixSize * 5.0, sparams[0]);
	dLength *= (camPara.z - camPara.x);
	ShadowParam param1 = sparams[1], param2 = sparams[2];
	float per = 1.0;
	if(dLength >= sparams[2].minLength) 
		return genPCF(shadowMap, shadowCoord, bias, pixSize * param2.size, param2);
	else if(dLength <= sparams[1].minLength) 
		return genPCF(shadowMap, shadowCoord, bias, pixSize * param1.size, param1);
	else {
		per = (dLength - param1.minLength) / (param2.minLength - param1.minLength);
			
		float sf1 = genPCF(shadowMap, shadowCoord, bias, pixSize * vec2(param1.size), param1);
		float sf2 = genPCF(shadowMap, shadowCoord, bias, pixSize * vec2(param2.size), param2);
		return mix(sf1, sf2, per);
	}
	return 1.0;
}

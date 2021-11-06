#include "shader/util.glsl"
#include "shader/pbr.glsl"

uniform BindlessSampler2D texBuffer, 
						matBuffer, 
						roughMetalBuffer, 
						depthBuffer;

uniform BindlessSamplerCube irradianceMap;
uniform BindlessSamplerCube prefilteredMap;
uniform BindlessSampler2D brdfMap;

uniform vec2 pixelSize;
uniform mat4 invViewProjMatrix, invProjMatrix;
uniform mat3 invViewMatrix;

uniform BindlessSampler2D shadowBuffers[4];

uniform mat4 lightViewProjDyn, lightViewProjNear, lightViewProjMid, lightViewProjFar;
uniform vec2 shadowPixSize;
uniform vec3 levels;
uniform vec3 light;
uniform float udotl;
uniform vec3 eyePos;
uniform float time;

uniform mat4 lightProjDyn, lightProjNear, lightProjMid, lightProjFar;
uniform mat4 lightViewDyn, lightViewNear, lightViewMid, lightViewFar;
uniform vec2 camParas[4];
uniform vec2 gaps;
uniform vec3 shadowCenter;

in vec2 vTexcoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;
layout (location = 2) out vec4 FragBright;

const vec3 KCool = vec3(0.15, 0.15, 0.35);
const vec3 KWarm = vec3(0.9, 0.9, 0.25);

float genPCF(sampler2D shadowMap, vec3 shadowCoord, float bias, float pcount, float inv) {
	float shadowFactor = 0.0, biasDepth = shadowCoord.z + bias, i = 0.0, rand = 0.0;

	i += 1.0;
	rand = random(vec3(shadowCoord.xy + vec2(-pcount, -pcount), i), i);
	shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(-pcount, -pcount) * shadowPixSize * rand).r);
	
	i += 1.0;
	rand = random(vec3(shadowCoord.xy + vec2(pcount, -pcount), i), i);
	shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(pcount, -pcount) * shadowPixSize * rand).r);

	i += 1.0;
	rand = random(vec3(shadowCoord.xy + vec2(pcount, pcount), i), i);
	shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(pcount, pcount) * shadowPixSize * rand).r);
	
	i += 1.0;
	rand = random(vec3(shadowCoord.xy + vec2(-pcount, pcount), i), i);
	shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(-pcount, pcount) * shadowPixSize * rand).r);

	float preFactor = shadowFactor * 0.25;
	if(preFactor * (1.0 - preFactor) < 0.01) return preFactor;
	else {
		float pcount2 = pcount * pcount;
		for(float offx = -pcount; offx <= pcount; offx += 1.0) {
			for(float offy = -pcount; offy <= pcount; offy += 1.0) {
				float off2 = abs(offx * offy);
				if(off2 != pcount2) {
					i += 1.0;
					rand = random(vec3(shadowCoord.xy + vec2(offx, offy), i), i);
					shadowFactor += step(biasDepth, texture(shadowMap, shadowCoord.xy + vec2(offx, offy) * shadowPixSize * rand).r);
				}
				else continue;
			}
		}

		return shadowFactor * inv;
	}
}


float genShadow(sampler2D shadowMap, vec3 shadowCoord, float bias) {
	return step(shadowCoord.z + bias, texture(shadowMap, shadowCoord.xy).r);
}

float getShadowLen(sampler2D shadowMap, vec3 shadowCoord, float bias) {
	float stored = texture(shadowMap, shadowCoord.xy).r;
	float scene = shadowCoord.z + bias;
	return max(scene - stored, 0.0);
}

vec4 genShadowFactor(vec4 worldPos, float depthView, float bias) {
	float lightDepth = depthView;
	if(lightDepth <= levels.x - gaps.x) {
		#ifndef USE_LINEAR_DEPTH
			float bs = bias;
		#else
			float bs = bias;
		#endif

		vec4 dyn = DepthToLinear(lightViewProjDyn, lightProjDyn, lightViewDyn, camParas[0].x, camParas[0].y, worldPos);
		vec3 lightPositionDyn = dyn.xyz / dyn.w;
		vec3 shadowCoordDyn = lightPositionDyn * 0.5 + 0.5;
		float sd = genPCF(shadowBuffers[0], shadowCoordDyn, bs, 3.0, 0.0205);

		vec4 near = DepthToLinear(lightViewProjNear, lightProjNear, lightViewNear, camParas[1].x, camParas[1].y, worldPos);
		vec3 lightPosition = near.xyz / near.w;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		float sf = genPCF(shadowBuffers[1], shadowCoord, bs, 3.0, 0.0205);
		
		sf = min(sd, sf);
		return vec4(1.0, 0.0, 0.0, sf);
	} else if(lightDepth > levels.x - gaps.x && lightDepth < levels.x + gaps.x) {
		#ifndef USE_LINEAR_DEPTH
			float bsNear = bias, bsMid = bias * 0.25;
		#else
			float bsNear = bias, bsMid = bias * 0.25;
		#endif

		vec4 dyn = DepthToLinear(lightViewProjDyn, lightProjDyn, lightViewDyn, camParas[0].x, camParas[0].y, worldPos);
		vec3 lightPositionDyn = dyn.xyz / dyn.w;
		vec3 shadowCoordDyn = lightPositionDyn * 0.5 + 0.5;

		vec4 near = DepthToLinear(lightViewProjNear, lightProjNear, lightViewNear, camParas[1].x, camParas[1].y, worldPos);
		vec3 lightPositionNear = near.xyz / near.w;
		vec3 shadowCoordNear = lightPositionNear * 0.5 + 0.5;

		vec4 mid = DepthToLinear(lightViewProjMid, lightProjMid, lightViewMid, camParas[2].x, camParas[2].y, worldPos);
		vec3 lightPositionMid = mid.xyz / mid.w;
		vec3 shadowCoordMid = lightPositionMid * 0.5 + 0.5;

		float factorMid = genPCF(shadowBuffers[2], shadowCoordMid, bsMid, 2.0, 0.04);
		float sf = 0.0;
		if(any(bvec2(any(lessThan(shadowCoordNear, vec3(0.001))), any(greaterThan(shadowCoordNear, vec3(0.999))))))
			sf = factorMid;
		else {
			float factorDyn = genPCF(shadowBuffers[0], shadowCoordDyn, bsNear, 3.0, 0.0205);
			float factorNear = genPCF(shadowBuffers[1], shadowCoordNear, bsNear, 3.0, 0.0205);
			factorNear = min(factorNear, factorDyn);
			sf = mix(factorNear, factorMid, (lightDepth - (levels.x - gaps.x)) * gaps.y);
		}
		return vec4(0.0, 0.0, 1.0, sf);
	} else if(depthView <= levels.y) {
		vec4 mid = DepthToLinear(lightViewProjMid, lightProjMid, lightViewMid, camParas[2].x, camParas[2].y, worldPos);
		vec3 lightPosition = mid.xyz / mid.w;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		#ifndef USE_LINEAR_DEPTH
			float bs = bias * 0.25;
		#else
			float bs = bias * 0.25;
		#endif
		float sf = genPCF(shadowBuffers[2], shadowCoord, bs, 2.0, 0.04);
		return vec4(0.0, 1.0, 0.0, sf);
	}
	#ifdef DRAW_FAR_SHADOW
		else {
			vec4 far = DepthToLinear(lightViewProjFar, lightProjFar, lightViewFar, camParas[3].x, camParas[3].y, worldPos);
			vec3 lightPosition = far.xyz / far.w;
			vec3 shadowCoord = lightPosition * 0.5 + 0.5;
			float bs = bias * 0.0;
			float sf = genShadow(shadowBuffers[3], shadowCoord, bs);
			return vec4(1.0, 1.0, 1.0, sf);
		}
	#endif
	return vec4(1.0);
}

void main() {
	float depth = texture(depthBuffer, vTexcoord).r;
	vec3 ndcPos = vec3(vTexcoord, depth) * 2.0 - 1.0;
	vec4 tex = texture(texBuffer, vTexcoord);
	vec3 albedo = tex.rgb;
	vec3 sceneColor = albedo;
	vec3 bright = vec3(0.0);
	vec3 normal = vec3(0.0, 0.0, 1.0);

	if(ndcPos.z < 1.0) {
		vec4 worldPos = invViewProjMatrix * vec4(ndcPos, 1.0);
		worldPos /= worldPos.w;

		vec3 v = eyePos - worldPos.xyz;
		float depthView = length(v);

		vec4 roughMetal = texture(roughMetalBuffer, vTexcoord);
		vec4 material = texture(matBuffer, vTexcoord);
		normal = vec3(roughMetal.ba, material.z) * 2.0 - vec3(1.0);

		float ndotl = dot(light, normal);

		float bias = max(0.0000025 * (1.0 - ndotl), 0.0005);
		if(material.a > (GrassFlag - 0.1) && material.a < (TerrainFlag + 0.1)) bias *= 0.5; // terrain(0.8) & grass(0.7) receiver shadow bias
		else bias *= -1.0; // normal receiver shadow bias
		ndotl = max(ndotl, 0.0);

		vec3 shadowLayer = vec3(1.0);
		#ifdef USE_SHADOW
			vec4 sf = genShadowFactor(worldPos, depthView, bias);
			float shadowFactor = ndotl < 0.01 ? 0.0 : tex.a * sf.a;
			//shadowLayer = sf.rgb; // Used for csm debug
		#else
			float shadowFactor = 1.0;
		#endif
		vec3 ambient = albedo;

		// PBR
		#ifndef USE_CARTOON
			ambient *= texture(irradianceMap, normal).rgb;
			v /= depthView;
			
			vec3 ref = reflect(-v, normal); 
			float ndotv = max(dot(normal, v), 0.0);
			float roughness = roughMetal.r, metallic = roughMetal.g;
			if(material.a > (GrassFlag - 0.01) && material.a < (GrassFlag + 0.01)) 
				roughness = 1.0 - roughness;

			vec3 F0 = mix(vec3(0.04), albedo, metallic);	
			vec3 fA = FresnelSchlickRoughness(ndotv, F0, roughness);
			vec3 dA = (vec3(1.0) - fA) * (1.0 - metallic);

			float roughLevel = roughness * MAX_REFLECTION_LOD;
    		vec3 prefilteredColor = textureLod(prefilteredMap, ref, roughLevel).rgb;   
    		vec2 brdf  = texture(brdfMap, vec2(ndotv, roughness)).rg;
    		vec3 ambSpec = prefilteredColor * (fA * brdf.x + brdf.y);
    		ambient = ambient * dA + ambSpec;

			if(shadowFactor * udotl < 0.0001) {
				sceneColor = ambient;
			} else {
				roughness = roughMetal.r, metallic = roughMetal.g;
				vec3 h = normalize(v + light);
				vec3 radiance = vec3(4.0);
			
				// Cook-Torrance BRDF	
				float NDF = DistributionGGX(normal, h, roughness);   
				float G   = GeometrySmith2(normal, v, light, roughness);      
				vec3 kS   = FresnelSchlick(max(dot(h, v), 0.0), F0);
				vec3 kD   = (vec3(1.0) - kS) * (1.0 - metallic);

				float specular = (NDF * G) / (4.0 * ndotv * ndotl + 0.001);
				vec3 Lo = (kD * albedo * INV_PI + kS * specular) * radiance * ndotl;
    			
				sceneColor = shadowLayer * (ambient + shadowFactor * Lo);
			}
			sceneColor *= udotl;
		// Cartoon
		#else
			float darkness = shadowFactor;
			float threshold = 0.15;
			float cwFactor = step(darkness, threshold);
			vec3 kd = KCool * cwFactor + KWarm * (1.0 - cwFactor);
			sceneColor = (ambient * material.r + kd * albedo * material.g) * udotl;
		#endif
	} else {
		normal = invViewMatrix * normal;
		#ifdef USE_BLOOM
			bright = sceneColor * udotl * 2.5; // Bloom
		#endif
	}

	FragColor = vec4(sceneColor, depth);
	FragNormal = vec4(normal, 0.0);
	FragBright = vec4(bright, 1.0);
}

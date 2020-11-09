#include "shader/util.glsl"

layout(bindless_sampler) uniform sampler2D texBuffer, matBuffer, normalGrassBuffer, roughMetalBuffer, depthBuffer;
uniform vec2 pixelSize;
uniform mat4 invViewProjMatrix, invProjMatrix;

layout(bindless_sampler) uniform sampler2D depthBufferNear, depthBufferMid, depthBufferFar;
uniform mat4 lightViewProjNear, lightViewProjMid, lightViewProjFar;
uniform vec2 shadowPixSize;
uniform vec2 levels;
uniform vec3 light;
uniform float udotl;
uniform vec3 eyePos;
uniform float time;

uniform mat4 lightProjNear, lightProjMid, lightProjFar;
uniform mat4 lightViewNear, lightViewMid, lightViewFar;
uniform vec2 camParas[3];

in vec2 vTexcoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragBright;

const float GAP = 30.0;
const float INV2GAP = 0.01667;
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

float genShadowFactor(vec4 worldPos, float depthView, float bias) {
	if(depthView <= levels.x - GAP) {
		vec4 near = DepthToLinear(lightViewProjNear, lightProjNear, lightViewNear, camParas[0].x, camParas[0].y, worldPos);
		vec3 lightPosition = near.xyz / near.w;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		#ifndef USE_LINEAR_DEPTH
			float bs = bias * 0.00045;
		#else
			float bs = bias * 0.00015;
		#endif
		return genPCF(depthBufferNear, shadowCoord, bs, 3.0, 0.0205);
	} else if(depthView > levels.x - GAP && depthView < levels.x + GAP) {
		vec4 near = DepthToLinear(lightViewProjNear, lightProjNear, lightViewNear, camParas[0].x, camParas[0].y, worldPos);
		vec3 lightPositionNear = near.xyz / near.w;
		vec3 shadowCoordNear = lightPositionNear * 0.5 + 0.5;

		vec4 mid = DepthToLinear(lightViewProjMid, lightProjMid, lightViewMid, camParas[1].x, camParas[1].y, worldPos);
		vec3 lightPositionMid = mid.xyz / mid.w;
		vec3 shadowCoordMid = lightPositionMid * 0.5 + 0.5;

		#ifndef USE_LINEAR_DEPTH
			float bsNear = bias * 0.00045, bsMid = 0.0;
		#else
			float bsNear = bias * 0.00015, bsMid = 0.0;
		#endif
		float factorNear = genPCF(depthBufferNear, shadowCoordNear, bsNear, 3.0, 0.0205);
		float factorMid = genShadow(depthBufferMid, shadowCoordMid, bsMid);
		return mix(factorNear, factorMid, (depthView - (levels.x - GAP)) * INV2GAP);
	} else if(depthView <= levels.y) {
		vec4 mid = DepthToLinear(lightViewProjMid, lightProjMid, lightViewMid, camParas[1].x, camParas[1].y, worldPos);
		vec3 lightPosition = mid.xyz / mid.w;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		float bs = 0.00;
		return genShadow(depthBufferMid, shadowCoord, bs);
	}
	#ifdef DRAW_FAR_SHADOW
		else {
			vec4 far = DepthToLinear(lightViewProjFar, lightProjFar, lightViewFar, camParas[2].x, camParas[2].y, worldPos);
			vec3 lightPosition = far.xyz / far.w;
			vec3 shadowCoord = lightPosition * 0.5 + 0.5;
			float bs = bias * 0.0;
			return genShadow(depthBufferFar, shadowCoord, bs);
		}
	#endif
	return 1.0;
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
	float k = r * r * 0.125;
	
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, float NdotL, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

void main() {
	float depth = texture(depthBuffer, vTexcoord).r;
	vec3 ndcPos = vec3(vTexcoord, depth) * 2.0 - 1.0;
	vec4 tex = texture(texBuffer, vTexcoord);
	vec3 albedo = tex.rgb;
	vec3 sceneColor = albedo;
	vec3 bright = vec3(0.0);

	if(ndcPos.z < 1.0) {
		vec4 worldPos = invViewProjMatrix * vec4(ndcPos, 1.0);
		worldPos /= worldPos.w;

		vec3 v = eyePos - worldPos.xyz;
		float depthView = length(v);

		vec3 normal = texture(normalGrassBuffer, vTexcoord).xyz * 2.0 - vec3(1.0);
		vec3 material = texture(matBuffer, vTexcoord).rgb;

		float ndotl = dot(light, normal);
		float bias = tan(acos(abs(ndotl)));
		ndotl = max(ndotl, 0.0);

		#ifdef USE_SHADOW
			float shadowFactor = ndotl < 0.01 ? 0.0 : tex.a * genShadowFactor(worldPos, depthView, bias);
		#else
			float shadowFactor = 1.0;
		#endif
		vec3 ambient = material.r * albedo;

		// PBR
		#ifndef USE_CARTOON
			if(shadowFactor < 0.01 || udotl < 0.01) 
				sceneColor = ambient * udotl;
			else {
				v /= depthView;
				vec3 h = normalize(v + light);
				vec3 radiance = vec3(3.5);
				vec2 roughMetal = texture(roughMetalBuffer, vTexcoord).rg;
			
				// Cook-Torrance BRDF	
				vec3 F0 = mix(vec3(0.04), albedo, roughMetal.g);	
				float NDF = DistributionGGX(normal, h, roughMetal.r);   
				float G   = GeometrySmith(normal, v, ndotl, roughMetal.r);      
				vec3 kS   = FresnelSchlick(max(dot(h, v), 0.0), F0);
				vec3 kD   = (vec3(1.0) - kS) * (1.0 - roughMetal.g);	 

				float specular = (NDF * G) / (4.0 * max(dot(normal, v), 0.0) * ndotl + 0.001);
				vec3 Lo = (kD * albedo * INV_PI + kS * specular) * radiance * ndotl;

				sceneColor = (ambient + shadowFactor * Lo) * udotl;
			}
		// Cartoon
		#else
			float darkness = ndotl * shadowFactor;
			float threshold = 0.45;
			float cwFactor = step(darkness, threshold);
			vec3 kd = KCool * cwFactor + KWarm * (1.0 - cwFactor);
			sceneColor = (ambient + kd * albedo * material.g) * udotl;
		#endif
	} else {
		#ifdef USE_BLOOM
			bright = sceneColor * udotl * 2.5; // Bloom
		#endif
	}

	FragColor = vec4(sceneColor, depth);
	FragBright = vec4(bright, 1.0);
}

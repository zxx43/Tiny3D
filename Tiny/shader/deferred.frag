#include "shader/util.glsl"

uniform BindlessSampler2D texBuffer, 
						matBuffer, 
						roughMetalBuffer, 
						depthBuffer;

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

		float bias = max(0.005 * (1.0 - ndotl), 0.0005);
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
		vec3 ambient = material.r * albedo;

		// PBR
		#ifndef USE_CARTOON
			if(shadowFactor * udotl < 0.0001) 
				sceneColor = ambient * udotl;
			else {
				v /= depthView;
				vec3 h = normalize(v + light);
				vec3 radiance = vec3(3.5);
			
				// Cook-Torrance BRDF	
				vec3 F0 = mix(vec3(0.04), albedo, roughMetal.g);	
				float NDF = DistributionGGX(normal, h, roughMetal.r);   
				float G   = GeometrySmith(normal, v, ndotl, roughMetal.r);      
				vec3 kS   = FresnelSchlick(max(dot(h, v), 0.0), F0);
				vec3 kD   = (vec3(1.0) - kS) * (1.0 - roughMetal.g);	 

				float specular = (NDF * G) / (4.0 * max(dot(normal, v), 0.0) * ndotl + 0.001);
				vec3 Lo = (kD * albedo * INV_PI + kS * specular) * radiance * ndotl;

				sceneColor = shadowLayer * (ambient + shadowFactor * Lo) * udotl;
			}
		// Cartoon
		#else
			float darkness = shadowFactor;
			float threshold = 0.15;
			float cwFactor = step(darkness, threshold);
			vec3 kd = KCool * cwFactor + KWarm * (1.0 - cwFactor);
			sceneColor = (ambient + kd * albedo * material.g) * udotl;
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

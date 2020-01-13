layout(bindless_sampler) uniform sampler2D texBuffer, matBuffer, normalGrassBuffer, roughMetalBuffer, depthBuffer;
uniform vec2 pixelSize;

uniform mat4 invViewProjMatrix, invProjMatrix;

layout(bindless_sampler) uniform sampler2D depthBufferNear, depthBufferMid, depthBufferFar;
uniform mat4 lightViewProjNear, lightViewProjMid, lightViewProjFar;
uniform vec2 shadowPixSize;
uniform int useShadow;
uniform vec2 levels;
uniform vec3 light;
uniform vec3 eyePos;
uniform float time;
uniform float useCartoon;

in vec2 vTexcoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragBright;

#define GAP float(30.0)
#define INV2GAP float(0.01667)

float genPCF(sampler2D shadowMap, vec3 shadowCoord, float bias, float pcount, float inv) {
	float shadowFactor = 0.0;
	for(float offx = -pcount; offx <= pcount; offx += 1.0) {
		for(float offy = -pcount; offy <= pcount; offy += 1.0) 
			shadowFactor += texture2D(shadowMap, shadowCoord.xy + vec2(offx, offy) * shadowPixSize).r > (shadowCoord.z - bias) ? 1.0 : 0.0;
	}
	return shadowFactor * inv;
}


float genShadow(sampler2D shadowMap, vec3 shadowCoord, float bias) {
	return texture2D(shadowMap, shadowCoord.xy).r > (shadowCoord.z - bias) ? 1.0 : 0.0;
}

float genShadowFactor(vec4 worldPos, float depthView, float bias) {
	if(depthView <= levels.x - GAP) {
		vec4 near = lightViewProjNear * worldPos;
		vec3 lightPosition = near.xyz / near.w;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		float bs = bias * 0.00001;
		return genPCF(depthBufferNear, shadowCoord, bs, 2.0, 0.04);
	} else if(depthView > levels.x - GAP && depthView < levels.x + GAP) {
		vec4 near = lightViewProjNear * worldPos;
		vec3 lightPositionNear = near.xyz / near.w;
		vec3 shadowCoordNear = lightPositionNear * 0.5 + 0.5;
		float bsNear = bias * 0.00001;

		vec4 mid = lightViewProjMid * worldPos;
		vec3 lightPositionMid = mid.xyz / mid.w;
		vec3 shadowCoordMid = lightPositionMid * 0.5 + 0.5;
		float bsMid = bias * 0.00001;

		float factorNear = genPCF(depthBufferNear, shadowCoordNear, bsNear, 2.0, 0.04);
		float factorMid = genPCF(depthBufferMid, shadowCoordMid, bsMid, 1.0, 0.111);
		return mix(factorNear, factorMid, (depthView - (levels.x - GAP)) * INV2GAP);
	} else if(depthView <= levels.y) {
		vec4 mid = lightViewProjMid * worldPos;
		vec3 lightPosition = mid.xyz / mid.w;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		float bs = bias * 0.00001;
		return genPCF(depthBufferMid, shadowCoord, bs, 1.0, 0.111);
	} else {
		vec4 far = lightViewProjFar * worldPos;
		vec3 lightPosition = far.xyz / far.w;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		float bs = bias * 0.00001;
		return genShadow(depthBufferFar, shadowCoord, bs);
	}
	return 1.0;
}

vec3 Smudge(vec3 sceneTex, float grassFlag, float viewDist) {
	if(grassFlag < 1.0) 
		return sceneTex;
	else {
		float xx = vTexcoord.x;
		float yy = 1.0 - vTexcoord.y;

		float len = viewDist;
		float d = BlendVal(len, 0.0, 500.0, 100.0, 500.0);
		float dclose = BlendVal(len, 0.0, 20.0, 30.0, 1.0);
		d *= dclose;
		yy += dot(vec3(xx), vec3(1009.0, 1259.0, 2713.0));
		yy += time * 0.00005;
		yy += sceneTex.g * 0.4;
		
		float yoffset = 1.0 - fract(yy * d) / d;
		vec2 uvoffset = vTexcoord - vec2(0.0, yoffset);
		vec4 grassColor = texture2D(texBuffer, uvoffset);

		float depthGrass = texture2D(depthBuffer, uvoffset).r;
		vec3 ndcGrass = vec3(uvoffset, depthGrass) * 2.0 - 1.0;
		vec4 viewGrass = invProjMatrix * vec4(ndcGrass, 1.0);
		viewGrass /= viewGrass.w;
		
		if(viewGrass.z >= -viewDist)
			return sceneTex;
		else 
			return mix(sceneTex, grassColor.rgb, saturate(yoffset * d / 3.8));
	}
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
	float depth = texture2D(depthBuffer, vTexcoord).r;
	vec3 ndcPos = vec3(vTexcoord, depth) * 2.0 - 1.0;
	vec4 tex = texture2D(texBuffer, vTexcoord);
	vec3 albedo = tex.rgb;
	vec3 sceneColor = albedo;
	vec3 bright = vec3(0.0);

	if(ndcPos.z < 1.0) {
		vec4 worldPos = invViewProjMatrix * vec4(ndcPos, 1.0);
		worldPos /= worldPos.w;

		vec3 radiance = vec3(3.5);
		vec2 roughMetal = texture2D(roughMetalBuffer, vTexcoord).rg;

		vec3 v = eyePos - worldPos.xyz;
		float depthView = length(v);
		v = v / depthView;
		vec3 h = normalize(v + light);

		vec4 normalGrass = texture2D(normalGrassBuffer, vTexcoord);
		vec3 normal = normalGrass.xyz * 2.0 - vec3(1.0);
		vec3 material = texture2D(matBuffer, vTexcoord).rgb;

		float ndotl = dot(light, normal);
		float bias = tan(acos(abs(ndotl)));
		ndotl = max(ndotl, 0.0);

		float shadowFactor = (useShadow != 0) ? tex.a * genShadowFactor(worldPos, depthView, bias) : 1.0;
		vec3 ambient = material.r * albedo;

		if(useCartoon < 0.5) { // PBR
			// Cook-Torrance BRDF	
			vec3 F0 = mix(vec3(0.04), albedo, roughMetal.g);	
			float NDF = DistributionGGX(normal, h, roughMetal.r);   
			float G   = GeometrySmith(normal, v, ndotl, roughMetal.r);      
			vec3 kS   = FresnelSchlick(max(dot(h, v), 0.0), F0);
			vec3 kD   = (vec3(1.0) - kS) * (1.0 - roughMetal.g);	 

			float specular = (NDF * G) / (4.0 * max(dot(normal, v), 0.0) * ndotl + 0.001);
			vec3 Lo = (kD * albedo * INV_PI + kS * specular) * radiance * ndotl;

			sceneColor = ambient + shadowFactor * Lo;
		} else { // Cartoon
			float darkness = ndotl * shadowFactor;
			vec3 kCool = vec3(0.01, 0.01, 0.1), kWarm = vec3(0.9, 0.9, 0.25);
			float threshold = 0.1;
			vec3 kd = darkness < threshold ? kCool : kWarm;

			sceneColor = ambient + kd * albedo * material.g;
		}
	} else {
		bright = sceneColor * 1.5;
	}

	FragColor = vec4(sceneColor, depth);
	FragBright = vec4(bright, 1.0);
}

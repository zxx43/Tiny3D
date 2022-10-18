#include "shader/util.glsl"

layout(early_fragment_tests) in;

uniform BindlessSamplerCube texEnv, texSky;
uniform BindlessSampler2D texRef;
uniform BindlessSampler2D texScene, sceneDepth;

uniform mat4 viewMatrix, invViewProjMatrix;
uniform vec2 waterBias;
uniform vec3 light;
uniform float udotl;
uniform float wHeight;

in vec3 vNormal;
in vec3 vEye2Water;
in vec3 vWaterPos;
in vec4 vProjPos;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragNormal;

void main() {
	vec3 normal = normalize(vNormal);
	vec3 eye2Water = normalize(vEye2Water);
	
	vec3 refCoord = (vProjPos.xyz / vProjPos.w) * 0.5 + 0.5;
	float depth = refCoord.z;
	#ifdef DISABLE_SSR
		vec2 bias = normalize(mat3(viewMatrix) * normal).xz * waterBias;
		vec4 reflectTex = texture(texRef, refCoord.xy + bias);
	#else
		vec4 reflectTex = texture(texRef, refCoord.xy);
	#endif

	vec3 reflectCoord = reflect(eye2Water, normal);
	reflectCoord = vec3(reflectCoord.x, -reflectCoord.yz);
	#ifndef DYN_SKY
		vec3 reflectMapTex = udotl * texture(texEnv, reflectCoord).rgb;
	#else
		vec3 reflectMapTex = udotl * pow(texture(texSky, reflectCoord).rgb, INV_GAMMA);
	#endif

	vec3 refractCoord = refract(eye2Water, normal, 0.750395);
	refractCoord = vec3(refractCoord.x, -refractCoord.yz);
	#ifndef DYN_SKY
		vec3 refractMapTex = udotl * texture(texEnv, refractCoord).rgb;
	#else
		vec3 refractMapTex = udotl * pow(texture(texSky, reflectCoord).rgb, INV_GAMMA);
	#endif

	vec3 reflectedColor = reflectTex.rgb * reflectMapTex;

	vec4 sceneColor = texture(texScene, refCoord.xy);
	#ifndef HIGH_QUALITY
		float sDepth = texture(sceneDepth, refCoord.xy).r;
	#else
		float sDepth = sceneColor.w;
	#endif
	vec3 ndcScene = vec3(refCoord.xy, sDepth) * 2.0 - 1.0;
	vec4 scenePos = invViewProjMatrix * vec4(ndcScene, 1.0); 
	scenePos /= scenePos.w;
	float depthFactor = clamp((wHeight - scenePos.y - 10.0) * 0.01, 0.0, 1.0);
	vec3 refractedColor = mix(sceneColor.rgb, refractMapTex, depthFactor).rgb;

	float ndote = -dot(normal, eye2Water);
	float fresnel = mix(0.25, 1.0, pow(1.0 - ndote, 3.0));
	vec3 waterColor = mix(refractedColor, reflectedColor, fresnel);
	
	#ifdef USE_CARTOON
		FragTex = vec4(waterColor, depth);
	#else 
		float depthView = length(vEye2Water);
		FragTex = vec4(GenFogColor(-0.00000075, vec4(vWaterPos, 1.0), depthView, udotl, waterColor), depth);
	#endif
	FragNormal = vec4(normal * 0.5 + vec3(0.5), WaterFlag);
}
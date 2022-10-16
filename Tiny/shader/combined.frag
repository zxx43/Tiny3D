#include "shader/util.glsl"

uniform BindlessSampler2D sceneBuffer, 
						sceneNormalBuffer,
						sceneDepthBuffer, 
						waterBuffer, 
						waterMatBuffer, 
						waterNormalBuffer, 
						waterDepthBuffer, 
						bloomBuffer;

uniform vec2 pixelSize;
uniform mat4 invViewProjMatrix;
uniform vec3 light;
uniform float udotl;
uniform vec3 eyePos;
uniform float wHeight;

in vec2 vTexcoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NormalWaterFlag;

void main() {
 	vec4 waterRefColor = texture(waterBuffer, vTexcoord);	
	vec4 sceneColor = texture(sceneBuffer, vTexcoord);
	#ifdef USE_BLOOM
		sceneColor.rgb += texture(bloomBuffer, vTexcoord).rgb;
	#endif

	#ifndef HIGH_QUALITY
		float sDepth = texture(sceneDepthBuffer, vTexcoord).r;
		float wDepth = texture(waterDepthBuffer, vTexcoord).r;
	#else
		float sDepth = sceneColor.w;
		float wDepth = waterRefColor.w;
	#endif

	vec3 ndcScene = vec3(vTexcoord, sDepth) * 2.0 - 1.0;
	vec4 scenePos = invViewProjMatrix * vec4(ndcScene, 1.0); 
	scenePos /= scenePos.w;

	if(sDepth <= wDepth) { // Scene fragment
		vec3 finalColor = sceneColor.rgb;
		float depthView = length(scenePos.xyz - eyePos);

		#ifdef USE_CARTOON
			FragColor = vec4(finalColor, sDepth);
		#else
			FragColor = vec4(GenFogColor(-0.00000075, scenePos, depthView, udotl, finalColor), sDepth);
		#endif

		NormalWaterFlag = vec4(texture(sceneNormalBuffer, vTexcoord).xyz, 0.0);
	} else { // Water fragment
		vec3 ndcWater = vec3(vTexcoord, wDepth) * 2.0 - 1.0;
		vec4 waterPos = invViewProjMatrix * vec4(ndcWater, 1.0);
		waterPos /= waterPos.w;

		float depthView = length(waterPos.xyz - eyePos);

		vec4 waterMatColor = texture(waterMatBuffer, vTexcoord);
		float depthFactor = clamp((wHeight - scenePos.y - 10.0) * 0.01, 0.0, 1.0);

		vec4 waterNormalPix = texture(waterNormalBuffer, vTexcoord);
		float fresnel = waterNormalPix.w;

		vec3 sceneRefract = sceneColor.rgb;
		vec3 waterReflect = waterRefColor.rgb;
		vec3 waterRefract = waterMatColor.rgb;

		waterRefract = mix(sceneRefract, waterRefract, depthFactor).rgb;

		vec3 finalColor = mix(waterRefract, waterReflect, fresnel);

		#ifdef USE_CARTOON
			FragColor = vec4(finalColor, wDepth);
		#else
			FragColor = vec4(GenFogColor(-0.00000075, waterPos, depthView, udotl, finalColor), wDepth);
		#endif

		NormalWaterFlag = vec4(waterNormalPix.xyz, 1.0);
	}

	#ifdef HIGH_QUALITY
		FragColor.rgb = vec3(1.0) - exp(-FragColor.rgb * 2.5);
	#endif
	FragColor.rgb = pow(FragColor.rgb, INV_GAMMA);
}
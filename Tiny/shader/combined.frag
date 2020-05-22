#include "shader/util.glsl"

layout(bindless_sampler) uniform sampler2D sceneBuffer, sceneDepthBuffer, waterBuffer, waterDepthBuffer, matBuffer, waterNormalBuffer, bloomBuffer;
uniform vec2 pixelSize;
uniform mat4 invViewProjMatrix;
uniform vec3 light;
uniform float udotl;
uniform vec3 eyePos;

in vec2 vTexcoord;

out vec4 FragColor;

#define START_H float(0.0)
#define END_H float(1000.0)
#define FOG_COLOR vec3(0.9)

vec3 GenFogColor(vec4 worldPos, float depthView, vec3 sceneColor) {
	float worldH = worldPos.y / worldPos.w;
	float heightFactor = smoothstep(START_H, END_H, worldH);
	float fogFactor = exp2(-0.00000075 * depthView * depthView * LOG2);

	fogFactor = mix(fogFactor, 1.0, heightFactor);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	return mix(FOG_COLOR * udotl, sceneColor, fogFactor);
}

void main() {
 	vec4 waterRefColor = texture(waterBuffer, vTexcoord);	
	vec4 sceneColor = texture(sceneBuffer, vTexcoord);
	#ifdef USE_BLOOM
		vec4 bloomColor = texture(bloomBuffer, vTexcoord);
		sceneColor.rgb += bloomColor.rgb;
	#endif

	float sDepth = texture(sceneDepthBuffer, vTexcoord).r;
	float wDepth = texture(waterDepthBuffer, vTexcoord).r;

	vec3 ndcScene = vec3(vTexcoord, sDepth) * 2.0 - 1.0;
	vec3 ndcWater = vec3(vTexcoord, wDepth) * 2.0 - 1.0;
	
	vec4 scenePos = invViewProjMatrix * vec4(ndcScene, 1.0); 
	scenePos /= scenePos.w; 
	vec4 waterPos = invViewProjMatrix * vec4(ndcWater, 1.0);
	waterPos /= waterPos.w;

	vec3 eye2Water = waterPos.xyz - eyePos;
	float depthView = length(eye2Water);

	vec4 waterMatColor = texture(matBuffer, vTexcoord);
	float waterFactor = 1.0 - step(0.2, waterMatColor.w);
	float depthFactor = clamp((waterPos.y - scenePos.y - 10.0) * 0.01, 0.0, 1.0) * waterFactor;

	vec3 waterNormal = texture(waterNormalBuffer, vTexcoord).rgb * 2.0 - 1.0;
	float ndote = -dot(waterNormal, normalize(eye2Water));

	vec3 sceneRefract = sceneColor.rgb;
	vec3 waterReflect = waterRefColor.rgb;
	vec3 waterRefract = waterMatColor.rgb;

	waterRefract = mix(sceneRefract, waterRefract, depthFactor).rgb;

	float fresnel = mix(0.25, 1.0, pow(1.0 - ndote, 3.0));
	vec3 waterColor = mix(waterRefract, waterReflect, fresnel);
	vec3 finalColor = mix(sceneColor.rgb, waterColor, waterFactor);

	#ifdef USE_CARTOON
	FragColor = vec4(finalColor, waterFactor);
	#else
	FragColor = vec4(GenFogColor(waterPos, depthView, finalColor), waterFactor);
	#endif

	#ifdef HIGH_QUALITY
	FragColor.rgb = vec3(1.0) - exp(-FragColor.rgb * 2.5);
	#endif
	FragColor.rgb = pow(FragColor.rgb, INV_GAMMA);
}
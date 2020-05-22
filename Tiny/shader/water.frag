#include "shader/util.glsl"

layout(early_fragment_tests) in;

layout(bindless_sampler) uniform samplerCube texEnv, texSky;
layout(bindless_sampler) uniform sampler2D texRef;
uniform mat4 viewMatrix;
uniform vec2 waterBias;
uniform vec3 light;
uniform float udotl;

in vec3 vNormal;
in vec3 vEye2Water;
in vec4 vProjPos;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragMat;
layout (location = 2) out vec4 FragNormal;

void main() {
	vec3 normal = normalize(vNormal);
	vec3 eye2Water = normalize(vEye2Water);
	
	vec2 refCoord = (vProjPos.xy / vProjPos.w) * 0.5 + 0.5;
	#ifdef DISABLE_SSR
	vec2 bias = normalize(mat3(viewMatrix) * normal).xz * waterBias;
	vec4 reflectTex = texture(texRef, refCoord + bias);
	#else
	vec4 reflectTex = texture(texRef, refCoord);
	#endif

	vec3 reflectCoord = reflect(eye2Water, normal);
	reflectCoord = vec3(reflectCoord.x, -reflectCoord.yz);
	vec3 reflectMapTex = udotl * texture(texEnv, reflectCoord).rgb;

	vec3 refractCoord = refract(eye2Water, normal, 0.750395);
	refractCoord = vec3(refractCoord.x, -refractCoord.yz);
	vec3 refractMapTex = udotl * texture(texEnv, refractCoord).rgb;

	vec3 reflectedColor = reflectTex.rgb * reflectMapTex;
	vec3 refractedColor = refractMapTex;

	vec4 surfaceColor = vec4(reflectedColor, 0.5);
	vec4 matColor = vec4(refractedColor, 0.0);
		
	FragTex = surfaceColor;
	FragMat = matColor;
	FragNormal = vec4(normal * 0.5 + 0.5, 1.0);
}
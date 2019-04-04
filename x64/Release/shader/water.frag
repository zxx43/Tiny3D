#version 330
layout(early_fragment_tests) in;

uniform samplerCube textureEnv;
uniform sampler2D reflectBuffer;
uniform vec2 waterBias;

in vec3 vNormal;
in vec3 vViewNormal;
in vec3 vEye2Water;
in vec4 vProjPos;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;

void main() {
	vec3 normal = normalize(vNormal);
	vec3 eye2Water = normalize(vEye2Water);
	
	vec2 bias = normalize(vViewNormal).xz * waterBias;
	vec2 refCoord = (vProjPos.xy/vProjPos.w) * 0.5 + 0.5;
	vec4 reflectTex = texture2D(reflectBuffer, refCoord + bias);

	vec3 reflectMapTex = texture(textureEnv, reflect(eye2Water, normal)).rgb;
	vec3 refractMapTex = texture(textureEnv, refract(eye2Water, normal, 0.750395)).rgb;

	vec3 reflectedColor = reflectTex.rgb * reflectMapTex;
	vec3 refractedColor = refractMapTex;

	vec4 surfaceColor = vec4(reflectedColor, 0.5);
	vec4 matColor = vec4(refractedColor, 0.0);
		
	FragTex = surfaceColor;
	FragColor = matColor;
	FragNormal = vec4(normal * 0.5 + 0.5, 1.0);
}
#version 330

uniform samplerCube textureEnv;
uniform vec3 light;

in vec3 vNormal;
in vec3 vEye2Water;
in vec3 vWater;

layout (location = 0) out vec4 FragTex;
layout (location = 1) out vec4 FragColor;
layout (location = 2) out vec4 FragNormal;

void main() {
	vec3 normal = normalize(vNormal);
	vec3 eye2Water = normalize(vEye2Water);
	vec3 reflectedColor = texture(textureEnv, reflect(eye2Water, normal)).rgb;
	vec3 refractedColor = texture(textureEnv, refract(eye2Water, normal, 0.750395)).rgb;
	vec3 lightDirectionReflected = reflect(normalize(vWater - light), normal);
	float specular = pow(max(-dot(eye2Water, lightDirectionReflected), 0.0), 40.0);
	
	float ndote = -dot(normal, eye2Water);
	float fresnel = clamp(1.0 - ndote, 0.0, 1.0);
    fresnel = pow(fresnel, 3.0) * 0.65;

	vec3 waterColor = mix(reflectedColor, refractedColor, fresnel) + vec3(specular);

	vec4 surfaceColor = vec4(waterColor, 0.5);
	vec3 color = vec3(1.0, 0.0, 0.0);
		
	FragTex = surfaceColor;
	FragColor = vec4(color, 1.0);
	FragNormal = vec4(normal * 0.5 + 0.5, 1.0);
}
#include "shader/util.glsl"

uniform BindlessSamplerCube environmentMap;

in vec3 vViewDir;

layout (location = 0) out vec4 FragColor;

void main() {
	// the sample direction equals the hemisphere's orientation 
    vec3 normal = normalize(vViewDir);
	vec3 right  = cross(UP_VEC3, normal);
	vec3 up     = cross(normal, right);
	
    vec3 irradiance   = vec3(0.0);
    float sampleDelta = 0.025;
	float nrSamples   = 0.0; 
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
    	for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
        	// spherical to cartesian (in tangent space)
        	vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
        	// tangent space to world
        	vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;  

        	irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
        	nrSamples += 1.0;
    	}
	}
	irradiance = PI * irradiance / nrSamples;
	FragColor.rgb = irradiance;
	FragColor.rgb = pow(FragColor.rgb, INV_GAMMA);
}

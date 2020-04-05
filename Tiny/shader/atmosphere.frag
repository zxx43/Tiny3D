#include "shader/atmosphere.glsl"

uniform vec3 light;

in vec3 vViewDir;

layout (location = 0) out vec4 FragColor;

const vec3 RlhCof = vec3(5.5e-6, 13.0e-6, 22.4e-6);

void main() {
	vec3 color = atmosphere(
        normalize(vViewDir),            // normalized ray direction
        vec3(0.0,6378e3,0.0),               // ray origin
        normalize(light),                        // position of the sun
        22.0,                           // intensity of the sun
        6371.0e3,                         // radius of the planet in meters
        6471.0e3,                         // radius of the atmosphere in meters
        RlhCof,                         // Rayleigh scattering coefficient
        21.0e-6,                          // Mie scattering coefficient
        8.0e3,                            // Rayleigh scale height
        1.2e3,                          // Mie scale height
        0.758                           // Mie preferred scattering direction
    );
	// Apply exposure.
	color = 1.0 - exp(-1.0 * color);
	FragColor = vec4(color, 1.0);
}
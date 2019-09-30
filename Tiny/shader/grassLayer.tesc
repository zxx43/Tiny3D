#version 450

layout(vertices = 3) out;

uniform mat4 viewMatrix;

in vec3 vPosition[];

out vec3 tcPosition[];

#define MAX_TESS 10.0
#define MIN_TESS 1.0
#define NO_TESS 1.0

void main() {
	tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];

	vec4 viewPos = viewMatrix * vec4(tcPosition[0], 1.0);
	float z = viewPos.z;
	float level = MAX_TESS;
	if(z > 10.0) 
		level = NO_TESS;
	else 
		level = (MAX_TESS - MIN_TESS) * (600.0 + z) * 0.008 + MIN_TESS + 0.1;

	gl_TessLevelInner[0] = level;
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
}
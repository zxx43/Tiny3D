layout(vertices = 3) out;

in vec4 vPosition[];
in vec3 vNormal[];
in float vBack[];

out vec3 tcPosition[];
out vec3 tcNormal[];

#define MAX_TESS 10.0
#define MIN_TESS 1.0
#define NO_TESS 1.0

void main() {
	vec4 vpos = vPosition[gl_InvocationID];
	tcPosition[gl_InvocationID] = vpos.xyz;
	tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];

	float level = NO_TESS;
	if(vpos.w <= 30.0 && vBack[gl_InvocationID] > -0.0001) 
		level = (MAX_TESS - MIN_TESS) * (450.0 + vpos.w) * 0.01 + MIN_TESS + 0.1;

	gl_TessLevelInner[0] = level;
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
}
layout(early_fragment_tests) in;

in vec3 vViewDir;

layout (location = 0) out vec4 FragColor;

// random/hash function              
float hash( float n ) {
    return fract(sin(n)*43758.5453);
}

const vec3 NoiseVec = vec3(1.0, 57.0, 113.0);
const vec3 Start = vec3(0.0,6378e3,0.0);
float noise( vec3 dir ) {
	vec3 x = dir * 1000.0;
	x += Start;

	vec3 p  = floor(x);
	vec3 f  = smoothstep(0.0, 1.0, fract(x));
	float n = dot(p, NoiseVec);

	return  mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
			mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
			mix(mix( hash(n+113.0), hash(n+114.0),f.x),
			mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

void main() {
	float res = noise(normalize(vViewDir)) * 5.0;
	FragColor = vec4(res, res, res, 1.0);
}
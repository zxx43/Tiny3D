const int cSteps = 16;
const float stepLen = 300.0;
const float cloudThre = 0.0007;

// random/hash function              
float hash( float n ) {
    return fract(sin(n)*43758.5453);
}

const vec3 NoiseVec = vec3(1.0, 57.0, 113.0);
float noise( sampler2D tex, vec3 x ) {
	vec3 p  = floor(x);
	vec3 f  = smoothstep(0.0, 1.0, fract(x));
	float n = dot(p, NoiseVec);

	return  mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
			mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
			mix(mix( hash(n+113.0), hash(n+114.0),f.x),
			mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

const mat3 FbmMat = mat3( 0.00,  1.60,  1.20, -1.60,  0.72, -0.96, -1.20, -0.96,  1.28 );
float fbm( sampler2D tex, vec3 p, float tf ) {
	vec3 pm = p;
	float tm = tf * 0.00008;

	float f = 0.5000 * noise( tex, pm + tm ); 
	pm = FbmMat * pm * 2.02;
	
	f += 0.2500 * noise( tex, pm + tm ); 
	pm = FbmMat * pm * 2.03;
	
	f += 0.1250 * noise( tex, pm + tm ); 
	pm = FbmMat * pm * 2.01;

	f += 0.0625 * noise( tex, pm + tm );
	return f;
}

float cloudFilter(float x) {
    return 1.0 / exp(abs(x));
}

float haveCloud(sampler2D tex, vec3 p, float tf) {
    float r = fbm(tex, vec3(p.x + tf * 100.0, p.y, p.z) * 0.00001, tf);
    r *= cloudFilter((p.y - 24000.0) * 0.000001);
    r *= cloudFilter(length(p.xz) * 0.0000006);
    return r;
}

vec3 cloudRayMarch(sampler2D tex, vec3 start, vec3 sun, vec3 dir, float lightness, vec3 astrAtomScat, float tf) {
	vec3 res = astrAtomScat;
    if(dir.y <= 0.0) return res;
	else {
		float deltaHor = 25000.0 - dir.y;
		float invy = 1.0 / dir.y;
		float stepDeltaHor = deltaHor * invy;
		vec3 nvec = start + stepDeltaHor * dir;
		vec3 deltaStep = dir * (stepLen * invy);
	
		for(int i = 0; i < cSteps; ++i) {
			float den = haveCloud(tex, nvec, tf);
			if(den > cloudThre) {
				float beers = exp(-2500.0 * den);
				float alpha = (den * 200.0) * lightness;
				res = mix(res, vec3(beers), alpha);
			}
			nvec += deltaStep;
		}
		return res;
	}
}

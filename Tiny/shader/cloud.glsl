const int cSteps = 8;
const float stepLen = 500.0;
const float cloudThre = 0.0009;

float mod289(float x) {return x - floor(x * 0.00346) * 289.0;}
vec4 mod289(vec4 x) {return x - floor(x * 0.00346) * 289.0;}
vec4 perm(vec4 x) {return mod289(((x * 34.0) + 1.0) * x);}

float noise(sampler2D tex, vec3 p) {
	vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * 0.02439);
    vec4 o2 = fract(k4 * 0.02439);

	vec4 o3 = mix(o1, o2, d.z);
	vec2 o4 = mix(o3.xz, o3.yw, d.x);
	return mix(o4.x, o4.y, d.y);
}

float fbm(sampler2D tex, vec3 x, float tf) {
    float tm = tf*0.00008;
    float v = noise(tex, x*2.0+tm)*0.5;
    v += noise(tex, x*4.0+tm)*0.25;
    v += noise(tex, x*8.0+tm)*0.125;
    v += noise(tex, x*16.0+tm)*0.0625;
    v += noise(tex, x*32.0+tm)*0.03125;
    v += noise(tex, x*64.0+tm)*0.015625;
    v += noise(tex, x*128.0+tm)*0.0078125;
    return v;
}

float cloudFilter(float x) {
    return 1.0 / exp(abs(x));
}

float haveCloud(sampler2D tex, vec3 p, float tf){
    float r = fbm(tex, vec3(p.x + tf * 100.0, p.y, p.z) * 0.00001, tf);
    r *= cloudFilter((p.y - 24000.0) * 0.000001);
    r *= cloudFilter(length(p.xz) * 0.0000006);
    return r;
}

vec3 cloudRayMarch(sampler2D tex, vec3 start, vec3 sun, vec3 dir, float lightness, vec3 astrAtomScat, float tf) {
	vec3 res = astrAtomScat;
    if(dir.y <= 0.0) return res;

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

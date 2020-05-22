#include "shader/util.glsl"

layout(bindless_sampler) uniform sampler2D colorBuffer;
layout(bindless_sampler) uniform sampler2D normalBuffer;
layout(bindless_sampler) uniform sampler2D depthBuffer;
uniform vec2 pixelSize;

in vec2 vTexcoord;

out vec4 FragColor;

const vec4 OUTLINE_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

void main() {
	float separation = 1.0;
	vec2 off = pixelSize * separation;

	vec2 ld = vTexcoord + vec2(-off.x, -off.y);
	vec2 dd = vTexcoord + vec2(0.0,    -off.y);
	vec2 rd = vTexcoord + vec2(off.x,  -off.y);
	vec2 ll = vTexcoord + vec2(-off.x,    0.0);
	vec2 rr = vTexcoord + vec2(off.x,     0.0);
	vec2 lt = vTexcoord + vec2(-off.x,  off.y);
	vec2 tt = vTexcoord + vec2(0.0,     off.y);
	vec2 rt = vTexcoord + vec2(off.x,   off.y);
	vec2 ct = vTexcoord;
	
	float d0 = texture(depthBuffer, ld).r;
	float d1 = texture(depthBuffer, dd).r;
	float d2 = texture(depthBuffer, rd).r;
	float d3 = texture(depthBuffer, ll).r;
	float d4 = texture(depthBuffer, rr).r;
	float d5 = texture(depthBuffer, lt).r;
	float d6 = texture(depthBuffer, tt).r;
	float d7 = texture(depthBuffer, rt).r;
	float depth = texture(depthBuffer, ct).r;
	depth = 1.0 / depth;
	
	vec4 depth1 = vec4(1.0) / vec4(d0, d1, d2, d3);
	vec4 depth2 = vec4(1.0) / vec4(d4, d5, d6, d7);
	
	vec4 dDepth1 = abs(depth1 - depth);
	vec4 dDepth2 = abs(depth2 - depth);
	
	vec4 minDDepth = max(min(dDepth1, dDepth2), 0.00001);
	vec4 maxDDepth = max(dDepth1, dDepth2);
	vec4 depthResults = step(minDDepth * 110.0, maxDDepth);

	vec4 ldn = texture(normalBuffer, ld) * 2.0 - 1.0;
	vec4 ddn = texture(normalBuffer, dd) * 2.0 - 1.0;
	vec4 rdn = texture(normalBuffer, rd) * 2.0 - 1.0;
	vec4 lln = texture(normalBuffer, ll) * 2.0 - 1.0;
	vec4 rrn = texture(normalBuffer, rr) * 2.0 - 1.0;
	vec4 ltn = texture(normalBuffer, lt) * 2.0 - 1.0;
	vec4 ttn = texture(normalBuffer, tt) * 2.0 - 1.0;
	vec4 rtn = texture(normalBuffer, rt) * 2.0 - 1.0;
	vec4 ctn = texture(normalBuffer, ct) * 2.0 - 1.0;

	vec3 normal = ctn.xyz;
	vec4 dNormal1 = vec4(dot(normal, ldn.xyz),
						 dot(normal, ddn.xyz),
						 dot(normal, rdn.xyz),
						 dot(normal, lln.xyz));
	vec4 dNormal2 = vec4(dot(normal, rrn.xyz),
						 dot(normal, ltn.xyz),
						 dot(normal, ttn.xyz),
						 dot(normal, rtn.xyz));
	vec4 dotDeltas = abs(dNormal1 - dNormal2);
    vec4 normalResults = step(0.4, dotDeltas);
	
	vec4 c0 = texture(colorBuffer, ld);
	vec4 c1 = texture(colorBuffer, dd);
	vec4 c2 = texture(colorBuffer, rd);
	vec4 c3 = texture(colorBuffer, ll);
	vec4 c4 = texture(colorBuffer, rr);
	vec4 c5 = texture(colorBuffer, lt);
	vec4 c6 = texture(colorBuffer, tt);
	vec4 c7 = texture(colorBuffer, rt);
	vec4 color = texture(colorBuffer, ct);
	
	vec4 dWater1 = vec4(color.a) - vec4(c0.a, c1.a, c2.a, c3.a);
	vec4 dWater2 = vec4(color.a) - vec4(c4.a, c5.a, c6.a, c7.a);
	vec4 waterResulats = step(0.4, abs(dWater1 + dWater2));

	vec4 results = max(normalResults, depthResults) * (1.0 - color.a);
	results = max(waterResulats, results);
	float edgeWeight = dot(results, vec4(1.0)) * 0.25;
	
	if(edgeWeight > 0.25) {
		#ifdef USE_CARTOON
			FragColor = OUTLINE_COLOR;
		#else
			vec3 sum = ZERO_VEC3;
			sum += c0.rgb + c1.rgb + c2.rgb + c3.rgb;
			sum += c4.rgb + c5.rgb + c6.rgb + c7.rgb;
			FragColor = vec4(mix(color.rgb, sum * 0.125, edgeWeight), 1.0);
		#endif
	} else {
		FragColor = vec4(color.rgb, 1.0);
	}	
}
#include "shader/util.glsl"

uniform BindlessSampler2D colorBuffer;
uniform vec2 pixelSize;

in vec2 vTexcoord;

out vec4 FragColor;

const float FXAA_REDUCE_MIN = 0.0078125;
const float FXAA_REDUCE_MUL = 0.125;
const float FXAA_SPAN_MAX = 8.0;
const vec3 Luma = vec3(0.299, 0.587, 0.114);

void main() {
#ifdef USE_FXAA
	float val = 1.0;
	vec2 inverseVP = pixelSize;
	vec3 rgbNW = texture(colorBuffer, vTexcoord + (vec2(-val, -val) * inverseVP)).rgb;
	vec3 rgbNE = texture(colorBuffer, vTexcoord + (vec2(val, -val) * inverseVP)).rgb;
	vec3 rgbSW = texture(colorBuffer, vTexcoord + (vec2(-val, val) * inverseVP)).rgb;
	vec3 rgbSE = texture(colorBuffer, vTexcoord + (vec2(val, val) * inverseVP)).rgb;
	vec3 rgbM  = texture(colorBuffer, vTexcoord).rgb;
	float lumaNW = dot(rgbNW, Luma);
	float lumaNE = dot(rgbNE, Luma);
	float lumaSW = dot(rgbSW, Luma);
	float lumaSE = dot(rgbSE, Luma);
	float lumaM  = dot(rgbM,  Luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	vec2 dir = vec2(-((lumaNW + lumaNE) - (lumaSW + lumaSE)), 
					((lumaNW + lumaSW) - (lumaNE + lumaSE)));
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
		(0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
		max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
		dir * rcpDirMin)) * inverseVP;
	
	vec3 rgbA = 0.5 * (
		texture(colorBuffer, vTexcoord + dir * (-0.167)).rgb +
		texture(colorBuffer, vTexcoord + dir * (0.167)).rgb);
	vec3 rgbB = rgbA * 0.5 + 0.25 * (
		texture(colorBuffer, vTexcoord + dir * (-0.5)).rgb +
		texture(colorBuffer, vTexcoord + dir * (0.5)).rgb);
	
	FragColor = vec4(1.0);
	float lumaB = dot(rgbB, Luma);
	if ((lumaB < lumaMin) || (lumaB > lumaMax)){
		FragColor.rgb = rgbA;
	}else{
		FragColor.rgb = rgbB;
	}
#else
	FragColor = vec4(texture(colorBuffer, vTexcoord).rgb, 1.0);
#endif

	// gamma correction
	#ifdef HIGH_QUALITY
		FragColor.rgb = vec3(1.0) - exp(-FragColor.rgb * 2.5);
	#endif
	FragColor.rgb = pow(FragColor.rgb, INV_GAMMA);
}
#include "shader/util.glsl"

uniform BindlessSampler2D colorBuffer;
uniform vec2 pixelSize;

in vec2 vTexcoord;

out vec4 FragColor;

const float FXAA_REDUCE_MIN = 0.0078125;
const float FXAA_REDUCE_MUL = 0.125;
const float FXAA_SPAN_MAX = 8.0;

void main() {
	float val = 1.0;
	vec2 inverseVP = pixelSize;
	vec3 rgbNW = texture(colorBuffer, vTexcoord + (vec2(-val, -val) * inverseVP)).xyz;
	vec3 rgbNE = texture(colorBuffer, vTexcoord + (vec2(val, -val) * inverseVP)).xyz;
	vec3 rgbSW = texture(colorBuffer, vTexcoord + (vec2(-val, val) * inverseVP)).xyz;
	vec3 rgbSE = texture(colorBuffer, vTexcoord + (vec2(val, val) * inverseVP)).xyz;
	vec3 rgbM  = texture(colorBuffer, vTexcoord).xyz;
	vec3 luma  = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot(rgbM,  luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
		(0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
		max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
		dir * rcpDirMin)) * inverseVP;
	
	vec3 rgbA = 0.5 * (
		texture(colorBuffer, vTexcoord + dir * (1.0/3.0 - 0.5)).xyz +
		texture(colorBuffer, vTexcoord + dir * (2.0/3.0 - 0.5)).xyz);
	vec3 rgbB = rgbA * 0.5 + 0.25 * (
		texture(colorBuffer, vTexcoord + dir * (0.0/3.0 - 0.5)).xyz +
		texture(colorBuffer, vTexcoord + dir * (3.0/3.0 - 0.5)).xyz);
	
	FragColor = vec4(1.0);
	float lumaB = dot(rgbB, luma);
	if ((lumaB < lumaMin) || (lumaB > lumaMax)){
		FragColor.rgb = rgbA;
	}else{
		FragColor.rgb = rgbB;
	}
}
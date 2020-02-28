#include "shader/util.glsl"

uniform mat4 invProjMatrix;
uniform vec2 screenSize, pixelSize;
uniform float time;
layout(bindless_sampler) uniform sampler2D colorBuffer, normalGrassBuffer, depthBuffer;

in vec2 vTexcoord;

out vec4 SceneColor;

vec3 Smudge(vec3 sceneTex, float grassFlag, float viewDist) {
	if(grassFlag < 0.5) 
		return sceneTex;
	else {
		float xx = vTexcoord.x;
		float yy = 1.0 - vTexcoord.y;

		float len = viewDist;
		float d = BlendVal(len, 0.0, 500.0, 100.0, 500.0);
		float dclose = BlendVal(len, 0.0, 20.0, 30.0, 1.0);
		d *= dclose;
		yy += dot(vec3(xx), vec3(1009.0, 1259.0, 2713.0));
		yy += time * 0.004;
		yy += sceneTex.g * 0.4;
		
		float yoffset = 1.0 - fract(yy * d) / d;
		vec2 uvoffset = vTexcoord - vec2(0.0, yoffset);
		vec4 grassColor = texture2D(colorBuffer, uvoffset);

		float depthGrass = texture2D(depthBuffer, uvoffset).r;
		vec3 ndcGrass = vec3(uvoffset, depthGrass) * 2.0 - 1.0;
		vec4 viewGrass = invProjMatrix * vec4(ndcGrass, 1.0);
		viewGrass /= viewGrass.w;
		
		if(viewGrass.z > -viewDist)
			return sceneTex;
		else 
			return mix(sceneTex, grassColor.rgb, saturate(yoffset * d / 3.8));
	}
}

void main() {
	vec4 normalGrass = texture2D(normalGrassBuffer, vTexcoord);
	float grassFlag = normalGrass.a;

	float depth = texture2D(depthBuffer, vTexcoord).r;
	vec3 ndcPos = vec3(vTexcoord, depth) * 2.0 - 1.0;
	vec4 viewPos = invProjMatrix * vec4(ndcPos, 1.0);
	viewPos /= viewPos.w;
	float depthView = abs(viewPos.z);

	vec4 tex = texture2D(colorBuffer, vTexcoord);
	SceneColor = vec4(Smudge(tex.rgb, grassFlag, depthView), tex.a);
}
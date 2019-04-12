#version 330

uniform mat4 invProjMatrix;
uniform vec2 screenSize, pixelSize;
uniform float time;
uniform sampler2D colorBuffer, grassBuffer, depthBuffer;

in vec2 vTexcoord;

out vec4 SceneColor;

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

float Blend(float val, float val0, float val1, float res0, float res1) {
	if (val <= val0) return res0;
	if (val >= val1) return res1;
	return res0 + (val - val0) * (res1 - res0) / (val1 - val0);
}

void main() {
	vec4 sceneTex = texture2D(colorBuffer, vTexcoord);
	vec4 grassFlag = texture2D(grassBuffer, vTexcoord);
	
	if(grassFlag.r < 0.5) 
		SceneColor = sceneTex;
	else {
		float depth = texture2D(depthBuffer, vTexcoord).r;
		vec3 ndcPos = vec3(vTexcoord, depth) * 2.0 - 1.0;
		vec4 viewPos = invProjMatrix * vec4(ndcPos, 1.0);
		viewPos /= viewPos.w;
		
		float len = -viewPos.z;
		float xx = vTexcoord.x;
		float yy = vTexcoord.y;

		float d = Blend(len, 0.0, 500.0, 100.0, 500.0);
		float dclose = Blend(len, 0.0, 1.0, 40.0, 1.0);
		d *= dclose;
		//yy += xx * 1000.0;
		yy += dot(vec3(xx), vec3(1009.0, 1259.0, 2713.0));
		//yy += time * 0.00005;
		yy += dot(viewPos.xyz, vec3(1.0));
		
		float yoffset = 1.0 - fract(yy * d) / d;
		vec2 uvoffset = vTexcoord - vec2(0.0, yoffset);
		vec4 grassColor = texture2D(colorBuffer, uvoffset);

		float depthGrass = texture2D(depthBuffer, uvoffset).r;
		vec3 ndcGrass = vec3(uvoffset, depthGrass) * 2.0 - 1.0;
		vec4 viewGrass = invProjMatrix * vec4(ndcGrass, 1.0);
		viewGrass /= viewGrass.w;
		
		if(viewGrass.z > viewPos.z)
			SceneColor = sceneTex;
		else 
			SceneColor = mix(sceneTex, grassColor, saturate(yoffset * d / 2.8));
	}
	//*/
}
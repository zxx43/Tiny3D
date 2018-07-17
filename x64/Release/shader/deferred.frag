#version 330

uniform sampler2D texBuffer, colorBuffer, normalBuffer, depthBuffer;
uniform vec2 pixelSize;

uniform mat4 invViewProjMatrix;
uniform mat4 viewMatrix;

uniform mat4 lightViewProjNear, lightViewProjMid, lightViewProjFar;
uniform sampler2D depthBufferNear, depthBufferMid, depthBufferFar;
uniform int useShadow;
uniform vec2 levels;
uniform vec3 light;

in vec2 vTexcoord;

out vec3 FragColor;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float genPCF(sampler2D shadowMap, vec3 shadowCoord, float bias, int randround) {
	float shadowFactor = 1.0;
	float mag = 0.00143;
	float roundmag = 1.0 / float(randround);
	for (int i = 0; i < randround; i++) {
		int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
		float factor = texture2D(shadowMap, shadowCoord.xy + poissonDisk[index] * mag).r > (shadowCoord.z - bias) ? 1.0 : 0.0;
		shadowFactor -= roundmag * (1.0 - factor);
	}
	return shadowFactor;
}

float genShadow(sampler2D shadowMap, vec3 shadowCoord, float bias) {
	float shadowFactor = texture2D(shadowMap, shadowCoord.xy).r > (shadowCoord.z - bias) ? 1.0 : 0.0;
	return shadowFactor;
}

float genShadowFactor(vec4 worldPos, float depthView, float bias) {
	float gap = 30.0;
	if(depthView <= levels.x - gap) {
		vec4 near = lightViewProjNear * worldPos;
		float invW = 1.0 / near.w;
		vec3 lightPosition = near.xyz * invW;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		float bs = bias * 0.0005;
		return genPCF(depthBufferNear, shadowCoord, bs, 4);
	} else if(depthView > levels.x - gap && depthView < levels.x + gap) {
		vec4 near = lightViewProjNear * worldPos;
		float invWNear = 1.0 / near.w;
		vec3 lightPositionNear = near.xyz * invWNear;
		vec3 shadowCoordNear = lightPositionNear * 0.5 + 0.5;
		float bsNear = bias * 0.0005;

		vec4 mid = lightViewProjMid * worldPos;
		float invWMid = 1.0 / mid.w;
		vec3 lightPositionMid = mid.xyz * invWMid;
		vec3 shadowCoordMid = lightPositionMid * 0.5 + 0.5;
		float bsMid = bias * 0.00005;

		float factorNear = genPCF(depthBufferNear, shadowCoordNear, bsNear, 4);
		float factorMid = genPCF(depthBufferMid, shadowCoordMid, bsMid, 1);
		return mix(factorNear, factorMid, (depthView - (levels.x - gap)) / (gap * 2.0));
	} else if(depthView <= levels.y) {
		vec4 mid = lightViewProjMid * worldPos;
		float invW = 1.0 / mid.w;
		vec3 lightPosition = mid.xyz * invW;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		float bs = bias * 0.00005;
		return genPCF(depthBufferMid, shadowCoord, bs, 1);
	} 
	///*
	else {
		vec4 far = lightViewProjFar * worldPos;
		float invW = 1.0 / far.w;
		vec3 lightPosition = far.xyz * invW;
		vec3 shadowCoord = lightPosition * 0.5 + 0.5;
		float bs = bias * 0.000005;
		return genShadow(depthBufferFar, shadowCoord, bs);
	}
	//*/
	return 1.0;
}

vec3 genFogColor(vec4 worldPos, float depthView, vec3 sceneColor) {
	float fogStart = 0.0;
	float fogEnd = 1800.0;
	float startH = 200.0;
	float endH = 1600.0;
	vec3 fogColor = vec3(0.9);
	float worldH = worldPos.y / worldPos.w;
	float heightFactor = smoothstep(startH, endH, worldH);
	float fogFactor = (fogEnd - depthView) / (fogEnd - fogStart);
	fogFactor = mix(fogFactor, 1.0, heightFactor);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	return mix(fogColor, sceneColor, fogFactor);
}

void main() {
	float depth = texture2D(depthBuffer, vTexcoord).r;
	vec3 ndcPos = vec3(vTexcoord, depth)  * 2.0 - 1.0;
	vec4 tex = texture2D(texBuffer, vTexcoord);
	vec3 sceneColor = tex.rgb;
	
	vec4 worldPos = invViewProjMatrix * vec4(ndcPos, 1.0);
	vec4 viewPosition = viewMatrix * worldPos;
	float depthView = -viewPosition.z / viewPosition.w;

	if(ndcPos.z < 1.0) {
		vec3 normal = texture2D(normalBuffer, vTexcoord).xyz;
		normal = normal * 2.0 - 1.0;
		
		vec4 color = texture2D(colorBuffer, vTexcoord);

		float ndotl = dot(light, normal);
		float bias = tan(acos(abs(ndotl)));
		ndotl = max(ndotl, 0.0);

		float shadowFactor = 1.0;
		if (tex.a > 0.9)
			shadowFactor = (useShadow != 0) ? genShadowFactor(worldPos, depthView, bias) : 1.0;

		sceneColor = tex.rgb * (color.r + shadowFactor * ndotl * color.g);
	}

	FragColor = genFogColor(worldPos, depthView, sceneColor);
}

#version 330

uniform sampler2D colorBuffer;
uniform sampler2D depthBuffer;
uniform sampler2D normalBuffer;
uniform vec2 pixelSize;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
	float pixW = pixelSize.x;
	float pixH = pixelSize.y;
	
	vec4 sum = vec4(0.0);
	
	/*
	if(vTexcoord.x < 0.333333)
		FragColor = texture2D(depthBuffer, vTexcoord);
	else if(vTexcoord.x < 0.666666)
		FragColor = texture2D(normalBuffer, vTexcoord);
	else
	//*/ 
	//	FragColor = texture2D(colorBuffer, vTexcoord);
	///*
	vec4 tmp = texture2D(colorBuffer, vTexcoord + vec2(-pixW, -pixH));
	sum += tmp * 0.0625;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(0.0, -pixH));
	sum += tmp * 0.125;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(pixW, -pixH));
	sum += tmp * 0.0625;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(-pixW, 0.0));
	sum += tmp * 0.125;
	tmp = texture2D(colorBuffer, vTexcoord);
	sum += tmp * 0.25;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(pixW, 0.0));
	sum += tmp * 0.125;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(-pixW, pixH));
	sum += tmp * 0.0625;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(0.0, pixH));
	sum += tmp * 0.125;
	tmp = texture2D(colorBuffer, vTexcoord + vec2(pixW, pixH));
	sum += tmp * 0.0625;

	FragColor = sum;	
	//*/
}

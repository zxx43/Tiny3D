#version 450
#extension GL_ARB_bindless_texture : enable 

layout(bindless_sampler) uniform sampler2D colorBuffer;
uniform vec2 pixelSize;

in vec2 vTexcoord;

out vec4 FragColor;

void main() {
	float pw = pixelSize.x, ph = pixelSize.y;
	
	vec4 ld = texture2D(colorBuffer, vTexcoord + vec2(-pw, -ph));
	vec4 dd = texture2D(colorBuffer, vTexcoord + vec2(0.0, -ph));
	vec4 rd = texture2D(colorBuffer, vTexcoord + vec2(pw, -ph));
	vec4 ll = texture2D(colorBuffer, vTexcoord + vec2(-pw, 0.0));
	vec4 rr = texture2D(colorBuffer, vTexcoord + vec2(pw, 0.0));
	vec4 lt = texture2D(colorBuffer, vTexcoord + vec2(-pw, ph));
	vec4 tt = texture2D(colorBuffer, vTexcoord + vec2(0.0, ph));
	vec4 rt = texture2D(colorBuffer, vTexcoord + vec2(pw, ph));
	vec4 color = texture2D(colorBuffer, vTexcoord);
	
	float depth = color.w;
	vec4 depth1 = vec4(ld.w, dd.w, rd.w, ll.w);
	vec4 depth2 = vec4(rr.w, lt.w, tt.w, rt.w);
	
	vec4 dDepth1 = abs(depth1 - depth);
	vec4 dDepth2 = abs(depth2 - depth);
	
	vec4 minDDepth = max(min(dDepth1, dDepth2), 0.00001);
	vec4 maxDDepth = max(dDepth1, dDepth2);
	vec4 dDepth = step(minDDepth * 10.0, maxDDepth);
	float edgeWeight = dot(dDepth, vec4(1.0)) * 0.25;
	
	if(edgeWeight > 0.0001) {
		vec3 sum = vec3(0.0);
		sum += ld.rgb + dd.rgb + rd.rgb + ll.rgb;
		sum += rr.rgb + lt.rgb + tt.rgb + rt.rgb;
		FragColor = vec4(mix(color.rgb, sum * 0.125, edgeWeight), 1.0);
	} else {
		FragColor = vec4(color.rgb, 1.0);
	}	
}
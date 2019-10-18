layout(bindless_sampler) uniform sampler2D texBlds[MAX_TEX];

in vec2 vTexcoord;
flat in vec4 vTexid;

void main() {
	if(texture(texBlds[int(vTexid.x)], vTexcoord.xy).a < 0.3) discard;
}
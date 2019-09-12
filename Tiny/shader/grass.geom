#version 330
 
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 viewProjectMatrix;
 
in VertexData {
	vec2 vTexcoord;
	flat float vTexid;
	vec3 vNormal;
	flat vec3 vColor;
	flat float vOutBound;
} vertIn[3];
 
out GeomData {
	vec2 vTexcoord;
	flat float vTexid;
	vec3 vNormal;
	flat vec3 vColor;
} vertOut;
 
void main() {
	if(vertIn[0].vOutBound < 0.1) {
		gl_Position = gl_in[0].gl_Position;
		vertOut.vNormal = vertIn[0].vNormal;
		vertOut.vTexcoord = vertIn[0].vTexcoord;
		vertOut.vTexid = vertIn[0].vTexid;
		vertOut.vColor = vertIn[0].vColor;
		EmitVertex();

		gl_Position = gl_in[1].gl_Position;
		vertOut.vNormal = vertIn[1].vNormal;
		vertOut.vTexcoord = vertIn[1].vTexcoord;
		vertOut.vTexid = vertIn[1].vTexid;
		vertOut.vColor = vertIn[1].vColor;
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		vertOut.vNormal = vertIn[2].vNormal;
		vertOut.vTexcoord = vertIn[2].vTexcoord;
		vertOut.vTexid = vertIn[2].vTexid;
		vertOut.vColor = vertIn[2].vColor;
		EmitVertex();

		EndPrimitive(); 
	}
}
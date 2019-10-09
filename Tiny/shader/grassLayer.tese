layout(triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
in vec3 tcNormal[];

out vec3 tePosition;
out vec3 teNormal;

void main() {
	tePosition = gl_TessCoord.x * tcPosition[0] + 
				 gl_TessCoord.y * tcPosition[1] + 
				 gl_TessCoord.z * tcPosition[2];

	teNormal = gl_TessCoord.x * tcNormal[0] + 
			   gl_TessCoord.y * tcNormal[1] + 
			   gl_TessCoord.z * tcNormal[2];
}
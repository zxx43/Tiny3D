layout(triangles, equal_spacing, ccw, point_mode) in;

uniform mat4 viewProjectMatrix;

in vec3 tcPosition[];
in vec3 tcNormal[];

out vec3 tePosition;
out vec4 teProjPos;
out vec3 teNormal;
out vec4 teInfo;

void main() {
	vec3 pos =  gl_TessCoord.x * tcPosition[0] + 
				gl_TessCoord.y * tcPosition[1] + 
				gl_TessCoord.z * tcPosition[2];

	vec2 randTrans = vec2(random(pos, 0.1), random(pos.xzy, 0.2)); 
	pos.xz += randTrans;

	tePosition = pos;
	teProjPos = viewProjectMatrix * vec4(pos, 1.0);
	teInfo = vec4(randTrans, 0.0, 0.0);
	teNormal = gl_TessCoord.x * tcNormal[0] + 
			   gl_TessCoord.y * tcNormal[1] + 
			   gl_TessCoord.z * tcNormal[2];
}
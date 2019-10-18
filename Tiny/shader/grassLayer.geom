layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

layout(bindless_sampler) uniform sampler2D texBlds[MAX_TEX];
uniform mat4 viewProjectMatrix;
uniform mat4 viewMatrix;
uniform float time, distortionId;

in vec3 tePosition[];
in vec4 teProjPos[];
in vec3 teNormal[];
in vec4 teInfo[];

out vec4 vNormalHeight;

const float WindStrength = 0.6;
const vec2 WindFrequency = vec2(0.05, 0.05);

mat3 AngleAxis3x3(float angle, vec3 axis) {
	float c = cos(angle), s = sin(angle);

	float t = 1.0 - c;
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	
	return mat3(
		t * x * x + c, t * x * y + s * z, t * x * z - s * y,
		t * x * y - s * z, t * y * y + c, t * y * z + s * x,
		t * x * z + s * y, t * y * z - s * x, t * z * z + c
	);
}

mat3 GetWindMat(vec2 pos) {
	vec2 uv = pos * 0.01 + WindFrequency * time;
	vec2 windSample = texture(texBlds[int(distortionId)], uv).xy * 2.0 - 1.0;
	windSample *= WindStrength;
	vec3 wind = normalize(vec3(windSample.x, windSample.y, 0.0));
	mat3 windRotation = AngleAxis3x3(PI * windSample.x, wind);
	return windRotation;
}

void main() {
	vec4 projPos = teProjPos[0];
	if(projPos.z > 0.0 && 
			projPos.x > -projPos.w && projPos.x < projPos.w && 
			projPos.y < projPos.w) {

		vec4 info = teInfo[0];
		vec3 trans = tePosition[0];
		vec3 normal = teNormal[0];

		float rand = info.x + info.y;

		vec2 size = vec2(rand * 0.5 + 0.1, rand * 2.5 + 0.5);
		vec3 vertA = vec3(-size.x, 0.0, 0.0), vertB = -vertA, vertC = vec3(0.0, size.y, 0.0);
		vec2 hbt = vec2(vertA.y, vertC.y) / size.y;

		mat3 rotMat = RotY(rand * PI);
		float viewz = (viewMatrix * vec4(trans, 1.0)).z;
		mat3 windMat = GetWindMat(trans.xz + vec2(viewz));
		rotMat = rotMat * windMat;

		vertA = rotMat * vertA + trans;
		vertB = rotMat * vertB + trans;
		vertC = rotMat * vertC + trans;

		vNormalHeight = vec4(normal, hbt.x);
		gl_Position = viewProjectMatrix * vec4(vertA, 1.0);
		EmitVertex();
	
		vNormalHeight = vec4(normal, hbt.x);
		gl_Position = viewProjectMatrix * vec4(vertB, 1.0);
		EmitVertex();
	
		vNormalHeight = vec4(normal, hbt.y);
		gl_Position = viewProjectMatrix * vec4(vertC, 1.0);
		EmitVertex();
	
		EndPrimitive(); 
	}
}
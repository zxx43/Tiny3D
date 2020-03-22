#include "util.h"

mat4 lookAt(float eyeX, float eyeY, float eyeZ,
		float centerX, float centerY, float centerZ,
		float upX, float upY, float upZ) {
	static vec3 eye, center, up;
	eye.x=eyeX;
	eye.y=eyeY;
	eye.z=eyeZ;
	center.x=centerX;
	center.y=centerY;
	center.z=centerZ;
	up.x=upX;
	up.y=upY;
	up.z=upZ;

	vec3 f=center - eye;
	f.Normalize();
	vec3 s=f.CrossProduct(up);
	s.Normalize();
	vec3 u=s.CrossProduct(f);
	u.Normalize();

	mat4 mat;
	mat.entries[0]=s.x;
	mat.entries[4]=s.y;
	mat.entries[8]=s.z;
	mat.entries[1]=u.x;
	mat.entries[5]=u.y;
	mat.entries[9]=u.z;
	mat.entries[2]=-f.x;
	mat.entries[6]=-f.y;
	mat.entries[10]=-f.z;
	mat.entries[12]=-(s.DotProduct(eye));
	mat.entries[13]=-(u.DotProduct(eye));
	mat.entries[14]=f.DotProduct(eye);
	return mat;
}

mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
	vec3 f = center - eye;
	f.Normalize();
	vec3 s = f.CrossProduct(up);
	s.Normalize();
	vec3 u = s.CrossProduct(f);
	u.Normalize();

	mat4 mat;
	mat.entries[0] = s.x;
	mat.entries[4] = s.y;
	mat.entries[8] = s.z;
	mat.entries[1] = u.x;
	mat.entries[5] = u.y;
	mat.entries[9] = u.z;
	mat.entries[2] = -f.x;
	mat.entries[6] = -f.y;
	mat.entries[10] = -f.z;
	mat.entries[12] = -(s.DotProduct(eye));
	mat.entries[13] = -(u.DotProduct(eye));
	mat.entries[14] = f.DotProduct(eye);
	return mat;
}

mat4 viewMat(const vec3& x, const vec3& y, const vec3& z, const vec3& position) {
	mat4 mat;
	mat.entries[0] = x.x;
	mat.entries[1] = y.x;
	mat.entries[2] = z.x;
	mat.entries[3] = 0.0f;
	mat.entries[4] = x.y;
	mat.entries[5] = y.y;
	mat.entries[6] = z.y;
	mat.entries[7] = 0.0f;
	mat.entries[8] = x.z;
	mat.entries[9] = y.z;
	mat.entries[10] = z.z;
	mat.entries[11] = 0.0f;
	mat.entries[12] = -(x.DotProduct(position));
	mat.entries[13] = -(y.DotProduct(position));
	mat.entries[14] = -(z.DotProduct(position));
	mat.entries[15] = 1.0f;
	return mat;
}

mat4 perspective(float fovy,float aspect,float zNear,float zFar) {
	float rFovy=fovy*A2R;
	float tanHalfFovy=tanf(rFovy*0.5);

	mat4 mat;
	mat.entries[0]=1.0/(aspect * tanHalfFovy);
	mat.entries[5]=1.0/tanHalfFovy;
	mat.entries[10]=-(zFar + zNear)/(zFar - zNear);
	mat.entries[11]=-1.0;
	mat.entries[14]=-2.0*zFar*zNear/(zFar - zNear);
	mat.entries[15]=0.0;
	return mat;
}

mat4 ortho(float left, float right, float bottom, float top, float n, float f) {
	mat4 mat;
	mat.entries[0]=2.0f/(right-left);
	mat.entries[5]=2.0f/(top-bottom);
	mat.entries[10]=-2.0f/(f-n);
	mat.entries[12]=-(right+left)/(right-left);
	mat.entries[13]=-(top+bottom)/(top-bottom);
	mat.entries[14]=-(f+n)/(f-n);
	return mat;
}

int project(float objX, float objY, float objZ,
		const float* model, const float* proj, const int* view,
		float* winX, float* winY, float* winZ) {
	vec4 point;
	point.x=objX;
	point.y=objY;
	point.z=objZ;
	point.w=1.0f;

	mat4 modelMat,projMat;
	modelMat = model;
	projMat = proj;
	point=modelMat * point;
	point=projMat * point;
	point /= point.w;

	vec4 bias(0.5,0.5,0.5,0.5);
	point = (point * 0.5);
	point = (point + bias);

	point.x = point.x * view[2] + view[0];
	point.y = point.y * view[3] + view[1];
	*winX=point.x;
	*winY=point.y;
	*winZ=point.z;
	return 1;
}

mat4 rotateX(float angle) {
	float radian=angle*A2R;
	float sinA=sinf(radian);
	float cosA=cosf(radian);
	mat4 mat;
	mat.entries[5]=cosA;
	mat.entries[6]=sinA;
	mat.entries[9]=-sinA;
	mat.entries[10]=cosA;
	return mat;
}

mat4 rotateY(float angle) {
	float radian=angle*A2R;
	float sinA=sinf(radian);
	float cosA=cosf(radian);
	mat4 mat;
	mat.entries[0]=cosA;
	mat.entries[2]=-sinA;
	mat.entries[8]=sinA;
	mat.entries[10]=cosA;
	return mat;
}

mat4 rotateZ(float angle) {
	float radian=angle*A2R;
	float sinA=sinf(radian);
	float cosA=cosf(radian);
	mat4 mat;
	mat.entries[0]=cosA;
	mat.entries[1]=sinA;
	mat.entries[4]=-sinA;
	mat.entries[5]=cosA;
	return mat;
}

mat4 scale(float size) {
	mat4 mat;
	mat.entries[0]=size;
	mat.entries[5]=size;
	mat.entries[10]=size;
	return mat;
}

mat4 scale(float sx, float sy, float sz) {
	mat4 mat;
	mat.entries[0] = sx;
	mat.entries[5] = sy;
	mat.entries[10] = sz;
	return mat;
}

mat4 scaleX(float size) {
	mat4 mat;
	mat.entries[0]=size;
	return mat;
}

mat4 scaleY(float size) {
	mat4 mat;
	mat.entries[5]=size;
	return mat;
}

mat4 scaleZ(float size) {
	mat4 mat;
	mat.entries[10]=size;
	return mat;
}

mat4 translate(float tx,float ty,float tz) {
	mat4 mat;
	mat.entries[12]=tx;
	mat.entries[13]=ty;
	mat.entries[14]=tz;
	return mat;
}

mat4 translate(const vec3& t) {
	mat4 mat;
	mat.entries[12] = t.x;
	mat.entries[13] = t.y;
	mat.entries[14] = t.z;
	return mat;
}

vec4 mul(const vec4& a,const vec4& b) {
	vec4 vec;
	vec.x=a.x*b.x;
	vec.y=a.y*b.y;
	vec.z=a.z*b.z;
	vec.w=a.w*b.w;
	return vec;
}

vec3 mul(const vec3& a, const vec3& b) {
	vec3 vec;
	vec.x = a.x*b.x;
	vec.y = a.y*b.y;
	vec.z = a.z*b.z;
	return vec;
}

bool CaculateIntersect(const Line* line, const Plane* plane, const float lineDistance, vec3& result) {
	vec3 p = line->origin;// (x1, y1, z1)
	vec3 v = line->dir;// (Vx, Vy, Vz)   

	// from plane: ax + by + cz + d = 0
	vec3 n = plane->normal;// (a, b, c)
	float d = plane->d;// constant term of plane

	// dot products
	float dot1 = n.DotProduct(v);// a*Vx + b*Vy + c*Vz
	float dot2 = n.DotProduct(p);// a*x1 + b*y1 + c*z1

	// if denominator=0, no intersect
	if (dot1 == 0) return false;

	// find t = -(a*x1 + b*y1 + c*z1 + d) / (a*Vx + b*Vy + c*Vz)
	float t = -(dot2 + d) / dot1;
	if (t < 0 || t > lineDistance) // intersect is outside
		return false;

	// find intersection point
	result = p + (t * v);
	return true;
}

vec3 CaculateTangent(const vec3& p0, const vec3& p1, const vec3& p2, const vec2& t0, const vec2& t1, const vec2& t2) {
	vec3 deltaPos1 = p1 - p0;
	vec3 deltaPos2 = p2 - p0;
	vec2 deltaUV1 = t1 - t0;
	vec2 deltaUV2 = t2 - t0;
	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	tangent.Normalize();
	return tangent;
}
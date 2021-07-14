#include "frustum.h"

uint Frustum::PlaneVertexIndex[24] = {
	0, 4, 6, 2, 3, 7, 5, 6,
	1, 5, 4, 0, 2, 6, 7, 3,
	0, 2, 3, 1, 4, 5, 7, 6
};

Frustum::Frustum() {
	ndcVertex[0]=vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	ndcVertex[1]=vec4(1.0f, -1.0f, 1.0f, 1.0f);
	ndcVertex[2]=vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	ndcVertex[3]=vec4(1.0f, 1.0f, 1.0f, 1.0f);
	ndcVertex[4]=vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	ndcVertex[5]=vec4(1.0f, -1.0f, -1.0f, 1.0f);
	ndcVertex[6]=vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	ndcVertex[7]=vec4(1.0f, 1.0f, -1.0f, 1.0f);
}

Frustum::~Frustum() {}

void Frustum::update(const mat4& invViewProjectMatrix, const vec3& lookDir) {
	static vec4 worldVert(0, 0, 0, 1);
	for (int i = 0; i<8; i++) {
		worldVert = invViewProjectMatrix * ndcVertex[i];
		float invW = 1.0 / worldVert.w;
		worldVertex[i].x = worldVert.x * invW;
		worldVertex[i].y = worldVert.y * invW;
		worldVertex[i].z = worldVert.z * invW;
	}

	normals[0] = (worldVertex[0] - worldVertex[4]).CrossProduct(worldVertex[2] - worldVertex[4]);
	normals[1] = (worldVertex[3] - worldVertex[5]).CrossProduct(worldVertex[1] - worldVertex[5]);
	normals[2] = (worldVertex[1] - worldVertex[4]).CrossProduct(worldVertex[0] - worldVertex[4]);
	normals[3] = (worldVertex[2] - worldVertex[6]).CrossProduct(worldVertex[3] - worldVertex[6]);
	normals[4] = -lookDir;
	normals[5] = lookDir;
	normals[0].Normalize();
	normals[1].Normalize();
	normals[2].Normalize();
	normals[3].Normalize();

	ds[0] = -normals[0].DotProduct(worldVertex[2]);
	ds[1] = -normals[1].DotProduct(worldVertex[3]);
	ds[2] = -normals[2].DotProduct(worldVertex[1]);
	ds[3] = -normals[3].DotProduct(worldVertex[6]);
	ds[4] = -normals[4].DotProduct(worldVertex[0]);
	ds[5] = -normals[5].DotProduct(worldVertex[4]);

	for (int i = 0; i<4; i++) {
		edgeDir[i] = worldVertex[i] - worldVertex[i + 4];
		edgeLength[i] = edgeDir[i].GetLength();
		edgeDir[i] /= edgeLength[i];
	}

	planes[0].update(normals[0], ds[0]);
	planes[1].update(normals[1], ds[1]);
	planes[2].update(normals[2], ds[2]);
	planes[3].update(normals[3], ds[3]);
	planes[4].update(normals[4], ds[4]);
	planes[5].update(normals[5], ds[5]);
}

bool Frustum::intersectsWidthRay(const vec3& origin, const vec3& dir, float maxDistance) {
	Line line(dir, origin);
	static vec3 interPoint(0, 0, 0);
	for (uint i = 0; i < 6; i++) {
	//for (uint i = 0; i < 5; i++) {
		bool isInter = CaculateIntersect(&line, &planes[i], maxDistance, interPoint);
		if (!isInter) continue;
		uint pi0 = PlaneVertexIndex[i * 4 + 0];
		uint pi1 = PlaneVertexIndex[i * 4 + 1];
		uint pi2 = PlaneVertexIndex[i * 4 + 2];
		uint pi3 = PlaneVertexIndex[i * 4 + 3];
		vec3 a = worldVertex[pi0];
		vec3 b = worldVertex[pi1];
		vec3 c = worldVertex[pi2];
		vec3 d = worldVertex[pi3];
		vec3 ia = a - interPoint;
		vec3 ib = b - interPoint;
		vec3 ic = c - interPoint;
		vec3 id = d - interPoint;
		vec3 aib = ia.CrossProduct(ib);
		vec3 bic = ib.CrossProduct(ic);
		vec3 cid = ic.CrossProduct(id);
		vec3 dia = id.CrossProduct(ia);
		if ((aib.DotProduct(bic) >= 0 && bic.DotProduct(cid) >= 0 && cid.DotProduct(dia) >= 0 && dia.DotProduct(aib) >= 0) ||
			(aib.DotProduct(bic) <= 0 && bic.DotProduct(cid) <= 0 && cid.DotProduct(dia) <= 0 && dia.DotProduct(aib) <= 0))
			return true;
	}
	return false;
}

bool Frustum::checkSphereIn(const vec3& origin, const float radius) {
	for (int i = 0; i < 6; ++i) {
		if (planes[i].pointDistance(origin) < -radius)
			return false;
	}
	return true;
}
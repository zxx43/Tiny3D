#include "frustum.h"

Frustum::Frustum() {
	ndcVertex[0]=VECTOR4D(-1.0f, -1.0f, 1.0f, 1.0f);
	ndcVertex[1]=VECTOR4D(1.0f, -1.0f, 1.0f, 1.0f);
	ndcVertex[2]=VECTOR4D(-1.0f, 1.0f, 1.0f, 1.0f);
	ndcVertex[3]=VECTOR4D(1.0f, 1.0f, 1.0f, 1.0f);
	ndcVertex[4]=VECTOR4D(-1.0f, -1.0f, -1.0f, 1.0f);
	ndcVertex[5]=VECTOR4D(1.0f, -1.0f, -1.0f, 1.0f);
	ndcVertex[6]=VECTOR4D(-1.0f, 1.0f, -1.0f, 1.0f);
	ndcVertex[7]=VECTOR4D(1.0f, 1.0f, -1.0f, 1.0f);

	planeVertexIndex[0] = 0; planeVertexIndex[1] = 4; planeVertexIndex[2] = 6; planeVertexIndex[3] = 2;
	planeVertexIndex[4] = 3; planeVertexIndex[5] = 7; planeVertexIndex[6] = 5; planeVertexIndex[7] = 6;
	planeVertexIndex[8] = 1; planeVertexIndex[9] = 5; planeVertexIndex[10] = 4; planeVertexIndex[11] = 0;
	planeVertexIndex[12] = 2; planeVertexIndex[13] = 6; planeVertexIndex[14] = 7; planeVertexIndex[15] = 3;
	planeVertexIndex[16] = 0; planeVertexIndex[17] = 2; planeVertexIndex[18] = 3; planeVertexIndex[19] = 1;
	planeVertexIndex[20] = 4; planeVertexIndex[21] = 5; planeVertexIndex[22] = 7; planeVertexIndex[23] = 6;
}

Frustum::~Frustum() {}

void Frustum::update(const MATRIX4X4& invViewProjectMatrix, const VECTOR3D& lookDir) {
	static VECTOR4D worldVert(0, 0, 0, 1);
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

bool Frustum::intersectsWidthRay(const VECTOR3D& origin, const VECTOR3D& dir, float maxDistance) {
	Line line(dir, origin);
	static VECTOR3D interPoint(0, 0, 0);
	for (uint i = 0; i < 6; i++) {
	//for (uint i = 0; i < 5; i++) {
		bool isInter = CaculateIntersect(&line, &planes[i], maxDistance, interPoint);
		if (!isInter) continue;
		uint pi0 = planeVertexIndex[i * 4];
		uint pi1 = planeVertexIndex[i * 4 + 1];
		uint pi2 = planeVertexIndex[i * 4 + 2];
		uint pi3 = planeVertexIndex[i * 4 + 3];
		VECTOR3D a = worldVertex[pi0];
		VECTOR3D b = worldVertex[pi1];
		VECTOR3D c = worldVertex[pi2];
		VECTOR3D d = worldVertex[pi3];
		VECTOR3D ia = a - interPoint;
		VECTOR3D ib = b - interPoint;
		VECTOR3D ic = c - interPoint;
		VECTOR3D id = d - interPoint;
		VECTOR3D aib = ia.CrossProduct(ib);
		VECTOR3D bic = ib.CrossProduct(ic);
		VECTOR3D cid = ic.CrossProduct(id);
		VECTOR3D dia = id.CrossProduct(ia);
		if ((aib.DotProduct(bic) >= 0 && bic.DotProduct(cid) >= 0 && cid.DotProduct(dia) >= 0 && dia.DotProduct(aib) >= 0) ||
			(aib.DotProduct(bic) <= 0 && bic.DotProduct(cid) <= 0 && cid.DotProduct(dia) <= 0 && dia.DotProduct(aib) <= 0))
			return true;
	}
	return false;
}
#include "aabb.h"

AABB::AABB(const vec3& min,const vec3& max) :BoundingBox() {
	minVertex.x=min.x; minVertex.y=min.y; minVertex.z=min.z;
	maxVertex.x=max.x; maxVertex.y=max.y; maxVertex.z=max.z;
	sizex=maxVertex.x-minVertex.x;
	sizey=maxVertex.y-minVertex.y;
	sizez=maxVertex.z-minVertex.z;
	halfSize = vec3(sizex, sizey, sizez) * 0.5;
	position.x = minVertex.x + halfSize.x;
	position.y = minVertex.y + halfSize.y;
	position.z = minVertex.z + halfSize.z;

	vertices[0]=vec3(min.x,min.y,min.z);
	vertices[1]=vec3(max.x,min.y,min.z);
	vertices[2]=vec3(min.x,max.y,min.z);
	vertices[3]=vec3(max.x,max.y,min.z);
	vertices[4]=vec3(min.x,min.y,max.z);
	vertices[5]=vec3(max.x,min.y,max.z);
	vertices[6]=vec3(min.x,max.y,max.z);
	vertices[7]=vec3(max.x,max.y,max.z);

	caculateRadius();
}

AABB::AABB(const vec3& pos,float sx,float sy,float sz) :BoundingBox() {
	minVertex.x=pos.x-sx*0.5; minVertex.y=pos.y-sy*0.5; minVertex.z=pos.z-sz*0.5;
	maxVertex.x=pos.x+sx*0.5; maxVertex.y=pos.y+sy*0.5; maxVertex.z=pos.z+sz*0.5;
	sizex=sx; sizey=sy; sizez=sz;
	halfSize = vec3(sizex, sizey, sizez) * 0.5;
	position.x=pos.x; position.y=pos.y; position.z=pos.z;

	vertices[0]=vec3(minVertex.x,minVertex.y,minVertex.z);
	vertices[1]=vec3(maxVertex.x,minVertex.y,minVertex.z);
	vertices[2]=vec3(minVertex.x,maxVertex.y,minVertex.z);
	vertices[3]=vec3(maxVertex.x,maxVertex.y,minVertex.z);
	vertices[4]=vec3(minVertex.x,minVertex.y,maxVertex.z);
	vertices[5]=vec3(maxVertex.x,minVertex.y,maxVertex.z);
	vertices[6]=vec3(minVertex.x,maxVertex.y,maxVertex.z);
	vertices[7]=vec3(maxVertex.x,maxVertex.y,maxVertex.z);

	caculateRadius();
}

AABB::AABB(const AABB& rhs) :BoundingBox(rhs) {
	minVertex.x=rhs.minVertex.x; minVertex.y=rhs.minVertex.y; minVertex.z=rhs.minVertex.z;
	maxVertex.x=rhs.maxVertex.x; maxVertex.y=rhs.maxVertex.y; maxVertex.z=rhs.maxVertex.z;
	sizex=rhs.sizex;
	sizey=rhs.sizey;
	sizez=rhs.sizez;
	halfSize = rhs.halfSize;
	position.x=rhs.position.x;
	position.y=rhs.position.y;
	position.z=rhs.position.z;
	for(int i=0;i<8;i++)
		vertices[i]=rhs.vertices[i];

	caculateRadius();
}

AABB::~AABB() {

}

void AABB::caculateRadius() {
	radius = halfSize.GetLength();
}

void AABB::update(const vec3& newMinVertex,const vec3& newMaxVertex) {
	minVertex = newMinVertex, maxVertex = newMaxVertex;
	vec3 size = maxVertex - minVertex;
	sizex = size.x, sizey = size.y, sizez = size.z;
	halfSize = size * 0.5;
	position = minVertex + halfSize;

	vertices[0] = vec3(newMinVertex.x, newMinVertex.y, newMinVertex.z);
	vertices[1] = vec3(newMaxVertex.x, newMinVertex.y, newMinVertex.z);
	vertices[2] = vec3(newMinVertex.x, newMaxVertex.y, newMinVertex.z);
	vertices[3] = vec3(newMaxVertex.x, newMaxVertex.y, newMinVertex.z);
	vertices[4] = vec3(newMinVertex.x, newMinVertex.y, newMaxVertex.z);
	vertices[5] = vec3(newMaxVertex.x, newMinVertex.y, newMaxVertex.z);
	vertices[6] = vec3(newMinVertex.x, newMaxVertex.y, newMaxVertex.z);
	vertices[7] = vec3(newMaxVertex.x, newMaxVertex.y, newMaxVertex.z);

	caculateRadius();
}

void AABB::update(float sx, float sy, float sz) {
	update(vec3(position.x - sx*0.5, position.y - sy*0.5, position.z - sz*0.5),
		vec3(position.x + sx*0.5, position.y + sy*0.5, position.z + sz*0.5));
}

void AABB::update(const vec3& pos) {
	update(pos - halfSize, pos + halfSize);
}

AABB* AABB::clone() {
	return new AABB(*this);
}

bool AABB::sphereInCamera(Frustum* frustum) {
	return frustum->checkSphereIn(position, radius);
}

bool AABB::vertexInsideCamera(const vec3& vertex, const Frustum* frustum) {
	for(int i=0;i<6;i++) {
		if(frustum->normals[i].DotProduct(vertex)+frustum->ds[i]<0)
			return false;
	}
	return true;
}

bool AABB::intersectsWidthRay(const vec3& origin,const vec3& dir,float maxDistance) {
	float distance=0;
	static vec3 vertex;

	if(dir.x!=0) {
		float d[2]={vertices[0].x,vertices[7].x};
		float invDirX = 1.0 / dir.x;
		for(int i=0;i<2;i++) {
			distance=(d[i]-origin.x)*invDirX;
			if(distance>=0&&distance<=maxDistance) {
				vertex=dir*distance+origin;
				if(vertex.y>=vertices[0].y&&vertex.y<=vertices[7].y&&vertex.z>=vertices[0].z&&vertex.z<=vertices[7].z)
					return true;
			}
		}
	}

	if(dir.y!=0) {
		float d[2]={vertices[0].y,vertices[7].y};
		float invDirY = 1.0 / dir.y;
		for(int i=0;i<2;i++) {
			distance=(d[i]-origin.y)*invDirY;
			if(distance>=0&&distance<=maxDistance) {
				vertex=dir*distance+origin;
				if(vertex.x>=vertices[0].x&&vertex.x<=vertices[7].x&&vertex.z>=vertices[0].z&&vertex.z<=vertices[7].z)
					return true;
			}
		}
	}

	if(dir.z!=0) {
		float d[2]={vertices[0].z,vertices[7].z};
		float invDirZ = 1.0 / dir.z;
		for(int i=0;i<2;i++) {
			distance=(d[i]-origin.z)*invDirZ;
			if(distance>=0&&distance<=maxDistance) {
				vertex=dir*distance+origin;
				if(vertex.x>=vertices[0].x&&vertex.x<=vertices[7].x&&vertex.y>=vertices[0].y&&vertex.y<=vertices[7].y)
					return true;
			}
		}
	}

	return false;
}

bool AABB::cameraVertexInside(const vec3& vertex) {
	if(vertex.x<minVertex.x)
		return false;
	if(vertex.x>maxVertex.x)
		return false;
	if(vertex.y<minVertex.y)
		return false;
	if(vertex.y>maxVertex.y)
		return false;
	if(vertex.z<minVertex.z)
		return false;
	if(vertex.z>maxVertex.z)
		return false;
	return true;
}

bool AABB::checkWithCamera(Frustum* frustum, int checkLevel) {
	if (checkLevel < 1) return true;

	for (int i = 0; i < 8; i++) {
		if (vertexInsideCamera(vertices[i], frustum))
			return true;
	}

	if (checkLevel >= 2) {
		for (int i = 0; i < 8; i++) {
			if (cameraVertexInside(frustum->worldVertex[i]))
				return true;
		}

		for (int i = 0; i < 4; i++) {
			if (intersectsWidthRay(frustum->worldVertex[i + 4],
					frustum->edgeDir[i], frustum->edgeLength[i]))
				return true;
		}
	}

	if (checkLevel >= 3) {
		static vec3 right = vec3(1, 0, 0);
		static vec3 far = vec3(0, 0, 1);
		static vec3 left = vec3(-1, 0, 0);
		static vec3 near = vec3(0, 0, -1);

		if (frustum->intersectsWidthRay(vertices[0], right, sizex))
			return true;
		if (frustum->intersectsWidthRay(vertices[0], far, sizez))
			return true;
		if (frustum->intersectsWidthRay(vertices[7], left, sizex))
			return true;
		if (frustum->intersectsWidthRay(vertices[7], near, sizez))
			return true;
		if (frustum->intersectsWidthRay(vertices[4], right, sizex))
			return true;
		if (frustum->intersectsWidthRay(vertices[3], left, sizex))
			return true;
		if (frustum->intersectsWidthRay(vertices[2], far, sizez))
			return true;
		if (frustum->intersectsWidthRay(vertices[1], far, sizez))
			return true;
	}

	if (checkLevel >= 4) {
		static vec3 up = vec3(0, 1, 0);
		static vec3 down = vec3(0, -1, 0);

		if (frustum->intersectsWidthRay(vertices[0], up, sizey))
			return true;
		if (frustum->intersectsWidthRay(vertices[7], down, sizey))
			return true;
		if (frustum->intersectsWidthRay(vertices[4], up, sizey))
			return true;
		if (frustum->intersectsWidthRay(vertices[1], up, sizey))
			return true;
	}

	return false;
}

void AABB::merge(const std::vector<BoundingBox*>& others) {
	if (others.size() > 0) {
		AABB* first = (AABB*)(others[0]);
		vec3 min = first->minVertex;
		vec3 max = first->maxVertex;
		if (others.size() > 1) {
			for (unsigned int i = 1; i < others.size(); i++) {
				AABB* other = (AABB*)(others[i]);
				float sx = other->minVertex.x;
				float sy = other->minVertex.y;
				float sz = other->minVertex.z; 
				float lx = other->maxVertex.x;
				float ly = other->maxVertex.y;
				float lz = other->maxVertex.z;

				min.x = min.x > sx ? sx : min.x;
				min.y = min.y > sy ? sy : min.y;
				min.z = min.z > sz ? sz : min.z;
				max.x = max.x < lx ? lx : max.x;
				max.y = max.y < ly ? ly : max.y;
				max.z = max.z < lz ? lz : max.z;
			}
		}
		minVertex.x = min.x; minVertex.y = min.y; minVertex.z = min.z;
		maxVertex.x = max.x; maxVertex.y = max.y; maxVertex.z = max.z;
		update(minVertex, maxVertex);
	}
}


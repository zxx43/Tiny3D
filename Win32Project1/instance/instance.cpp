#include "instance.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"

std::map<Mesh*, int> Instance::instanceTable;

Instance::Instance(InstanceData* data) : DataBuffer(STATICS_BUFFER) {
	create(data->insMesh);
	maxCount = data->maxInsCount;
	insData = NULL;
}

Instance::Instance(Mesh* mesh) : DataBuffer(STATICS_BUFFER) {
	create(mesh);
	insData = NULL;
}

void Instance::create(Mesh* mesh) {
	insId = InvalidInsId, insSingleId = InvalidInsId, insBillId = InvalidInsId;
	instanceMesh = mesh;

	isBillboard = instanceMesh->isBillboard;
	hasNormal = instanceMesh->normalFaces.size() > 0 && !isBillboard;
	hasSingle = instanceMesh->singleFaces.size() > 0 && !isBillboard;
}

Instance::~Instance() {
	releaseDatas();
}

void Instance::releaseDatas() {
	DataBuffer::releaseDatas();
}

void Instance::initInstanceBuffers(Object* object,int vertices,int indices,int cnt,bool copy) {
	vertexCount = vertices;
	vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	normalBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	tangentBuffer = (half*)malloc(vertexCount * 3 * sizeof(half));
	texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	texidBuffer = (float*)malloc(vertexCount * 2 * sizeof(float));
	colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));

	indexCount=indices;
	if (indexCount > 0)
		indexBuffer = (ushort*)malloc(indexCount*sizeof(ushort));

	for(int i=0;i<vertexCount;i++) {
		vec4 vertex=instanceMesh->vertices[i];
		vec3 normal=instanceMesh->normals[i];
		vec3 tangent = instanceMesh->tangents[i];
		vec2 texcoord=instanceMesh->texcoords[i];

		int mid = object->material;
		if (isBillboard) mid = object->billboard->material;
		Material* mat = NULL;
		if (mid >= 0)
			mat = MaterialManager::materials->find(mid);
		else if (instanceMesh->materialids) {
			mid = instanceMesh->materialids[i];
			mat = MaterialManager::materials->find(mid);
		}
		if (!mat) {
			mid = 0;
			mat = MaterialManager::materials->find(mid);
		}
		vec3 ambient = mat->ambient;
		vec3 diffuse = mat->diffuse;
		vec3 specular = mat->specular;
		vec4 texids = mat->texids;

		for (int v = 0; v < 3; v++) {
			vertexBuffer[i * 3 + v] = GetVec4(&vertex, v);
			normalBuffer[i * 3 + v] = Float2Half(GetVec3(&normal, v));
			tangentBuffer[i * 3 + v] = Float2Half(GetVec3(&tangent, v));
		}

		texcoordBuffer[i * 4 + 0] = (texcoord.x);
		texcoordBuffer[i * 4 + 1] = (texcoord.y);
		//texcoordBuffer[i * 4 + 2] = (texids.x);
		texcoordBuffer[i * 4 + 2] = mid;
		texcoordBuffer[i * 4 + 3] = (texids.y);

		texidBuffer[i * 2 + 0] = (texids.z);
		texidBuffer[i * 2 + 1] = (texids.w);

		colorBuffer[i * 3 + 0] = (byte)(ambient.x * 255);
		colorBuffer[i * 3 + 1] = (byte)(diffuse.x * 255);
		colorBuffer[i * 3 + 2] = (byte)(specular.x * 255);
	}

	if(instanceMesh->indices) {
		for(int i=0;i<indexCount;i++) {
			int index=instanceMesh->indices[i];
			indexBuffer[i]=(ushort)index;
		}
	}

	maxCount = cnt;
}

void Instance::setRenderData(InstanceData* data) {
	insData = data;
}

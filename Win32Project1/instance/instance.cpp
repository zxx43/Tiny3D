#include "instance.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"

std::map<Mesh*, int> Instance::instanceTable;

Instance::Instance(InstanceData* data) {
	create(data->insMesh);
	maxInstanceCount = data->maxInsCount;
	insData = NULL;
}

Instance::Instance(Mesh* mesh) {
	create(mesh);
	insData = NULL;
}

void Instance::create(Mesh* mesh) {
	insId = -1, insSingleId = -1, insBillId = -1;
	instanceMesh = mesh;
	vertexCount = 0;
	indexCount = 0;
	vertexBuffer = NULL;
	normalBuffer = NULL;
	tangentBuffer = NULL;
	texcoordBuffer = NULL;
	texidBuffer = NULL;
	colorBuffer = NULL;
	indexBuffer = NULL;

	maxInstanceCount = 0;
	isBillboard = instanceMesh->isBillboard;
}

Instance::~Instance() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (tangentBuffer) free(tangentBuffer); tangentBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (texidBuffer) free(texidBuffer); texidBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;
}

void Instance::releaseInstanceData() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (tangentBuffer) free(tangentBuffer); tangentBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (texidBuffer) free(texidBuffer); texidBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;
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

	int mid = object->material;
	if (isBillboard) mid = object->billboard->material;
	for(int i=0;i<vertexCount;i++) {
		vec4 vertex=instanceMesh->vertices[i];
		vec3 normal=instanceMesh->normals[i];
		vec3 tangent = instanceMesh->tangents[i];
		vec2 texcoord=instanceMesh->texcoords[i];

		Material* mat = NULL;
		if (!instanceMesh->materialids && mid >= 0)
			mat = MaterialManager::materials->find(mid);
		else if (instanceMesh->materialids)
			mat = MaterialManager::materials->find(instanceMesh->materialids[i]);
		if (!mat) mat = MaterialManager::materials->find(0);
		vec3 ambient = mat->ambient;
		vec3 diffuse = mat->diffuse;
		vec3 specular = mat->specular;
		vec4 texids = mat->texids;

		for (int v = 0; v < 3; v++) {
			vertexBuffer[i * 3 + v] = GetVec4(&vertex, v);
			normalBuffer[i * 3 + v] = Float2Half(GetVec3(&normal, v));
			tangentBuffer[i * 3 + v] = Float2Half(GetVec3(&tangent, v));
		}

		if (texcoord.x < 0) 
			texcoord.x = 1 + texcoord.x - (int)texcoord.x;
		else if (texcoord.x > 1)
			texcoord.x = texcoord.x - (int)texcoord.x;
		if (texcoord.y < 0)
			texcoord.y = 1 + texcoord.y - (int)texcoord.y;
		else if (texcoord.y > 1)
			texcoord.y = texcoord.y - (int)texcoord.y;

		texcoordBuffer[i * 4 + 0] = (texcoord.x);
		texcoordBuffer[i * 4 + 1] = (texcoord.y);
		texcoordBuffer[i * 4 + 2] = (texids.x);
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

	maxInstanceCount = cnt;
}

void Instance::setRenderData(InstanceData* data) {
	insData = data;
}

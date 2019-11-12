#include "instance.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"

std::map<Mesh*, int> Instance::instanceTable;

Instance::Instance(InstanceData* data, bool dyn) {
	create(data->insMesh, dyn, data->state);
	maxInstanceCount = data->maxInsCount;
}

Instance::Instance(Mesh* mesh, bool dyn, InstanceState* state) {
	create(mesh, dyn, state);
}

void Instance::create(Mesh* mesh, bool dyn, InstanceState* state) {
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

	instanceCount = 0, maxInstanceCount = 0;
	drawcall = NULL;
	isBillboard = instanceMesh->isBillboard;
	isDynamic = dyn;
	isSimple = state->simple;
	isGrass = state->grass;

	modelMatrices = NULL;
	billboards = NULL;
	copyData = true;
}

Instance::~Instance() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (tangentBuffer) free(tangentBuffer); tangentBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (texidBuffer) free(texidBuffer); texidBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	if (copyData) {
		if (modelMatrices) free(modelMatrices); modelMatrices = NULL;
		if (billboards) free(billboards); billboards = NULL;
	}
	if (drawcall) delete drawcall;
}

void Instance::releaseInstanceData() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (tangentBuffer) free(tangentBuffer); tangentBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (texidBuffer) free(texidBuffer); texidBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	if (!isDynamic && copyData) {
		if (modelMatrices) free(modelMatrices); modelMatrices = NULL;
		if (billboards) free(billboards); billboards = NULL;
	}
}

void Instance::initInstanceBuffers(Object* object,int vertices,int indices,int cnt,bool copy) {
	vertexCount = vertices;
	vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	normalBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	tangentBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
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
			normalBuffer[i * 3 + v] = GetVec3(&normal, v);
			tangentBuffer[i * 3 + v] = GetVec3(&tangent, v);
		}

		if (texcoord.x < 0) 
			texcoord.x = 1 + texcoord.x - (int)texcoord.x;
		else if (texcoord.x > 1)
			texcoord.x = texcoord.x - (int)texcoord.x;
		if (texcoord.y < 0)
			texcoord.y = 1 + texcoord.y - (int)texcoord.y;
		else if (texcoord.y > 1)
			texcoord.y = texcoord.y - (int)texcoord.y;

		texcoordBuffer[i * 4 + 0] = texcoord.x;
		texcoordBuffer[i * 4 + 1] = texcoord.y;
		texcoordBuffer[i * 4 + 2] = texids.x;
		texcoordBuffer[i * 4 + 3] = texids.y;

		texidBuffer[i * 2 + 0] = texids.z;
		texidBuffer[i * 2 + 1] = texids.w;

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

	copyData = copy;
	if (copyData) {
		if (isBillboard)
			initBillboards(cnt);
		else
			initMatrices(cnt);
	}

	maxInstanceCount = cnt;
}


void Instance::initMatrices(int cnt) {
	if (!isSimple) {
		modelMatrices = (float*)malloc(cnt * 12 * sizeof(float));
		memset(modelMatrices, 0, cnt * 12 * sizeof(float));
	} else {
		modelMatrices = (float*)malloc(cnt * 4 * sizeof(float));
		memset(modelMatrices, 0, cnt * 4 * sizeof(float));
	}
}

void Instance::initBillboards(int cnt) {
	billboards = (float*)malloc(cnt * 6 * sizeof(float));
	memset(billboards, 0, cnt * 6 * sizeof(float));
}

void Instance::setRenderData(InstanceData* data) {
	instanceCount = data->count;
	if (drawcall) drawcall->objectToPrepare = instanceCount;

	if (copyData) {
		if (isSimple && data->matrices)
			memcpy(modelMatrices, data->matrices, instanceCount * 4 * sizeof(float));
		else if (!isSimple && data->matrices)
			memcpy(modelMatrices, data->matrices, instanceCount * 12 * sizeof(float));
		else 
			memcpy(billboards, data->billboards, instanceCount * 6 * sizeof(float));
	} else {
		if (data->matrices) {
			if (modelMatrices != data->matrices) modelMatrices = data->matrices;
		} else {
			if (billboards != data->billboards) billboards = data->billboards;
		}
	}
}

void Instance::createDrawcall() {
	drawcall = new InstanceDrawcall(this);
}

void Instance::addObject(Object* object, int index) {
	instanceCount++;
	if (!billboards) {
		if (isSimple) memcpy(modelMatrices + (index * 4), object->transforms, 4 * sizeof(float));
		else {
			memcpy(modelMatrices + (index * 12), object->transforms, 4 * sizeof(float));
			memcpy(modelMatrices + (index * 12) + 4, object->rotateQuat, 4 * sizeof(float));

			AABB* bb = (AABB*)object->bounding;
			float boundInfo[4] = { bb->sizex, bb->sizey, bb->sizez, bb->position.y };
			memcpy(modelMatrices + (index * 12) + 8, boundInfo, 4 * sizeof(float));
		}
	} else {
		Material* mat = NULL;
		if (MaterialManager::materials)
			mat = MaterialManager::materials->find(object->billboard->material);

		billboards[index * 6 + 0] = object->billboard->data[0];
		billboards[index * 6 + 1] = object->billboard->data[1];
		billboards[index * 6 + 2] = mat ? mat->texids.x : 0.0;
		memcpy(billboards + (index * 6) + 3, object->transformMatrix.entries + 12, 3 * sizeof(float));
	}
}

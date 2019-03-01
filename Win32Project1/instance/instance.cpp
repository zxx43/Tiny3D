#include "instance.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"

std::map<Mesh*, int> Instance::instanceTable;

Instance::Instance(InstanceData* data, bool dyn) {
	create(data->insMesh, dyn, data->state);
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
	texcoordBuffer = NULL;
	colorBuffer = NULL;
	indexBuffer = NULL;

	instanceCount = 0;
	drawcall = NULL;
	isBillboard = instanceMesh->isBillboard;
	isDynamic = dyn;
	singleSide = state->singleSide;
	isSimple = state->simple;
	isGrass = state->grass;

	modelMatrices = NULL;
	positions = NULL;
	billboards = NULL;
	copyData = true;
}

Instance::~Instance() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	if (copyData) {
		if (modelMatrices) free(modelMatrices); modelMatrices = NULL;
		if (positions) free(positions); positions = NULL;
		if (billboards) free(billboards); billboards = NULL;
	}
	if (drawcall) delete drawcall;
}

void Instance::releaseInstanceData() {
	if (vertexBuffer) free(vertexBuffer); vertexBuffer = NULL;
	if (normalBuffer) free(normalBuffer); normalBuffer = NULL;
	if (texcoordBuffer) free(texcoordBuffer); texcoordBuffer = NULL;
	if (colorBuffer) free(colorBuffer); colorBuffer = NULL;
	if (indexBuffer) free(indexBuffer); indexBuffer = NULL;

	if (!isDynamic && copyData) {
		if (modelMatrices) free(modelMatrices); modelMatrices = NULL;
		if (positions) free(positions); positions = NULL;
		if (billboards) free(billboards); billboards = NULL;
	}
}

void Instance::initInstanceBuffers(Object* object,int vertices,int indices,int cnt,bool copy) {
	vertexCount = vertices;
	vertexBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	normalBuffer = (float*)malloc(vertexCount * 3 * sizeof(float));
	texcoordBuffer = (float*)malloc(vertexCount * 4 * sizeof(float));
	colorBuffer = (byte*)malloc(vertexCount * 3 * sizeof(byte));

	indexCount=indices;
	if (indexCount > 0)
		indexBuffer = (ushort*)malloc(indexCount*sizeof(ushort));

	int mid = object->material;
	if (isBillboard) mid = object->billboard->material;
	for(int i=0;i<vertexCount;i++) {
		VECTOR4D vertex=instanceMesh->vertices[i];
		VECTOR3D normal=instanceMesh->normals[i];
		VECTOR2D texcoord=instanceMesh->texcoords[i];

		Material* mat = NULL;
		if (!instanceMesh->materialids && mid >= 0)
			mat = MaterialManager::materials->find(mid);
		else if (instanceMesh->materialids)
			mat = MaterialManager::materials->find(instanceMesh->materialids[i]);
		if (!mat) mat = MaterialManager::materials->find(0);
		VECTOR3D ambient = mat->ambient;
		VECTOR3D diffuse = mat->diffuse;
		VECTOR3D specular = mat->specular;
		VECTOR4D texOfs = mat->texOfs1;
		float texWidth = mat->texSize.x;
		float texHeight = mat->texSize.y;
		float pixWidth = mat->texSize.z;
		float pixHeight = mat->texSize.w;

		for (int v = 0; v < 3; v++) {
			vertexBuffer[i * 3 + v] = GetVec4(&vertex, v);
			normalBuffer[i * 3 + v] = GetVec3(&normal, v);
		}

		if (texcoord.x < 0) 
			texcoord.x = 1 + texcoord.x - (int)texcoord.x;
		else if (texcoord.x > 1)
			texcoord.x = texcoord.x - (int)texcoord.x;
		if (texcoord.y < 0)
			texcoord.y = 1 + texcoord.y - (int)texcoord.y;
		else if (texcoord.y > 1)
			texcoord.y = texcoord.y - (int)texcoord.y;

		texcoordBuffer[i * 4] = texcoord.x;
		texcoordBuffer[i * 4 + 1] = texcoord.y;
		texcoordBuffer[i * 4 + 2] = (texcoord.x * texWidth + texOfs.x) * pixWidth;
		texcoordBuffer[i * 4 + 3] = (texcoord.y * texHeight + texOfs.y) * pixHeight;

		colorBuffer[i * 3] = (byte)(ambient.x * 255);
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
	positions = (float*)malloc(cnt * 3 * sizeof(float));
	memset(positions, 0, cnt * 3 * sizeof(float));
	billboards = (float*)malloc(cnt * 4 * sizeof(float));
	memset(billboards, 0, cnt * 4 * sizeof(float));
}

void Instance::setRenderData(InstanceData* data) {
	instanceCount = data->count;
	if (drawcall) drawcall->objectToPrepare = instanceCount;

	if (copyData) {
		if (isSimple && data->matrices)
			memcpy(modelMatrices, data->matrices, instanceCount * 4 * sizeof(float));
		else if (!isSimple && data->matrices)
			memcpy(modelMatrices, data->matrices, instanceCount * 12 * sizeof(float));
		else {
			memcpy(billboards, data->billboards, instanceCount * 4 * sizeof(float));
			memcpy(positions, data->positions, instanceCount * 3 * sizeof(float));
		}
	} else {
		if (data->matrices)
			modelMatrices = data->matrices;
		else {
			billboards = data->billboards;
			positions = data->positions;
		}
	}
}

void Instance::createDrawcall() {
	drawcall = new InstanceDrawcall(this);
	drawcall->setSide(singleSide);
}

void Instance::addObject(Object* object, int index) {
	instanceCount++;

	if (isSimple && !billboards) 
		memcpy(modelMatrices + (index * 4), object->transforms, 4 * sizeof(float));
	else if (!isSimple && !billboards)
		memcpy(modelMatrices + (index * 12), object->transformTransposed.entries, 12 * sizeof(float));
	else if (billboards) {
		Material* mat = NULL;
		if (MaterialManager::materials)
			mat = MaterialManager::materials->find(object->billboard->material);

		billboards[index * 4 + 0] = object->billboard->data[0];
		billboards[index * 4 + 1] = object->billboard->data[1];
		if (mat) {
			billboards[index * 4 + 2] = mat->texOfs1.x;
			billboards[index * 4 + 3] = mat->texOfs1.y;
		}

		memcpy(positions + (index * 3), object->transformMatrix.entries + 12, 3 * sizeof(float));
	}
}

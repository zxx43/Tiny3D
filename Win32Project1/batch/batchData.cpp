#include "batch.h"

BatchData::BatchData() {
	vertices = (float*)malloc(MAX_VERTEX_COUNT * 3 * sizeof(float));
	normals = (float*)malloc(MAX_VERTEX_COUNT * 3 * sizeof(float));
	tangents = (float*)malloc(MAX_VERTEX_COUNT * 3 * sizeof(float));
	texcoords = (float*)malloc(MAX_VERTEX_COUNT * 4 * sizeof(float));
	texids = (float*)malloc(MAX_VERTEX_COUNT * 4 * sizeof(float));
	colors = (byte*)malloc(MAX_VERTEX_COUNT * 3 * sizeof(byte));
	objectids = (byte*)malloc(MAX_VERTEX_COUNT * sizeof(byte));
	indices = (uint*)malloc(MAX_INDEX_COUNT * sizeof(uint));
	matrices = (float*)malloc(MAX_OBJECT_COUNT * 12 * sizeof(float));

	vertexCount = 0;
	indexCount = 0;
	objectCount = 0;
	batch = NULL;
}

BatchData::~BatchData() {
	free(vertices);
	free(normals);
	free(tangents);
	free(texcoords);
	free(texids);
	free(colors);
	free(objectids);
	free(indices);
	free(matrices);
	if (batch) delete batch;
}

void BatchData::resetBatch() {
	vertexCount = 0;
	indexCount = 0;
	objectCount = 0;
}

void BatchData::addObject(Object* object, Mesh* mesh) {
	int baseVertex = vertexCount;
	int currentObject = objectCount++;

	Material* mat = NULL;
	if (object->material >= 0)
		mat = MaterialManager::materials->find(object->material);

	for (int i = 0; i < mesh->vertexCount; i++) {
		if (!mat && mesh->materialids)
			mat = MaterialManager::materials->find(mesh->materialids[i]);
		if (!mat) 
			mat = MaterialManager::materials->find(0);

		vec3 vertex3 = mesh->vertices3[i];
		vertices[vertexCount * 3 + 0] = vertex3.x;
		vertices[vertexCount * 3 + 1] = vertex3.y;
		vertices[vertexCount * 3 + 2] = vertex3.z;

		normals[vertexCount * 3 + 0] = mesh->normals4[i].x;
		normals[vertexCount * 3 + 1] = mesh->normals4[i].y;
		normals[vertexCount * 3 + 2] = mesh->normals4[i].z;

		tangents[vertexCount * 3 + 0] = mesh->tangents[i].x;
		tangents[vertexCount * 3 + 1] = mesh->tangents[i].y;
		tangents[vertexCount * 3 + 2] = mesh->tangents[i].z;

		texcoords[vertexCount * 4 + 0] = mesh->texcoords[i].x;
		texcoords[vertexCount * 4 + 1] = mesh->texcoords[i].y;
		texcoords[vertexCount * 4 + 2] = mat->exTexids.x;
		texcoords[vertexCount * 4 + 3] = mat->exTexids.y;

		texids[vertexCount * 4 + 0] = mat->texids.x;
		texids[vertexCount * 4 + 1] = mat->texids.y;
		texids[vertexCount * 4 + 2] = mat->texids.z;
		texids[vertexCount * 4 + 3] = mat->texids.w;

		colors[vertexCount * 3 + 0] = (byte)(mat->ambient.x * 255);
		colors[vertexCount * 3 + 1] = (byte)(mat->diffuse.x * 255);
		colors[vertexCount * 3 + 2] = (byte)(mat->specular.x * 255);

		objectids[vertexCount++] = currentObject;
	}

	for (int i = 0; i < mesh->indexCount; i++)
		indices[indexCount++] = (uint)(baseVertex + mesh->indices[i]);

	memcpy(matrices + (currentObject * 12), object->transformTransposed.entries, 12 * sizeof(float));
}
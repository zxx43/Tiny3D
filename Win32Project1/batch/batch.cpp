#include "batch.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"

Batch::Batch() {
	vertexCount=0;
	indexCount=0;
	storeVertexCount=0;
	storeIndexCount=0;
	vertexBuffer=NULL;
	normalBuffer=NULL;
	texcoordBuffer=NULL;
	textureIdBuffer = NULL;
	colorBuffer = NULL;
	modelMatrices=NULL;
	normalMatrices=NULL;
	indexBuffer=NULL;
}

Batch::~Batch() {
	if(vertexBuffer) delete[] vertexBuffer;
	if(normalBuffer) delete[] normalBuffer;
	if(texcoordBuffer) delete[] texcoordBuffer;
	if (textureIdBuffer) delete[] textureIdBuffer;
	if (colorBuffer) delete[] colorBuffer;
	if(modelMatrices) delete[] modelMatrices;
	if(normalMatrices) delete[] normalMatrices;
	if(indexBuffer) delete[] indexBuffer;
	vertexBuffer=NULL;
	normalBuffer=NULL;
	texcoordBuffer=NULL;
	textureIdBuffer = NULL;
	colorBuffer = NULL;
	modelMatrices=NULL;
	normalMatrices=NULL;
	indexBuffer=NULL;
}

void Batch::initBatchBuffers(int vertices,int indices) {
	vertexCount=vertices;
	vertexBuffer=new float[vertexCount*3];
	normalBuffer=new float[vertexCount*3];
	texcoordBuffer=new float[vertexCount*2];
	textureIdBuffer = new short[vertexCount*4];
	colorBuffer = new byte[vertexCount * 3];
	modelMatrices=new float[vertexCount*12];
	normalMatrices=new float[vertexCount*9];
	storeVertexCount=0;

	indexCount=indices;
	if(indexCount>0)
		indexBuffer=new uint[indexCount];
	storeIndexCount=0;
}

void Batch::pushMeshToBuffers(Mesh* mesh,int mid,const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix) {
	int baseVertex=storeVertexCount;
	for(int i=0;i<mesh->vertexCount;i++) {
		VECTOR4D vertex=mesh->vertices[i];
		VECTOR4D normal(mesh->normals[i].x,mesh->normals[i].y,mesh->normals[i].z,0);
		VECTOR2D texcoord=mesh->texcoords[i];
		
		Material* mat = NULL;
		if (!mesh->materialids && mid >= 0)
			mat = materials->find(mid);
		else if (mesh->materialids)
			mat = materials->find(mesh->materialids[i]);
		if (!mat) mat = materials->find(0);
		VECTOR3D ambient = mat->ambient;
		VECTOR3D diffuse = mat->diffuse;
		VECTOR3D specular = mat->specular;
		VECTOR4D textures = mat->texture;

		vertexBuffer[storeVertexCount*3]=vertex.x/vertex.w;
		vertexBuffer[storeVertexCount*3+1]=vertex.y/vertex.w;
		vertexBuffer[storeVertexCount*3+2]=vertex.z/vertex.w;

		normalBuffer[storeVertexCount*3]=normal.x;
		normalBuffer[storeVertexCount*3+1]=normal.y;
		normalBuffer[storeVertexCount*3+2]=normal.z;

		texcoordBuffer[storeVertexCount*2]=texcoord.x;
		texcoordBuffer[storeVertexCount*2+1]=texcoord.y;

		colorBuffer[storeVertexCount * 3] = (byte)(ambient.x * 255);
		colorBuffer[storeVertexCount * 3 + 1] = (byte)(diffuse.x * 255);
		colorBuffer[storeVertexCount * 3 + 2] = (byte)(specular.x * 255);

		textureChannel = textures.y>=0?4:1;
		textureIdBuffer[storeVertexCount * textureChannel] = (short)textures.x;
		if(textureChannel==4) {
			textureIdBuffer[storeVertexCount * 4 + 1] = (short)textures.y;
			textureIdBuffer[storeVertexCount * 4 + 2] = (short)textures.z;
			textureIdBuffer[storeVertexCount * 4 + 3] = (short)textures.w;
		}

		initMatrix(transformMatrix,normalMatrix);

		storeVertexCount++;
	}

	if(mesh->indices) {
		for(int i=0;i<mesh->indexCount;i++) {
			int index=baseVertex+mesh->indices[i];
			indexBuffer[storeIndexCount]=(uint)index;
			storeIndexCount++;
		}
	}
}

void Batch::updateMatricesBuffer(int baseVertex, Mesh* mesh, const MATRIX4X4& transformMatrix, const MATRIX4X4* normalMatrix) {
	MATRIX4X4 transform = transformMatrix;
	transform.Transpose();
	for (int v = baseVertex; v < mesh->vertexCount + baseVertex; v++) {
		for (int m = 0; m < 12; m++)
			modelMatrices[v * 12 + m] = transform.entries[m];
		if (normalMatrix) {
			for (int n = 0; n < 3; n++)
				normalMatrices[v * 9 + n] = normalMatrix->entries[n];
			for (int n = 3; n < 6; n++)
				normalMatrices[v * 9 + n] = normalMatrix->entries[n + 1];
			for (int n = 6; n < 9; n++)
				normalMatrices[v * 9 + n] = normalMatrix->entries[n + 2];
		}
	}
}

void Batch::initMatrix(const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix) {
	MATRIX4X4 transform = transformMatrix;
	transform.Transpose();
	for (int m = 0; m < 12; m++)
		modelMatrices[storeVertexCount * 12 + m] = transform.entries[m];
	for (int n = 0; n < 3; n++)
		normalMatrices[storeVertexCount * 9 + n] = normalMatrix.entries[n];
	for (int n = 3; n < 6; n++)
		normalMatrices[storeVertexCount * 9 + n] = normalMatrix.entries[n + 1];
	for (int n = 6; n < 9; n++)
		normalMatrices[storeVertexCount * 9 + n] = normalMatrix.entries[n + 2];
}


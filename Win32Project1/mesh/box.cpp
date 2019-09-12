#include "box.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include <stdlib.h>
#include <string.h>

Box::Box():Mesh() {
	vertexCount=24;
	vertices=new vec4[vertexCount];
	normals=new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords=new vec2[vertexCount];
	indexCount = 36;
	indices = (int*)malloc(indexCount*sizeof(int));
	materialids = NULL;
	initFaces();
	caculateExData();
}

Box::Box(const Box& rhs) {
	vertexCount=rhs.vertexCount;
	vertices=new vec4[vertexCount];
	normals=new vec3[vertexCount];
	tangents = new vec3[vertexCount];
	texcoords=new vec2[vertexCount];
	if (rhs.materialids)
		materialids = new int[vertexCount];
	for(int i=0;i<vertexCount;i++) {
		vertices[i]=rhs.vertices[i];
		normals[i]=rhs.normals[i];
		tangents[i] = rhs.tangents[i];
		texcoords[i]=rhs.texcoords[i];
		if (rhs.materialids)
			materialids[i] = rhs.materialids[i];
	}
	indexCount = rhs.indexCount;
	indices = (int*)malloc(indexCount*sizeof(int));
	memcpy(indices, rhs.indices, indexCount*sizeof(int));
	caculateExData();
}

Box::~Box() {
}

void Box::initFaces() {
	vertices[0]= vec4(0.5,-0.5,-0.5,1);
	normals[0] = vec3(0, 0, -1);
	texcoords[0] = vec2(1, 1);

	vertices[1]= vec4(-0.5,-0.5,-0.5,1);
	normals[1]= vec3(0,0,-1);
	texcoords[1]= vec2(0,1);

	vertices[2]= vec4(-0.5,0.5,-0.5,1);
	normals[2]= vec3(0,0,-1);
	texcoords[2]= vec2(0,0);

	vertices[3]= vec4(0.5,0.5,-0.5,1);
	normals[3]= vec3(0,0,-1);
	texcoords[3]= vec2(1,0);

	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 2; indices[4] = 3; indices[5] = 0;

	vec3 tg0 = CaculateTangent(vertices[0], vertices[1], vertices[2], texcoords[0], texcoords[1], texcoords[2]);
	vec3 tg1 = CaculateTangent(vertices[2], vertices[3], vertices[0], texcoords[2], texcoords[3], texcoords[0]);
	tangents[0] = tg0;
	tangents[1] = tg0;
	tangents[2] = tg1;
	tangents[3] = tg1;

	vertices[4]= vec4(-0.5,0.5,0.5,1);
	normals[4]= vec3(0,0,1);
	texcoords[4]= vec2(0,0);

	vertices[5]= vec4(-0.5,-0.5,0.5,1);
	normals[5]= vec3(0,0,1);
	texcoords[5]= vec2(0,1);

	vertices[6]= vec4(0.5,-0.5,0.5,1);
	normals[6]= vec3(0,0,1);
	texcoords[6]= vec2(1,1);

	vertices[7]= vec4(0.5,0.5,0.5,1);
	normals[7]= vec3(0,0,1);
	texcoords[7]= vec2(1,0);

	indices[6] = 4; indices[7] = 5; indices[8] = 6;
	indices[9] = 6; indices[10] = 7; indices[11] = 4;

	vec3 tg2 = CaculateTangent(vertices[4], vertices[5], vertices[6], texcoords[4], texcoords[5], texcoords[6]);
	vec3 tg3 = CaculateTangent(vertices[6], vertices[7], vertices[4], texcoords[6], texcoords[7], texcoords[4]);
	tangents[4] = tg2;
	tangents[5] = tg2;
	tangents[6] = tg3;
	tangents[7] = tg3;

	vertices[8]= vec4(0.5,0.5,0.5,1);
	normals[8]= vec3(1,0,0);
	texcoords[8]= vec2(1,0);

	vertices[9]= vec4(0.5,-0.5,0.5,1);
	normals[9]= vec3(1,0,0);
	texcoords[9]= vec2(1,1);

	vertices[10]= vec4(0.5,-0.5,-0.5,1);
	normals[10]= vec3(1,0,0);
	texcoords[10]= vec2(0,1);

	vertices[11]= vec4(0.5,0.5,-0.5,1);
	normals[11]= vec3(1,0,0);
	texcoords[11]= vec2(0,0);

	indices[12] = 8; indices[13] = 9; indices[14] = 10;
	indices[15] = 10; indices[16] = 11; indices[17] = 8;

	vec3 tg4 = CaculateTangent(vertices[8], vertices[9], vertices[10], texcoords[8], texcoords[9], texcoords[10]);
	vec3 tg5 = CaculateTangent(vertices[10], vertices[11], vertices[8], texcoords[10], texcoords[11], texcoords[8]);
	tangents[8] = tg4;
	tangents[9] = tg4;
	tangents[10] = tg5;
	tangents[11] = tg5;

	vertices[12]= vec4(-0.5,-0.5,0.5,1);
	normals[12]= vec3(-1,0,0);
	texcoords[12]= vec2(0,1);

	vertices[13]= vec4(-0.5,0.5,0.5,1);
	normals[13]= vec3(-1,0,0);
	texcoords[13]= vec2(0,0);

	vertices[14]= vec4(-0.5,0.5,-0.5,1);
	normals[14]= vec3(-1,0,0);
	texcoords[14]= vec2(1,0);

	vertices[15]= vec4(-0.5,-0.5,-0.5,1);
	normals[15]= vec3(-1,0,0);
	texcoords[15]= vec2(1,1);

	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 14; indices[22] = 15; indices[23] = 12;

	vec3 tg6 = CaculateTangent(vertices[12], vertices[13], vertices[14], texcoords[12], texcoords[13], texcoords[14]);
	vec3 tg7 = CaculateTangent(vertices[14], vertices[15], vertices[12], texcoords[14], texcoords[15], texcoords[12]);
	tangents[12] = tg6;
	tangents[13] = tg6;
	tangents[14] = tg7;
	tangents[15] = tg7;

	vertices[16]=vec4(-0.5,0.5,-0.5,1);
	normals[16]=vec3(0,1,0);
	texcoords[16]=vec2(0,1);

	vertices[17]=vec4(-0.5,0.5,0.5,1);
	normals[17]=vec3(0,1,0);
	texcoords[17]=vec2(0,0);

	vertices[18]=vec4(0.5,0.5,0.5,1);
	normals[18]=vec3(0,1,0);
	texcoords[18]=vec2(1,0);

	vertices[19]=vec4(0.5,0.5,-0.5,1);
	normals[19]=vec3(0,1,0);
	texcoords[19]=vec2(1,1);

	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 18; indices[28] = 19; indices[29] = 16;

	vec3 tg8 = CaculateTangent(vertices[16], vertices[17], vertices[18], texcoords[16], texcoords[17], texcoords[18]);
	vec3 tg9 = CaculateTangent(vertices[18], vertices[19], vertices[16], texcoords[18], texcoords[19], texcoords[16]);
	tangents[16] = tg8;
	tangents[17] = tg8;
	tangents[18] = tg9;
	tangents[19] = tg9;

	vertices[20]=vec4(0.5,-0.5,0.5,1);
	normals[20]=vec3(0,-1,0);
	texcoords[20]=vec2(1,0);

	vertices[21]=vec4(-0.5,-0.5,0.5,1);
	normals[21]=vec3(0,-1,0);
	texcoords[21]=vec2(0,0);

	vertices[22]=vec4(-0.5,-0.5,-0.5,1);
	normals[22]=vec3(0,-1,0);
	texcoords[22]=vec2(0,1);

	vertices[23]=vec4(0.5,-0.5,-0.5,1);
	normals[23]=vec3(0,-1,0);
	texcoords[23]=vec2(1,1);

	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 22; indices[34] = 23; indices[35] = 20;

	vec3 tg10 = CaculateTangent(vertices[20], vertices[21], vertices[22], texcoords[20], texcoords[21], texcoords[22]);
	vec3 tg11 = CaculateTangent(vertices[22], vertices[23], vertices[20], texcoords[22], texcoords[23], texcoords[20]);
	tangents[20] = tg10;
	tangents[21] = tg10;
	tangents[22] = tg11;
	tangents[23] = tg11;
}


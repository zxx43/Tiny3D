#include "meshGroupData.h"
#include <stdlib.h>
#include <string.h>

MeshGroupData::MeshGroupData() {
	buffer = NULL;
	count = 0;
	channel = 8;
}

MeshGroupData::~MeshGroupData() {
	releaseBuffer();
}

void MeshGroupData::releaseBuffer() {
	if (buffer) free(buffer);
	buffer = NULL;
}

void MeshGroupData::append(const MeshGroup* meshGroup) {
	int newCount = count + 1;

	int* bufferTmp = (int*)malloc(newCount * channel * sizeof(int));
	if (count > 0)
		memcpy(bufferTmp, buffer, count * channel * sizeof(int));
	uint base = count * channel;
	bufferTmp[base + 0] = meshGroup->subNormals[0];
	bufferTmp[base + 1] = meshGroup->subNormals[1];
	bufferTmp[base + 2] = meshGroup->subNormals[2];
	bufferTmp[base + 3] = meshGroup->subNormals[3];
	bufferTmp[base + 4] = meshGroup->subSingles[0];
	bufferTmp[base + 5] = meshGroup->subSingles[1];
	bufferTmp[base + 6] = meshGroup->subSingles[2];
	bufferTmp[base + 7] = meshGroup->subSingles[3];
	// todo

	releaseBuffer();
	count = newCount;
	buffer = bufferTmp;
}
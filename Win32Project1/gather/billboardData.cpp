#include "billboardData.h"
#include <stdlib.h>
#include <string.h>

BillboardData::BillboardData() {
	buffer = NULL;
	count = 0;
	channel = 4;
}

BillboardData::~BillboardData() {
	releaseBuffer();
}

void BillboardData::releaseBuffer() {
	if (buffer) free(buffer);
	buffer = NULL;
}

void BillboardData::append(const Billboard* board) {
	int newCount = count + 1;

	float* bufferTmp = (float*)malloc(newCount * channel * sizeof(float));
	if (count > 0)
		memcpy(bufferTmp, buffer, count * channel * sizeof(float));
	memcpy(bufferTmp + count * channel, board->data, 3 * sizeof(float));
	bufferTmp[count * channel + 3] = board->material;

	releaseBuffer();
	count = newCount;
	buffer = bufferTmp;
}
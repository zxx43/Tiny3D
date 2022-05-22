#include "indirectData.h"
#include <stdlib.h>
#include <string.h>

IndirectData::IndirectData() {
	buffer = NULL;
	count = 0;
}

IndirectData::~IndirectData() {
	releaseBuffer();
}

void IndirectData::releaseBuffer() {
	if (buffer) free(buffer);
	buffer = NULL;
}

void IndirectData::append(const Indirect* indirect) {
	int newCount = count + 1;
	
	Indirect* bufferTmp = (Indirect*)malloc(newCount * sizeof(Indirect));
	if (count > 0) 
		memcpy(bufferTmp, buffer, count * sizeof(Indirect));
	memcpy(bufferTmp + count, indirect, sizeof(Indirect));

	releaseBuffer();
	count = newCount;
	buffer = bufferTmp;
}
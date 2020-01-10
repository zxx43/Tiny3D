#include "drawcall.h"
#include "../constants/constants.h"
#include <stdlib.h>

Drawcall::Drawcall() {
	uModelMatrix = NULL;
	uNormalMatrix = NULL; 
	setType(NULL_DC);
	setFullStatic(false);
	objectCount = 0;
	dataBuffer = NULL;
	frame = 0;
}

Drawcall::~Drawcall() {
	if (dataBuffer) delete dataBuffer;
	dataBuffer = NULL;
	if (uNormalMatrix) free(uNormalMatrix);
	uNormalMatrix = NULL;
}

void Drawcall::setType(int typ) {
	type = typ;
}

int Drawcall::getType() {
	return type;
}

void Drawcall::setFullStatic(bool stat) {
	fullStatic = stat;
}

bool Drawcall::isFullStatic() {
	return fullStatic;
}
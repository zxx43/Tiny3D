#include "drawcall.h"
#include "../constants/constants.h"
#include <stdlib.h>

Drawcall::Drawcall() {
	uModelMatrix = NULL;
	uNormalMatrix = NULL; 
	setSide(false);
	setType(NULL_DC);
	setFullStatic(false);
	objectCount = 0;
	dataBuffers = NULL;
	bufferCount = 3;
	billboardDC = false;
}

Drawcall::~Drawcall() {
	if (uNormalMatrix) free(uNormalMatrix);
	uNormalMatrix = NULL;
}

void Drawcall::setSide(bool single) {
	singleSide = single;
}

bool Drawcall::isSingleSide() {
	return singleSide;
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

void Drawcall::setBillboard(bool billboard) {
	billboardDC = billboard;
}

bool Drawcall::isBillboard() {
	return billboardDC;
}
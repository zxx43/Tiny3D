#include "drawcall.h"
#include "../constants/constants.h"

Drawcall::Drawcall() {
	uModelMatrix = NULL;
	uNormalMatrix = NULL; 
	setSide(false);
}

Drawcall::~Drawcall() {
	if (uModelMatrix) delete[] uModelMatrix;
	uModelMatrix = NULL;
	if (uNormalMatrix) delete[] uNormalMatrix;
	uNormalMatrix = NULL;
}

void Drawcall::setSide(bool single) {
	singleSide = single;
}

bool Drawcall::isSingleSide() {
	return singleSide;
}

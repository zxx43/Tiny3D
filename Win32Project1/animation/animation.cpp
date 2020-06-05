#include "animation.h"

Animation::Animation() {
	vertCount = 0;
	faceCount = 0;
	boneCount = 0;
	aVertices.clear();
	aNormals.clear();
	aTangents.clear();
	aTexcoords.clear();
	aTextures.clear();
	aAmbients.clear();
	aDiffuses.clear();
	aSpeculars.clear();
	aIndices.clear();
	aBoneids.clear();
	aWeights.clear();
	frameIndex.clear();
	inverseYZ = false;
}

Animation::~Animation() {
	aVertices.clear();
	aNormals.clear();
	aTangents.clear();
	aTexcoords.clear();
	aTextures.clear();
	aAmbients.clear();
	aDiffuses.clear();
	aSpeculars.clear();
	aIndices.clear();
	aBoneids.clear();
	aWeights.clear();

	for (int i = 0; i < animCount; i++)
		delete animFrames[i];
	delete[] animFrames;

	frameIndex.clear();
}

std::string Animation::convertTexPath(const std::string& path) {
	int lc = path.find_last_of('/');
	int ld = path.find_last_of('\\');
	lc = lc > ld ? lc : ld;
	if (lc == std::string::npos) lc = 0;
	else lc += 1;
	int ln = path.find_last_of('.');
	std::string res = path.substr(lc, ln - lc);
	res += ".bmp";
	printf("convert %s\n", res.data());
	return res;
}
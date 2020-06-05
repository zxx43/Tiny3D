#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include <map>
#include <string>
#include "../maths/Maths.h"
#include "../material/materialManager.h"
#include <stdlib.h>
#include <string.h>
#include "../constants/constants.h"

struct Frame {
	int boneCount;
	float* data;
	Frame(int bc) {
		boneCount = bc;
		data = (float*)malloc(boneCount * 12 * sizeof(float));
	}
	~Frame() {
		free(data);
	}
};

struct AnimFrame {
	std::vector<Frame*> frames;
	AnimFrame() {
		frames.clear();
	}
	~AnimFrame() {
		for (unsigned int i = 0; i < frames.size(); i++)
			delete frames[i];
		frames.clear();
	}
};

class Animation {
public:
	std::string name;
	std::map<int, int> frameIndex;
	bool inverseYZ;
public:
	int faceCount, vertCount, boneCount;
	std::vector<vec3> aVertices;
	std::vector<vec3> aNormals;
	std::vector<vec3> aTangents;
	std::vector<vec2> aTexcoords;
	std::vector<Material*> aTextures;
	std::vector<vec3> aAmbients;
	std::vector<vec3> aDiffuses;
	std::vector<vec3> aSpeculars;
	std::vector<int> aIndices;
	std::vector<vec4> aBoneids;
	std::vector<vec4> aWeights;
	int animCount;
	AnimFrame** animFrames;
public:
	Animation();
	virtual ~Animation();
public:
	virtual float getBoneFrame(int animIndex, float time, bool& end) = 0;
	std::string getName() { return name; }
	void setName(std::string value) { name = value; }
	void setFrameIndex(int aid, int fid) { frameIndex[aid] = fid; }
	int getFrameIndex(int aid) { return frameIndex[aid]; }
	std::string convertTexPath(const std::string& path);
};

#endif
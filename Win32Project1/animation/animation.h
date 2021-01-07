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
	std::string name;
	float duration, ticksPerSecond;
	std::vector<Frame*> frames;
	AnimFrame(const char* n) {
		name = n;
		duration = 0.0;
		ticksPerSecond = 0.0;
		frames.clear();
	}
	~AnimFrame() {
		for (unsigned int i = 0; i < frames.size(); i++)
			delete frames[i];
		frames.clear();
	}
	std::string getName() {
		return name;
	}
	void setDuration(float d) {
		duration = d;
	}
	void setTicksPerSecond(float t) {
		ticksPerSecond = t;
	}
};

class Animation {
public:
	std::string name;
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
	std::vector<AnimFrame*> datasToExport;
public:
	Animation();
	virtual ~Animation();
public:
	float getBoneFrame(AnimFrame* animation, float time, bool& end);
	std::string getName() { return name; }
	void setName(std::string value) { name = value; }
	std::string convertTexPath(const std::string& path);
	uint getExportSize() { return datasToExport.size(); }
	void exportAnims(std::string path);
private:
	void clearExportData();
};

#endif
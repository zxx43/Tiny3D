#include "frameMgr.h"
#include <iostream>
#include <fstream>
#include <sstream>

FrameMgr::FrameMgr() {
	frames.clear();
	frameIndex.clear();
	datas = NULL;
	animCount = 0;
}

FrameMgr::~FrameMgr() {
	for (uint i = 0; i < frames.size(); ++i)
		delete frames[i];
	frames.clear();
	frameIndex.clear();
	if (datas) free(datas); datas = NULL;
}

int FrameMgr::addFrame(AnimFrame* data) {
	if (data->frames.size() <= 0) return -1;

	int boneCount = data->frames[0]->boneCount, imgWidth = boneCount * 3;
	int frameCount = data->frames.size(), imgHeight = frameCount;
	float* texData = (float*)malloc(imgWidth * imgHeight * 4 * sizeof(float));

	for (int f = 0; f < frameCount; ++f) {
		Frame* frame = data->frames[f];
		memcpy(texData + f * boneCount * 12, frame->data, boneCount * 12 * sizeof(float));
	}

	uint curTex = frames.size();
	frames.push_back(new Texture2D(imgWidth, imgHeight, false, TEXTURE_TYPE_ANIME, FLOAT_PRE, 4, NEAREST, WRAP_REPEAT, false, texData));
	free(texData);
	return curTex;
}

void FrameMgr::addAnimationData(AnimFrame* data, Animation* anim) {
	frameIndex[data->getName()] = addFrame(data);
}

void FrameMgr::readAnimationData(const char* path, AnimFrame* animation) {
	float boneCount, frameCount, duration, ticksPerSecond;

	std::ifstream ifs(path, std::ios::binary);
	std::string line;
	if (getline(ifs, line)) {
		std::istringstream ins(line);
		ins >> boneCount >> frameCount >> duration >> ticksPerSecond;
	}
	float* data = (float*)malloc(frameCount * boneCount * 12 * sizeof(float));
	int cur = 0;
	while (getline(ifs, line)) {
		std::istringstream ins(line);
		float d = 0.0;
		while (ins >> d) 
			data[cur++] = d;
	}
	ifs.close();

	int curOut = 0;
	for (uint f = 0; f < frameCount; ++f) {
		Frame* frame = new Frame(boneCount);
		int curIn = 0;
		for (uint b = 0; b < boneCount; ++b) {
			for (uint inn = 0; inn < 12; ++inn)
				frame->data[curIn++] = data[curOut++];
		}
		animation->frames.push_back(frame);
	}
	free(data);

	animation->setDuration(duration);
	animation->setTicksPerSecond(ticksPerSecond);
}

void FrameMgr::init() {
	animCount = frames.size();
	datas = (u64*)malloc(animCount * sizeof(u64));
	for (uint i = 0; i < animCount; ++i)
		datas[i] = frames[i]->hnd;
}
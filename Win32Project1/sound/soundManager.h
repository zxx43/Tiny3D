#ifndef SOUND_MANAGER_H_
#define SOUND_MANAGER_H_

#include "CWaves.h"
#include "../util/util.h"

class SoundObject {
private:   
	ALenum alFormatBuffer;
	ALsizei alFreqBuffer;  
	ALsizei alBufferLen;
	uint alSource;  
	uint alSampleSet;
	const char* filePath;
	bool isLoop;
public:
	SoundObject(const char* path);
	SoundObject(const SoundObject& rhs);
	~SoundObject();
public:
	void play();
	void stop();
	void setLoop(bool loop);
	void setPosition(const vec3& position);
};

class SoundManager {
private:
	ALCcontext* context;
	ALCdevice* device;
public:
	SoundManager();
	~SoundManager();
private:
	void addListener();
public:
	void setListenerPosition(const vec3& position);
};

#endif
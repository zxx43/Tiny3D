#include "soundManager.h" 
#include "../util/util.h"

SoundObject::SoundObject(const char* path) {
	filePath = path;
	alGenBuffers(1, &alSampleSet);
	alGenSources(1, &alSource);
	CWaves file;
	file.loadWavFile(filePath, &alSource, &alSampleSet, &alBufferLen, &alFreqBuffer, &alFormatBuffer);
	isPlay = false;
	setLoop(false);
	setPosition(vec3(0.0));
}

SoundObject::SoundObject(const SoundObject& rhs) {
	filePath = rhs.filePath;
	alGenBuffers(1, &alSampleSet);
	alGenSources(1, &alSource);
	CWaves file;
	file.loadWavFile(filePath, &alSource, &alSampleSet, &alBufferLen, &alFreqBuffer, &alFormatBuffer);
	isPlay = rhs.isPlay;
	setLoop(rhs.isLoop);
	setPosition(vec3(0, 0, 0));
}

SoundObject::~SoundObject() {
	alDeleteSources(1, &alSource);
	alDeleteBuffers(1, &alSampleSet);
}

void SoundObject::play() {
	ALint state;
	alGetSourcei(alSource, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING) alSourcePlay(alSource);
	isPlay = true;
}

void SoundObject::stop() {
	alSourceStop(alSource);
	isPlay = false;
}

void SoundObject::setLoop(bool loop) {
	isLoop = loop;
	alSourcei(alSource, AL_LOOPING, isLoop ? AL_TRUE : AL_FALSE);
}

void SoundObject::setPosition(const vec3& position) {
	alSource3f(alSource, AL_POSITION, position.x, position.y, position.z);
}

SoundManager::SoundManager() {
	context = NULL;
	device = alcOpenDevice((const ALCchar*)"DirectSound3D");
	if (!device) printf("Device error!\n");
	else {
		context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);
		addListener();
		printf("Sound inited\n");
	}
}

SoundManager::~SoundManager() {
	alcMakeContextCurrent(NULL);
	if (context) alcDestroyContext(context);
	if (device) alcCloseDevice(device);
}

void SoundManager::addListener() {
	float ori[6] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
	alListenerfv(AL_ORIENTATION, ori);
	alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
}

void SoundManager::setListenerPosition(const vec3& position) {
	alListener3f(AL_POSITION, position.x, position.y, position.z);
}
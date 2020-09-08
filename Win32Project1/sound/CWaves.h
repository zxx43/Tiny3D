// Waves.h: interface for the CWaves class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CWAVES_H_
#define _CWAVES_H_

#include <AL/al.h>
#include <AL/alc.h>
#include "../util/util.h"

struct WAVE_Data {
    char subChunkID[4]; //should contain the word data  
    long subChunk2Size; //Stores the size of the data block  
};

struct WAVE_Format {
    char subChunkID[4];
    long subChunkSize;
    short audioFormat;
    short numChannels;
    long sampleRate;
    long byteRate;
    short blockAlign;
    short bitsPerSample;
};

struct RIFF_Header {
    char chunkID[4];
    long chunkSize;//size not including chunkSize or chunkID  
    char format[4];
};

class CWaves {
public:
	CWaves() {}
	~CWaves() {}
public:
    bool loadWavFile(const char* filename, ALuint* source, ALuint* buffer,
        ALsizei* size, ALsizei* frequency,
        ALenum* format);
};

#endif // _CWAVES_H_

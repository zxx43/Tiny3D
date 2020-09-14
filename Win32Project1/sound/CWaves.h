// Waves.h: interface for the CWaves class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CWAVES_H_
#define _CWAVES_H_

#include <AL/al.h>
#include <AL/alc.h>
#include "../util/util.h"

/**************************************************************************************************************
 * WAVE FILE LOADING
**************************************************************************************************************/

// You can just use alutCreateBufferFromFile to load a wave file, but there seems to be a lot of problems with 
// alut not beign available, being deprecated, etc.  So...here's a stupid routine to load a wave file.  I have
// tested this only on x86 machines, so if you find a bug on PPC please let me know.

// Macros to swap endian-values.

#define SWAP_32(value)                 \
        (((((unsigned short)value)<<8) & 0xFF00)   | \
         ((((unsigned short)value)>>8) & 0x00FF))

#define SWAP_16(value)                     \
        (((((unsigned int)value)<<24) & 0xFF000000)  | \
         ((((unsigned int)value)<< 8) & 0x00FF0000)  | \
         ((((unsigned int)value)>> 8) & 0x0000FF00)  | \
         ((((unsigned int)value)>>24) & 0x000000FF))

// Wave files are RIFF files, which are "chunky" - each section has an ID and a length.  This lets us skip
// things we can't understand to find the parts we want.  This header is common to all RIFF chunks.
struct chunk_header {
	int			id;
	int			size;
};

// WAVE file format info.  We pass this through to OpenAL so we can support mono/stereo, 8/16/bit, etc.
struct format_info {
	short		format;				// PCM = 1, not sure what other values are legal.
	short		num_channels;
	int			sample_rate;
	int			byte_rate;
	short		block_align;
	short		bits_per_sample;
};

// This utility returns the start of data for a chunk given a range of bytes it might be within.  Pass 1 for
// swapped if the machine is not the same endian as the file.
inline static char *	find_chunk(char * file_begin, char * file_end, int desired_id, int swapped)
{
	while (file_begin < file_end)
	{
		chunk_header * h = (chunk_header *)file_begin;
		if (h->id == desired_id && !swapped)
			return file_begin + sizeof(chunk_header);
		if (h->id == SWAP_32(desired_id) && swapped)
			return file_begin + sizeof(chunk_header);
		int chunk_size = swapped ? SWAP_32(h->size) : h->size;
		char * next = file_begin + chunk_size + sizeof(chunk_header);
		if (next > file_end || next <= file_begin)
			return NULL;
		file_begin = next;
	}
	return NULL;
}

// Given a chunk, find its end by going back to the header.
inline static char * chunk_end(char * chunk_start, int swapped)
{
	chunk_header * h = (chunk_header *)(chunk_start - sizeof(chunk_header));
	return chunk_start + (swapped ? SWAP_32(h->size) : h->size);
}

#define FAIL(X) { printf("%s\n", X); return 0; }

#define RIFF_ID 0x46464952			// 'RIFF'
#define FMT_ID  0x20746D66			// 'FMT '
#define DATA_ID 0x61746164			// 'DATA'

class CWaves {
public:
	CWaves() {}
	~CWaves() {}
public:
    bool loadWavFile(const char* filename, ALuint* source, ALuint* buffer,
		ALsizei* aSize, ALsizei* aFrequency,
		ALenum* aFormat);
};

#endif // _CWAVES_H_

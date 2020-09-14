#include "CWaves.h"

bool CWaves::loadWavFile(const char* filename, ALuint* source, ALuint* buffer,
        ALsizei* aSize, ALsizei* aFrequency,
        ALenum* aFormat) {
	// First: we open the file and copy it into a single large memory buffer for processing.

	FILE* fi = fopen(filename, "rb");
	if (fi == NULL) return false;

	fseek(fi, 0, SEEK_END);
	int file_size = ftell(fi);
	fseek(fi, 0, SEEK_SET);
	char* mem = (char*)malloc(file_size);
	if (mem == NULL) {
		fclose(fi);
		return false;
	}
	if (fread(mem, 1, file_size, fi) != file_size) {
		free(mem);
		fclose(fi);
		return false;
	}
	fclose(fi);
	char* mem_end = mem + file_size;

	// Second: find the RIFF chunk.  Note that by searching for RIFF both normal
	// and reversed, we can automatically determine the endian swap situation for
	// this file regardless of what machine we are on.

	int swapped = 0;
	char* riff = find_chunk(mem, mem_end, RIFF_ID, 0);
	if (riff == NULL) {
		riff = find_chunk(mem, mem_end, RIFF_ID, 1);
		if (riff)
			swapped = 1;
		else
			printf("Could not find RIFF chunk in wave file.\n");
	}

	// The wave chunk isn't really a chunk at all. :-(  It's just a "WAVE" tag 
	// followed by more chunks.  This strikes me as totally inconsistent, but
	// anyway, confirm the WAVE ID and move on.

	if (riff[0] != 'W' ||
		riff[1] != 'A' ||
		riff[2] != 'V' ||
		riff[3] != 'E')
		printf("Could not find WAVE signature in wave file.\n");

	char* format = find_chunk(riff + 4, chunk_end(riff, swapped), FMT_ID, swapped);
	if (format == NULL)
		printf("Could not find FMT  chunk in wave file.\n");

	// Find the format chunk, and swap the values if needed.  This gives us our real format.

	format_info * fmt = (format_info *)format;
	if (swapped) {
		fmt->format = SWAP_16(fmt->format);
		fmt->num_channels = SWAP_16(fmt->num_channels);
		fmt->sample_rate = SWAP_32(fmt->sample_rate);
		fmt->byte_rate = SWAP_32(fmt->byte_rate);
		fmt->block_align = SWAP_16(fmt->block_align);
		fmt->bits_per_sample = SWAP_16(fmt->bits_per_sample);
	}

	// Reject things we don't understand...expand this code to support weirder audio formats.

	if (fmt->format != 1) printf("Wave file is not PCM format data.\n");
	if (fmt->num_channels != 1 && fmt->num_channels != 2) printf("Must have mono or stereo sound.\n");
	if (fmt->bits_per_sample != 8 && fmt->bits_per_sample != 16) printf("Must have 8 or 16 bit sounds.\n");

	char* data = find_chunk(riff + 4, chunk_end(riff, swapped), DATA_ID, swapped);
	if (data == NULL)
		printf("I could not find the DATA chunk.\n");

	int sample_size = fmt->num_channels * fmt->bits_per_sample / 8;
	int data_bytes = chunk_end(data, swapped) - data;
	int data_samples = data_bytes / sample_size;

	// If the file is swapped and we have 16-bit audio, we need to endian-swap the audio too or we'll 
	// get something that sounds just astoundingly bad!

	if (fmt->bits_per_sample == 16 && swapped)
	{
		short* ptr = (short*)data;
		int words = data_samples * fmt->num_channels;
		while (words--)
		{
			*ptr = SWAP_16(*ptr);
			++ptr;
		}
	}

	*aFormat = fmt->bits_per_sample == 16 ?
		(fmt->num_channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16) :
		(fmt->num_channels == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8);
	*aSize = data_bytes;
	*aFrequency = fmt->sample_rate;

	//now we put our data into the openAL buffer and  
	//check for success  
	alBufferData(*buffer, *aFormat, (void*)data,
		*aSize, *aFrequency);
	alSourcei(*source, AL_BUFFER, *buffer);

	//clean up and return true if successful  
	free(mem);
	return true;
}
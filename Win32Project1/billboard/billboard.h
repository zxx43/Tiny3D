#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include <stdlib.h>
#include <string.h>

struct Billboard {
	int texid;
	float* data;
	Billboard(float sx, float sy, int tex) {
		data = (float*)malloc(2 * sizeof(float));
		data[0] = sx, data[1] = sy;
		texid = tex;
	}
	~Billboard() {
		free(data);
	}
};

#endif
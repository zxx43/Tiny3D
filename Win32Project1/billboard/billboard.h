#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include <stdlib.h>
#include <string.h>

struct Billboard {
	int material;
	float* data;
	Billboard(float sx, float sy, int mid) {
		data = (float*)malloc(2 * sizeof(float));
		data[0] = sx, data[1] = sy;
		material = mid;
	}
	~Billboard() {
		free(data);
	}
};

#endif
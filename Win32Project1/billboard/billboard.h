#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include <stdlib.h>
#include <string.h>

struct Billboard {
	int material;
	float* data;
	Billboard(float sx, float sy, int mid) {
		data = (float*)malloc(3 * sizeof(float));
		data[0] = sx, data[1] = sy, data[2] = -1;
		material = mid;
	}
	Billboard(const Billboard& rhs) {
		data = (float*)malloc(3 * sizeof(float));
		data[0] = rhs.data[0];
		data[1] = rhs.data[1];
		data[2] = rhs.data[2];
		material = rhs.material;
	}
	~Billboard() {
		free(data);
	}
	bool equals(const Billboard* other) {
		bool res = other->material == material &&
			other->data[0] == data[0] &&
			other->data[1] == data[1] &&
			other->data[2] == data[2];
		return res;
	}
};

#endif
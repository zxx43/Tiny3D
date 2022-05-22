#ifndef BILLBOARD_DATA_H_
#define BILLBOARD_DATA_H_

#include "../constants/constants.h"
#include "../util/util.h"
#include "../billboard/billboard.h"

class BillboardData {
public:
	float* buffer;
	int count;
	int channel;
public:
	BillboardData();
	~BillboardData();
public:
	void append(const Billboard* board);
private:
	void releaseBuffer();
};

#endif // !BILLBOARD_DATA_H_


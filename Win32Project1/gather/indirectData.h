#ifndef INDIRECT_DATA_H_
#define INDIRECT_DATA_H_

#include "../constants/constants.h"
#include "../util/util.h"

class IndirectData {
public:
	Indirect* buffer;
	int count;
public:
	IndirectData();
	~IndirectData();
public:
	void append(const Indirect* indirect);
private:
	void releaseBuffer();
};

#endif // !INDIRECT_DATA_H_

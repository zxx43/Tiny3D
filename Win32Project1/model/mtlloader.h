/*
 * mtlloader.h
 *
 *  Created on: 2017-4-28
 *      Author: a
 */

#ifndef MTLLOADER_H_
#define MTLLOADER_H_

#include <map>
#include <string>
#include "../maths/Maths.h"

class MtlLoader {
private:
	const char* mtlFilePath;
	int mtlCount;

	void readMtlInfo();
	void readMtlFile();
public:
	std::map<std::string,int> objMtls;

	MtlLoader(const char* mtlPath);
	~MtlLoader();
};


#endif /* MTLLOADER_H_ */

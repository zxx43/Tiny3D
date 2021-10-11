/*
 * objloader.h
 *
 *  Created on: 2017-4-27
 *      Author: a
 */

#ifndef OBJLOADER_H_
#define OBJLOADER_H_

#include <string>
#include "mtlloader.h"

class ObjLoader {
private:
	std::string objFilePath;
	std::string mtlFilePath;
	int vtNumber;
	int vtCount,vnCount;

	void readObjInfo();
	void readObjFile();

public:
	int vCount,faceCount;
	float** vArr;
	float** vtArr;
	float** vnArr;
	int** fvArr;
	int** fnArr;
	int** ftArr;
	std::string* mtArr;
	MtlLoader* mtlLoader;

	ObjLoader(const char* objPath,const char* mtlPath,int vtNum);
	~ObjLoader();
};


#endif /* OBJLOADER_H_ */

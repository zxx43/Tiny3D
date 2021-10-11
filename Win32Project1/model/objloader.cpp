#include "objloader.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

ObjLoader::ObjLoader(const char* objPath,const char* mtlPath,int vtNum) {
	objFilePath=string(objPath);
	mtlFilePath=string(mtlPath);
	vtNumber=vtNum;
	vCount=0;
	vnCount=0;
	vtCount=0;
	faceCount=0;
	readObjInfo();
	readObjFile();
	mtlLoader=new MtlLoader(mtlFilePath.data());
}

void ObjLoader::readObjInfo() {
	ifstream infile(objFilePath.data());
	string sline;

	while(getline(infile,sline)) {//从指定文件逐行读取
		if(sline[0]=='v') {
			if(sline[1]=='n')
				vnCount++;
			else if(sline[1]=='t')
				vtCount++;
			else
				vCount++;
		}
		if(sline[0]=='f')
			faceCount++;
	}
	infile.close();
}

void ObjLoader::readObjFile() {
	vArr=new float*[vCount];
	for (int i=0;i<vCount;i++)
		vArr[i]=new float[3];

	vnArr=new float*[vnCount];
	for (int i=0;i<vnCount;i++)
		vnArr[i]=new float[3];

	vtArr=new float*[vtCount];
	for (int i=0;i<vtCount;i++)
		vtArr[i]=new float[vtNumber];

	fvArr=new int*[faceCount];
	ftArr=new int*[faceCount];
	fnArr=new int*[faceCount];
	for (int i=0;i<faceCount;i++) {
		fvArr[i]=new int[3];
		ftArr[i]=new int[3];
		fnArr[i]=new int[3];
	}
	mtArr=new string[faceCount];

	ifstream infile(objFilePath.data());
	string sline;
	int ii=0,tt=0,jj=0,kk=0;

	string s1;
	float f2,f3,f4;
	string mtl("");

	while(getline(infile,sline)) {
		if(sline[0]=='v') {
			if(sline[1]=='n') {//vn
				istringstream ins(sline);
				ins>>s1>>f2>>f3>>f4;
				vnArr[ii][0]=f2;
				vnArr[ii][1]=f3;
				vnArr[ii][2]=f4;
				ii++;
			} else if(sline[1]=='t') {//vt
				istringstream ins(sline);
				if(vtNumber==3) {
					ins>>s1>>f2>>f3>>f4;
					vtArr[tt][0]=f2;
					vtArr[tt][1]=f3;
					vtArr[tt][2]=f4;
				} else {
					ins>>s1>>f2>>f3;
					vtArr[tt][0]=f2;
					vtArr[tt][1]=f3;
				}
				tt++;
			} else {//v
				istringstream ins(sline);
				ins>>s1>>f2>>f3>>f4;
				vArr[jj][0]=f2;
				vArr[jj][1]=f3;
				vArr[jj][2]=f4;
				jj++;
			}
		} else if(sline[0]=='f') { //f
			istringstream ins(sline);
			float a;
			ins>>s1;//去掉f
			int i,k;
			for(i=0;i<3;i++) {
				ins>>s1;
				//取出第一个顶点和法线索引
				a=0;
				for(k=0;s1[k]!='/';k++)
					a=a*10+(s1[k]-48);
				fvArr[kk][i]=a;

				a=0;
				for(k=k+1;s1[k]!='/';k++)
					a=a*10+(s1[k]-48);
				ftArr[kk][i]=a;

				a=0;
				for(k=k+1;s1[k];k++)
					a=a*10+(s1[k]-48);
				fnArr[kk][i]=a;
			}
			mtArr[kk]=mtl;
			kk++;
		} else if(sline[0]=='u'&&sline[1]=='s') {//usemtl
			istringstream ins(sline);
			ins>>s1>>mtl;
		}
	}
	infile.close();
}

ObjLoader::~ObjLoader() {
	for(int i=0;i<vCount;i++)
		delete[] vArr[i];
	for(int i=0;i<vnCount;i++)
		delete[] vnArr[i];
	for(int i=0;i<vtCount;i++)
		delete[] vtArr[i];
	for(int i=0;i<faceCount;i++) {
		delete[] fvArr[i];
		delete[] ftArr[i];
		delete[] fnArr[i];
	}
	delete[] vArr;
	delete[] vnArr;
	delete[] vtArr;
	delete[] fvArr;
	delete[] ftArr;
	delete[] fnArr;
	delete[] mtArr;

	delete mtlLoader;
	mtlLoader=NULL;
}


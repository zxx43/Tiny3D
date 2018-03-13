#include "mtlloader.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../assets/assetManager.h"
#include "../material/materialManager.h"
using namespace std;

MtlLoader::MtlLoader(const char* mtlPath) {
	mtlFilePath=mtlPath;
	mtlCount=0;
	readMtlInfo();
	readMtlFile();
}

MtlLoader::~MtlLoader() {
	objMtls.clear();
}

void MtlLoader::readMtlInfo() {
	ifstream infile(mtlFilePath);
	string sline;
	while(getline(infile,sline)) {
		if(sline[0]=='n'&&sline[1]=='e')//newmtl
			mtlCount++;
	}
	infile.close();
}

void MtlLoader::readMtlFile() {
	ifstream infile(mtlFilePath);
	string sline;
	int n=0,t=0,d=0,a=0,s=0;

	string value,name,texture;
	float red=0,green=0,blue=0;
	Material* mtl=NULL;
	while(getline(infile,sline)) {
		if(sline!="") {
			istringstream ins(sline);
			ins>>value;
			if(value=="newmtl") {
				ins>>name;
				mtl = new Material(name.c_str());
				mtl->texture=VECTOR4D(-1,-1,-1,-1);
				objMtls[name] = MaterialManager::materials->add(mtl);
				n++;
			} else if(value=="map_Kd") {
				ins>>texture;
				if (mtl) {
					ImageSet* textures = AssetManager::assetManager->textures;
					if (textures->findTexture(texture.c_str()) < 0)
						textures->addTexture(texture.c_str());
					mtl->texture.x = textures->findTexture(texture.c_str());
				}
				t++;
			} else if(value=="Kd") {
				ins>>red>>green>>blue;
				if(mtl) {
					mtl->diffuse.x=red;
					mtl->diffuse.y=green;
					mtl->diffuse.z=blue;
				}
				d++;
			} else if (value == "Ka") {
				ins >> red >> green >> blue;
				if (mtl) {
					mtl->ambient.x = red;
					mtl->ambient.y = green;
					mtl->ambient.z = blue;
				}
				a++;
			} else if (value == "Ks") {
				ins >> red >> green >> blue;
				if (mtl) {
					mtl->specular.x = red;
					mtl->specular.y = green;
					mtl->specular.z = blue;
				}
				s++;
			}
		}
	}
	infile.close();
}


/*
 * shader.h
 *
 *  Created on: 2017-4-7
 *      Author: a
 */

#ifndef SHADER_H_
#define SHADER_H_

#include "shaderprogram.h"
#include "../constants/constants.h"
#include <map>
#include <string>

#ifndef INVALID_LOCATION 
#define INVALID_LOCATION -1
#endif

class Shader {
private:
	ShaderProgram* program;
	std::map<std::string,GLint> paramLocations;
	std::map<std::string,GLint> attribLocations;
	std::map<u64, bool> bindedTexs;
	std::map<int, std::string> texSlots;
	std::map<int, u64> slotHnds;
	std::string vertName, fragName;
	std::string compName;
public:
	bool isTexBinded(u64 texhnd) { 
		std::map<u64, bool>::iterator it = bindedTexs.find(texhnd);
		if (it == bindedTexs.end()) 
			return false; 
		else 
			return it->second; 
	}
	void rebindTex(u64 texhnd) { bindedTexs[texhnd] = false; }
	void setSlot(const std::string& texName, int slot) { texSlots[slot] = texName; }
	bool hasSlot(int slot) { return texSlots.find(slot) != texSlots.end(); }
	std::string getSlot(int slot) { return texSlots[slot]; }
	i64 getSlotHnd(int slot) { 
		std::map<int, u64>::iterator it = slotHnds.find(slot);  
		if (it == slotHnds.end()) 
			return -1; 
		else 
			return it->second; 
	}
public:
	std::string name;
	Shader(const char* vert, const char* frag, const char* tesc = NULL, const char* tese = NULL, const char* geom = NULL);
	Shader(const char* comp);
	~Shader();
	void attachDef(const char* def, const char* value);
	void attachEx(std::string ex);
	void compose();
	void compile(bool preload);
	void dettach();
	void use();
	void addAttrib(const char* name);
	void addParam(const char* name);
	int findAttribLocation(const char* attrib);
	int findParamLocation(const char* param);
	bool getError(const char* param, int location);
	void setInt(const char* param,int value);
	void setUint(const char* param, uint value);
	void setUintv(const char* param, int count, uint* arr);
	void setSampler(const char* param,int value);
	void setFloat(const char* param,float value);
	void setVector2(const char* param,float x,float y);
	void setVector3(const char* param,float x,float y,float z);
	void setVector4(const char* param,float x,float y,float z,float w);
	void setVector2v(const char* param, float* arr);
	void setVector3v(const char* param, float* arr);
	void setVector4v(const char* param, float* arr);
	void setMatrix4(const char* param,float* matrix);
	void setMatrix4(const char* param,int count,float* matrices);
	void setMatrix3x4(const char* param, int count, float* matrices);
	void setMatrix3(const char* param, float* matrix);
	void setMatrix3(const char* param, int count, float* matrices);
	void setHandle64(const char* param, u64 value);
	void setHandle64v(const char* param, int count, u64* arr);
	void setSlotHnd(int slot, u64 hnd);
};


#endif /* SHADER_H_ */

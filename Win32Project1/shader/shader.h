/*
 * shader.h
 *
 *  Created on: 2017-4-7
 *      Author: a
 */

#ifndef SHADER_H_
#define SHADER_H_

#include "shaderprogram.h"
#include <map>
#include <string>

#ifndef INVALID_LOCATION 
#define INVALID_LOCATION -1
#endif

class Shader {
private:
	ShaderProgram* program;
	std::map<std::string,GLuint> paramLocations;
	std::map<std::string,GLuint> attribLocations;
public:
	Shader(const char* vert,const char* frag);
	~Shader();
	void use();
	void addAttrib(const char* name);
	void addParam(const char* name);
	int findAttribLocation(const char* attrib);
	int findParamLocation(const char* param);
	void setInt(const char* param,int value);
	void setSampler(const char* param,int value);
	void setFloat(const char* param,float value);
	void setVector2(const char* param,float x,float y);
	void setVector3(const char* param,float x,float y,float z);
	void setVector4(const char* param,float x,float y,float z,float w);
	void setMatrix4(const char* param,float* matrix);
	void setMatrix4(const char* param,int count,float* matrices);
	void setMatrix3x4(const char* param, int count, float* matrices);
	void setMatrix3(const char* param, float* matrix);
	void setMatrix3(const char* param, int count, float* matrices);
};


#endif /* SHADER_H_ */

#include "shader.h"
#include "../constants/constants.h"
using namespace std;

Shader::Shader(const char* vert,const char* frag) {
	program=new ShaderProgram(vert,frag);
}

Shader::~Shader() {
	delete program;
	program=NULL;
	paramLocations.clear();
	attribLocations.clear();
}

void Shader::use() {
	program->use();
}

void Shader::addAttrib(const char* name) {
	GLuint location=glGetAttribLocation(program->shaderProg,name);
	if ((int)location != INVALID_LOCATION)
		attribLocations.insert(pair<string, GLuint>(name, location));
}

void Shader::addParam(const char* name) {
	GLuint location=glGetUniformLocation(program->shaderProg,name);
	if ((int)location != INVALID_LOCATION)
		paramLocations.insert(pair<string, GLuint>(name, location));
}

int Shader::findAttribLocation(const char* attrib) {
	map<string,GLuint>::iterator itor=attribLocations.find(attrib);
	if(itor!=attribLocations.end())
		return itor->second;
	return INVALID_LOCATION;
}

int Shader::findParamLocation(const char* param) {
	map<string,GLuint>::iterator itor=paramLocations.find(param);
	if(itor!=paramLocations.end())
		return itor->second;
	return INVALID_LOCATION;
}

void Shader::setInt(const char* param,int value) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniform1i(location, value);
}

void Shader::setSampler(const char* param,int value) {
	setInt(param,value);
}

void Shader::setFloat(const char* param,float value) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniform1f(location, value);
}

void Shader::setVector2(const char* param,float x,float y) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniform2f(location, x, y);
}

void Shader::setVector3(const char* param,float x,float y,float z) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniform3f(location, x, y, z);
}

void Shader::setVector4(const char* param,float x,float y,float z,float w) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniform4f(location, x, y, z, w);
}

void Shader::setMatrix4(const char* param,float* matrix) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void Shader::setMatrix4(const char* param,int count,float* matrices) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniformMatrix4fv(location, count, GL_FALSE, matrices);
}

void Shader::setMatrix3(const char* param, float* matrix) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniformMatrix3fv(location, 1, GL_FALSE, matrix);
}

void Shader::setMatrix3(const char* param, int count, float* matrices) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION)
		glUniformMatrix3fv(location, count, GL_FALSE, matrices);
}


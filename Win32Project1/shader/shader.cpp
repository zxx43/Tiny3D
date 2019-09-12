#include "shader.h"
#include "../constants/constants.h"
using namespace std;

Shader::Shader(const char* vert,const char* frag) {
	program = new ShaderProgram(vert, frag);
	bindedTexs.clear();
	texSlots.clear();
}

Shader::Shader(const char* vert, const char* frag, const char* geom) {
	program = new ShaderProgram(vert, frag, geom);
	bindedTexs.clear();
	texSlots.clear();
}

Shader::~Shader() {
	delete program;
	program = NULL;
	paramLocations.clear();
	attribLocations.clear();
	bindedTexs.clear();
	texSlots.clear();
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
	if (location != INVALID_LOCATION && program)
		glProgramUniform1i(program->shaderProg, location, value);
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
	if (location != INVALID_LOCATION && program)
		glProgramUniform1f(program->shaderProg, location, value);
}

void Shader::setVector2(const char* param,float x,float y) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniform2f(program->shaderProg, location, x, y);
}

void Shader::setVector3(const char* param,float x,float y,float z) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniform3f(program->shaderProg, location, x, y, z);
}

void Shader::setVector4(const char* param,float x,float y,float z,float w) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniform4f(program->shaderProg, location, x, y, z, w);
}

void Shader::setVector2v(const char* param, float* arr) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniform2fv(program->shaderProg, location, 1, arr);
}

void Shader::setVector3v(const char* param, float* arr) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniform3fv(program->shaderProg, location, 1, arr);
}

void Shader::setVector4v(const char* param, float* arr) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniform4fv(program->shaderProg, location, 1, arr);
}

void Shader::setMatrix4(const char* param,float* matrix) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniformMatrix4fv(program->shaderProg, location, 1, GL_FALSE, matrix);
}

void Shader::setMatrix4(const char* param,int count,float* matrices) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniformMatrix4fv(program->shaderProg, location, count, GL_FALSE, matrices);
}

void Shader::setMatrix3x4(const char* param, int count, float* matrices) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniformMatrix3x4fv(program->shaderProg, location, count, GL_FALSE, matrices);
}

void Shader::setMatrix3(const char* param, float* matrix) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniformMatrix3fv(program->shaderProg, location, 1, GL_FALSE, matrix);
}

void Shader::setMatrix3(const char* param, int count, float* matrices) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program)
		glProgramUniformMatrix3fv(program->shaderProg, location, count, GL_FALSE, matrices);
}

void Shader::setHandle64(const char* param, u64 value) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program) {
		glProgramUniformHandleui64ARB(program->shaderProg, location, value);
		bindedTexs[value] = true;
	}
}

void Shader::setHandle64v(const char* param, int count, u64* arr) {
	int location = findParamLocation(param);
	if (location == INVALID_LOCATION) {
		addParam(param);
		location = findParamLocation(param);
	}
	if (location != INVALID_LOCATION && program) {
		glProgramUniformHandleui64vARB(program->shaderProg, location, count, arr);
		for (int i = 0; i < count; i++)
			bindedTexs[arr[i]] = true;
	}
}


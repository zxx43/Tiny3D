#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "../render/glheader.h"
#include "../constants/constants.h"
#include "textfile.h"

class ShaderProgram {
private:
	GLuint vertShader;
	GLuint fragShader;
	GLuint tescShader;
	GLuint teseShader;
	GLuint geomShader;
public:
	GLuint shaderProg;
private:
	char* attach(const char* version, const char* attachStr, const char* shaderStr);
public:
	ShaderProgram(const char* vert, const char* frag, const char* defines = NULL, const char* tesc = NULL, const char* tese = NULL, const char* geom = NULL);
	~ShaderProgram();
	void use();
};

#endif /* SHADER_H_ */

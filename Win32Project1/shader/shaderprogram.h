#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "../render/glheader.h"
#include "../constants/constants.h"
#include "textfile.h"

class ShaderProgram {
public:
	GLuint vertShader;
	GLuint fragShader;
	GLuint tescShader;
	GLuint teseShader;
	GLuint geomShader;
	GLuint shaderProg;

	ShaderProgram(const char* vert, const char* frag, const char* defines = NULL, const char* tesc = NULL, const char* tese = NULL, const char* geom = NULL);
	~ShaderProgram();
	void use();
};

#endif /* SHADER_H_ */

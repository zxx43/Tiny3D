#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "../render/glheader.h"
#include "textfile.h"

class ShaderProgram {
public:
	GLuint vertShader;
	GLuint fragShader;
	GLuint shaderProg;

	ShaderProgram(const char* vert,const char* frag);
	~ShaderProgram();
	void use();
};

#endif /* SHADER_H_ */

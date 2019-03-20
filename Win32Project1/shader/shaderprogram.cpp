#include "shaderprogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

//Got this from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// it prints out shader info (debugging!)
void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printShaderInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else{
		printf("Shader Info Log: OK\n");
	}
}

//Got this from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// it prints out shader info (debugging!)
void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printProgramInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else{
		printf("Program Info Log: OK\n");
	}
}

ShaderProgram::ShaderProgram(const char* vert, const char* frag) {
	char *vs = 0, *fs = 0;

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	geomShader = NULL;

	vs = textFileRead((char*)vert);
	fs = textFileRead((char*)frag);

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(vertShader, 1, &vv, NULL);
	glShaderSource(fragShader, 1, &ff, NULL);

	free(vs);
	free(fs);

	glCompileShader(vertShader);
	glCompileShader(fragShader);

	printf("%s: ", vert);
	printShaderInfoLog(vertShader);
	printf("%s: ", frag);
	printShaderInfoLog(fragShader);

	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);

	glLinkProgram(shaderProg);
}

ShaderProgram::ShaderProgram(const char* vert, const char* frag, const char* geom) {
	char *vs = 0, *fs = 0, *gs = 0;

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	geomShader = glCreateShader(GL_GEOMETRY_SHADER);

	vs = textFileRead((char*)vert);
	fs = textFileRead((char*)frag);
	gs = textFileRead((char*)geom);

	const char* vv = vs;
	const char* ff = fs;
	const char* gg = gs;

	glShaderSource(vertShader, 1, &vv, NULL);
	glShaderSource(fragShader, 1, &ff, NULL);
	glShaderSource(geomShader, 1, &gg, NULL);

	free(vs);
	free(fs);
	free(gs);

	glCompileShader(vertShader);
	glCompileShader(fragShader);
	glCompileShader(geomShader);

	printf("%s: ", vert);
	printShaderInfoLog(vertShader);
	printf("%s: ", frag);
	printShaderInfoLog(fragShader);
	printf("%s: ", geom);
	printShaderInfoLog(geomShader);

	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);
	glAttachShader(shaderProg, geomShader);

	glLinkProgram(shaderProg);
}

ShaderProgram::~ShaderProgram() {
	glDetachShader(shaderProg, vertShader);
	glDetachShader(shaderProg, fragShader);
	if (geomShader) glDetachShader(shaderProg, geomShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	if (geomShader) glDeleteShader(geomShader);
	glDeleteProgram(shaderProg);
}

void ShaderProgram::use() {
	glUseProgram(shaderProg);
}

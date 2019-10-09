#include "shaderprogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

//Got this from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// it prints out shader info (debugging!)
void printShaderInfoLog(GLuint obj, const char* shaderStr)
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
		printf("%s\n", shaderStr);
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

char* ShaderProgram::attach(const char* version, const char* attachStr, const char* shaderStr) {
	if (!shaderStr) return NULL;
	string shstr = string(version) + string("\n");
	if (attachStr) shstr += string(attachStr) + string("\n");
	shstr += string(shaderStr);
	char* resStr = (char*)malloc(sizeof(char) * (shstr.length() + 1));
	strcpy(resStr, shstr.data());
	return resStr;
}

ShaderProgram::ShaderProgram(const char* vert, const char* frag, const char* defines, const char* tesc, const char* tese, const char* geom) {
	char *vs = NULL, *fs = NULL, *tc = NULL, *te = NULL, *gs = NULL;
	vertShader = NULL, fragShader = NULL, tescShader = NULL, teseShader = NULL, geomShader = NULL;

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (tesc) tescShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	if (tese) teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	if (geom) geomShader = glCreateShader(GL_GEOMETRY_SHADER);

	vs = textFileRead((char*)vert);
	fs = textFileRead((char*)frag);
	if (tesc) tc = textFileRead((char*)tesc);
	if (tese) te = textFileRead((char*)tese);
	if (geom) gs = textFileRead((char*)geom);
	
	const char* version = "#version 450";
	char* vv = attach(version, defines, vs);
	char* ff = attach(version, defines, fs);
	char* cc = attach(version, defines, tc);
	char* ee = attach(version, defines, te);
	char* gg = attach(version, defines, gs);

	glShaderSource(vertShader, 1, &vv, NULL);
	glShaderSource(fragShader, 1, &ff, NULL);
	if (cc) glShaderSource(tescShader, 1, &cc, NULL);
	if (ee) glShaderSource(teseShader, 1, &ee, NULL);
	if (gg) glShaderSource(geomShader, 1, &gg, NULL);

	free(vs);
	free(fs);
	if (tc) free(tc);
	if (te) free(te);
	if (gs) free(gs);

	glCompileShader(vertShader);
	glCompileShader(fragShader);
	if (tescShader) glCompileShader(tescShader);
	if (teseShader) glCompileShader(teseShader);
	if (geomShader) glCompileShader(geomShader);

	printf("%s: ", vert);
	printShaderInfoLog(vertShader, vv);
	printf("%s: ", frag);
	printShaderInfoLog(fragShader, ff);
	if (tesc) {
		printf("%s: ", tesc);
		printShaderInfoLog(tescShader, cc);
	}
	if (tese) {
		printf("%s: ", tese);
		printShaderInfoLog(teseShader, ee);
	}
	if (geom) {
		printf("%s: ", geom);
		printShaderInfoLog(geomShader, gg);
	}

	if (vv) free(vv);
	if (ff) free(ff);
	if (cc) free(cc);
	if (ee) free(ee);
	if (gg) free(gg);

	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);
	if (tescShader) glAttachShader(shaderProg, tescShader);
	if (teseShader) glAttachShader(shaderProg, teseShader);
	if (geomShader) glAttachShader(shaderProg, geomShader);

	glLinkProgram(shaderProg);
}

ShaderProgram::~ShaderProgram() {
	glDetachShader(shaderProg, vertShader);
	glDetachShader(shaderProg, fragShader);
	if (tescShader) glDetachShader(shaderProg, tescShader);
	if (teseShader) glDetachShader(shaderProg, teseShader);
	if (geomShader) glDetachShader(shaderProg, geomShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	if (tescShader) glDeleteShader(tescShader);
	if (teseShader) glDeleteShader(teseShader);
	if (geomShader) glDeleteShader(geomShader);
	glDeleteProgram(shaderProg);
}

void ShaderProgram::use() {
	glUseProgram(shaderProg);
}

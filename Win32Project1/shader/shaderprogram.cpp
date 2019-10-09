#include "shaderprogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
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
	
	string vss = string(vs);
	string fss = string(fs);
	string tcs = "", tes = "", gss = "";
	if (tc) tcs = string(tc);
	if (te) tes = string(te);
	if (gs) gss = string(gs);
	if (defines) {
		string defStr = "\n" + string(defines) + "\n";
		vss = defStr + vss;
		fss = defStr + fss;
		if (tcs.length() > 0) tcs = defStr + tcs;
		if (tes.length() > 0) tes = defStr + tes;
		if (gss.length() > 0) gss = defStr + gss;
	}
	string version = "#version 450\n";
	vss = version + vss;
	fss = version + fss;
	if (tcs.length() > 0) tcs = version + tcs;
	if (tes.length() > 0) tes = version + tes;
	if (gss.length() > 0) gss = version + gss;

	const char* vv = vss.data();
	const char* ff = fss.data();
	const char* cc = tcs.length() > 0 ? tcs.data() : NULL;
	const char* ee = tes.length() > 0 ? tes.data() : NULL;
	const char* gg = gss.length() > 0 ? gss.data() : NULL;

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
	printShaderInfoLog(vertShader);
	printf("%s: ", frag);
	printShaderInfoLog(fragShader);
	if (tesc) {
		printf("%s: ", tesc);
		printShaderInfoLog(tescShader);
	}
	if (tese) {
		printf("%s: ", tese);
		printShaderInfoLog(teseShader);
	}
	if (geom) {
		printf("%s: ", geom);
		printShaderInfoLog(geomShader);
	}

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

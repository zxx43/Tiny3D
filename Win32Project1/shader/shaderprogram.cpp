#include "shaderprogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
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
	GLint isLinked = 0;
	glGetProgramiv(obj, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		printf("Link error!\n");
	}

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

std::string ShaderProgram::attach(const char* version, const char* shaderStr) {
	if (!shaderStr) return "";
	string shstr = string(version) + string("\n");
	if (exStr.length() > 0) shstr += exStr + string("\n");
	shstr += string(shaderStr);
	return shstr;
}

ShaderProgram::ShaderProgram(const char* vert, const char* frag, const char* tesc, const char* tese, const char* geom) {
	vfile = (char*)vert, ffile = (char*)frag, cfile = (char*)tesc, efile = (char*)tese, gfile = (char*)geom;
	vs = NULL, fs = NULL, tc = NULL, te = NULL, gs = NULL;
	vertShader = NULL, fragShader = NULL, tescShader = NULL, teseShader = NULL, geomShader = NULL;

	vs = textFileRead(vfile);
	fs = textFileRead(ffile);
	if (cfile) tc = textFileRead(cfile);
	if (efile) te = textFileRead(efile);
	if (gfile) gs = textFileRead(gfile);

	exStr = "";
}

void ShaderProgram::attachDef(const char* def, const char* value) {
	exStr += "#define " + string(def) + " " + string(value) + "\n";
}

void ShaderProgram::attachEx(const char* ex) {
	exStr += string(ex) + "\n";
}

void ShaderProgram::compose() {
	const char* version = "#version 400";
	vStr = attach(version, vs);
	fStr = attach(version, fs);
	cStr = attach(version, tc);
	eStr = attach(version, te);
	gStr = attach(version, gs);
}

void ShaderProgram::compile(bool preload) {
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (tc) tescShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	if (te) teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	if (gs) geomShader = glCreateShader(GL_GEOMETRY_SHADER);

	GLchar* vv = (GLchar*)vStr.data();
	GLchar* ff = (GLchar*)fStr.data();
	GLchar* cc = (GLchar*)cStr.data();
	GLchar* ee = (GLchar*)eStr.data();
	GLchar* gg = (GLchar*)gStr.data();

	glShaderSource(vertShader, 1, &vv, NULL);
	glShaderSource(fragShader, 1, &ff, NULL);
	if (cStr.length() > 0) glShaderSource(tescShader, 1, &cc, NULL);
	if (eStr.length() > 0) glShaderSource(teseShader, 1, &ee, NULL);
	if (gStr.length() > 0) glShaderSource(geomShader, 1, &gg, NULL);

	glCompileShader(vertShader);
	glCompileShader(fragShader);
	if (tescShader) glCompileShader(tescShader);
	if (teseShader) glCompileShader(teseShader);
	if (geomShader) glCompileShader(geomShader);

	if (!preload) {
		printf("%s: ", vfile);
		printShaderInfoLog(vertShader, vv);
		printf("%s: ", ffile);
		printShaderInfoLog(fragShader, ff);
		if (cfile) {
			printf("%s: ", cfile);
			printShaderInfoLog(tescShader, cc);
		}
		if (efile) {
			printf("%s: ", efile);
			printShaderInfoLog(teseShader, ee);
		}
		if (gfile) {
			printf("%s: ", gfile);
			printShaderInfoLog(geomShader, gg);
		}
	}

	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);
	if (tescShader) glAttachShader(shaderProg, tescShader);
	if (teseShader) glAttachShader(shaderProg, teseShader);
	if (geomShader) glAttachShader(shaderProg, geomShader);

	glLinkProgram(shaderProg);
	if (!preload) {
		printf("%s, %s: ", vfile, ffile);
		printProgramInfoLog(shaderProg);
	}
}

void ShaderProgram::dettach() {
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

ShaderProgram::~ShaderProgram() {
	dettach();

	if (vs) free(vs); vs = NULL;
	if (fs) free(fs); fs = NULL;
	if (tc) free(tc); tc = NULL;
	if (te) free(te); te = NULL;
	if (gs) free(gs); gs = NULL;
}

void ShaderProgram::use() {
	glUseProgram(shaderProg);
}

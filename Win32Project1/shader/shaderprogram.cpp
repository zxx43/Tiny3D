#include "shaderprogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
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
		printf("\n\033[0;40;31mprintShaderInfoLog: %s\n", infoLog);
		printf("%s\033[0m\n\n", shaderStr);
		free(infoLog);
	}
	else{
		printf("Shader Info Log: OK\n");
	}
}

//Got this from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// it prints out shader info (debugging!)
void printProgramInfoLog(GLuint obj, const char* vsStr, const char* fsStr)
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
		printf("\033[0;40;33mprintProgramInfoLog: %s\n", infoLog);
		if (vsStr && fsStr)
			printf("%s\n%s\033[0m\n\n", vsStr, fsStr);
		free(infoLog);
	}
	else{
		printf("Program Info Log: OK\n");
	}
}

std::string ShaderProgram::attach(const char* version, const char* shaderStr) {
	if (!shaderStr || strlen(shaderStr) == 0) return "";
	string shstr = string(version) + string("\n");
	if (exStr.length() > 0) shstr += exStr + string("\n");
	shstr += string(shaderStr);
	return shstr;
}

ShaderProgram::ShaderProgram(const char* vert, const char* frag, const char* tesc, const char* tese, const char* geom) {
	vfile = (char*)vert, ffile = (char*)frag, cfile = (char*)tesc, efile = (char*)tese, gfile = (char*)geom, pfile = NULL;
	vs = NULL, fs = NULL, tc = NULL, te = NULL, gs = NULL, cs = NULL;
	vertShader = NULL, fragShader = NULL, tescShader = NULL, teseShader = NULL, geomShader = NULL, compShader = NULL;

	if (vfile) vs = textFileRead(vfile);
	if (ffile) fs = textFileRead(ffile);
	if (cfile) tc = textFileRead(cfile);
	if (efile) te = textFileRead(efile);
	if (gfile) gs = textFileRead(gfile);

	exStr = "";
}

ShaderProgram::ShaderProgram(const char* comp) {
	vfile = NULL, ffile = NULL, cfile = NULL, efile = NULL, gfile = NULL, pfile = (char*)comp;
	vs = NULL, fs = NULL, tc = NULL, te = NULL, gs = NULL, cs = NULL;
	vertShader = NULL, fragShader = NULL, tescShader = NULL, teseShader = NULL, geomShader = NULL, compShader = NULL;

	if (pfile) cs = textFileRead(pfile);

	exStr = "";
}

void ShaderProgram::attachDef(const char* def, const char* value) {
	exStr += "#define " + string(def) + " " + string(value) + "\n";
}

void ShaderProgram::attachEx(const char* ex) {
	exStr += string(ex) + "\n";
}

void ShaderProgram::compose() {
	const char* version = "#version 450";
	if (vs && fs) {
		vStr = attach(version, handleInclude(vs).data());
		fStr = attach(version, handleInclude(fs).data());
		cStr = attach(version, handleInclude(tc).data());
		eStr = attach(version, handleInclude(te).data());
		gStr = attach(version, handleInclude(gs).data());
	}
	if(cs) pStr = attach(version, handleInclude(cs).data());
}

string ShaderProgram::handleInclude(const char* shaderStr) {
	if (!shaderStr) return "";
	vector<string> incs; incs.clear();
	string res(shaderStr);
	int fs = 0, fe = 0;
	int fi = res.find("#include");
	while (fi != string::npos) {
		res = res.replace(fi, strlen("#include"), string(""));
		
		fs = res.find_first_of('"');
		string path = res.substr(fs + 1);
		fe = path.find_first_of('"');
		path = path.substr(0, fe);
		
		res = res.replace(fs, strlen(path.data()) + 2, "");

		char* fileStr = textFileRead((char*)path.data());
		incs.push_back(string(fileStr) + string("\n"));
		free(fileStr);

		fi = res.find("#include");
	}
	string incStr("");
	for (int i = 0; i < incs.size(); ++i)
		incStr += incs[i];
	res = incStr + res;
	return res;
}

void ShaderProgram::compile(bool preload) {
	if (vs) vertShader = glCreateShader(GL_VERTEX_SHADER);
	if (fs) fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (tc) tescShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	if (te) teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	if (gs) geomShader = glCreateShader(GL_GEOMETRY_SHADER);
	if (cs) compShader = glCreateShader(GL_COMPUTE_SHADER);

	GLchar* vv = (GLchar*)vStr.data();
	GLchar* ff = (GLchar*)fStr.data();
	GLchar* cc = (GLchar*)cStr.data();
	GLchar* ee = (GLchar*)eStr.data();
	GLchar* gg = (GLchar*)gStr.data();
	GLchar* pp = (GLchar*)pStr.data();

	if (vStr.length() > 0) glShaderSource(vertShader, 1, &vv, NULL);
	if (fStr.length() > 0) glShaderSource(fragShader, 1, &ff, NULL);
	if (cStr.length() > 0) glShaderSource(tescShader, 1, &cc, NULL);
	if (eStr.length() > 0) glShaderSource(teseShader, 1, &ee, NULL);
	if (gStr.length() > 0) glShaderSource(geomShader, 1, &gg, NULL);
	if (pStr.length() > 0) glShaderSource(compShader, 1, &pp, NULL);

	if (vertShader) glCompileShader(vertShader);
	if (fragShader) glCompileShader(fragShader);
	if (tescShader) glCompileShader(tescShader);
	if (teseShader) glCompileShader(teseShader);
	if (geomShader) glCompileShader(geomShader);
	if (compShader) glCompileShader(compShader);

	if (!preload) {
		if (vfile) {
			printf("%s: ", vfile);
			printShaderInfoLog(vertShader, vv);
		}
		if (ffile) {
			printf("%s: ", ffile);
			printShaderInfoLog(fragShader, ff);
		}
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
		if (pfile) {
			printf("%s: ", pfile);
			printShaderInfoLog(compShader, pp);
		}
	}

	shaderProg = glCreateProgram();
	if (vertShader) glAttachShader(shaderProg, vertShader);
	if (fragShader) glAttachShader(shaderProg, fragShader);
	if (tescShader) glAttachShader(shaderProg, tescShader);
	if (teseShader) glAttachShader(shaderProg, teseShader);
	if (geomShader) glAttachShader(shaderProg, geomShader);
	if (compShader) glAttachShader(shaderProg, compShader);

	glLinkProgram(shaderProg);
	if (!preload) {
		if (vfile && ffile)
			printf("%s, %s: ", vfile, ffile);
		else if (pfile)
			printf("%s: ", pfile);
		printProgramInfoLog(shaderProg, vv, ff);
	}
}

void ShaderProgram::dettach() {
	if (vertShader) glDetachShader(shaderProg, vertShader);
	if (fragShader) glDetachShader(shaderProg, fragShader);
	if (tescShader) glDetachShader(shaderProg, tescShader);
	if (teseShader) glDetachShader(shaderProg, teseShader);
	if (geomShader) glDetachShader(shaderProg, geomShader);
	if (compShader) glDetachShader(shaderProg, compShader);

	if (vertShader) glDeleteShader(vertShader);
	if (fragShader) glDeleteShader(fragShader);
	if (tescShader) glDeleteShader(tescShader);
	if (teseShader) glDeleteShader(teseShader);
	if (geomShader) glDeleteShader(geomShader);
	if (compShader) glDeleteShader(compShader);
	glDeleteProgram(shaderProg);
}

ShaderProgram::~ShaderProgram() {
	dettach();

	if (vs) free(vs); vs = NULL;
	if (fs) free(fs); fs = NULL;
	if (tc) free(tc); tc = NULL;
	if (te) free(te); te = NULL;
	if (gs) free(gs); gs = NULL;
	if (cs) free(cs); cs = NULL;
}

void ShaderProgram::use() {
	glUseProgram(shaderProg);
}

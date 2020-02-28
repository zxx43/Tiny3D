#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "../render/glheader.h"
#include "../constants/constants.h"
#include "textfile.h"
#include <string>

class ShaderProgram {
private:
	GLuint vertShader;
	GLuint fragShader;
	GLuint tescShader;
	GLuint teseShader;
	GLuint geomShader;
	char *vfile, *ffile, *cfile, *efile, *gfile;
	char *vs, *fs, *tc, *te, *gs;
	std::string vStr, fStr, cStr, eStr, gStr;
private:
	GLuint compShader;
	char* pfile;
	char* cs;
	std::string pStr;
private:
	std::string exStr;
public:
	GLuint shaderProg;
private:
	std::string attach(const char* version, const char* shaderStr);
	std::string handleInclude(const char* shaderStr);
public:
	ShaderProgram(const char* vert, const char* frag, const char* tesc = NULL, const char* tese = NULL, const char* geom = NULL);
	ShaderProgram(const char* comp);
	~ShaderProgram();
public:
	void compose();
	void compile(bool preload);
	void dettach();
	void attachDef(const char* def, const char* value);
	void attachEx(const char* ex);
	void use();
};

#endif /* SHADER_H_ */

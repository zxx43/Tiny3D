#include "shadermanager.h"
using namespace std;

ShaderManager::ShaderManager() {
	shaders.clear();
	shaderBindTex.clear();
}

ShaderManager::~ShaderManager() {
	map<string,Shader*>::iterator itor;
	for(itor=shaders.begin();itor!=shaders.end();itor++)
		delete itor->second;
	shaders.clear();
	shaderBindTex.clear();
}

Shader* ShaderManager::addShader(const char* name, const char* vs, const char* fs, const char* defines, const char* tc, const char* te, const char* gs) {
	Shader* shader = new Shader(vs, fs, defines, tc, te, gs);
	shader->name = name;
	shaders.insert(pair<string, Shader*>(name, shader));
	return shader;
}

Shader* ShaderManager::findShader(const char* name) {
	map<string,Shader*>::iterator itor=shaders.find(name);
	if(itor!=shaders.end())
		return itor->second;
	return NULL;
}

void ShaderManager::addShaderBindTex(Shader* shader) {
	shaderBindTex.push_back(shader);
}


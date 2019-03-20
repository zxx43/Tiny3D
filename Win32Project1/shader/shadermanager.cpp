#include "shadermanager.h"
using namespace std;

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {
	map<string,Shader*>::iterator itor;
	for(itor=shaders.begin();itor!=shaders.end();itor++)
		delete itor->second;
	shaders.clear();
}

Shader* ShaderManager::addShader(const char* name,const char* vs,const char* fs) {
	Shader* shader=new Shader(vs,fs);
	shader->name = name;
	shaders.insert(pair<string,Shader*>(name,shader));
	return shader;
}

Shader* ShaderManager::addShader(const char* name, const char* vs, const char* fs, const char* gs) {
	Shader* shader = new Shader(vs, fs, gs);
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


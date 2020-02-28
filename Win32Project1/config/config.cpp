#include "config.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

Config::Config(const char* path) {
	data.clear();

	ifstream ifs(path);
	string line;
	string key;
	float value;
	while (getline(ifs, line)) {
		istringstream ins(line);
		ins >> key >> value;
		data[key] = value;
	}
	ifs.close();
}

Config::~Config() {
	data.clear();
}

bool Config::getFloat(const char* key, float& value) {
	map<string, float>::iterator itor = data.find(key);
	if (itor == data.end()) return false;
	value = itor->second;
	return true;
}

bool Config::getInt(const char* key, int& value) {
	map<string, float>::iterator itor = data.find(key);
	if (itor == data.end()) return false;
	value = (int)itor->second;
	return true;
}

bool Config::getBool(const char* key, bool& value) {
	map<string, float>::iterator itor = data.find(key);
	if (itor == data.end()) return false;
	float fValue = itor->second;
	value = fValue > 0.1 ? true : false;
	return true;
}
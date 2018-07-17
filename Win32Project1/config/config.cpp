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

bool Config::get(const char* key, float& value) {
	map<string, float>::iterator itor = data.find(key);
	if (itor == data.end()) return false;
	value = itor->second;
	return true;
}
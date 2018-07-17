#ifndef CONFIG_H_
#define CONFIG_H_

#include <map>
#include <string>

class Config {
private:
	std::map<std::string, float> data;
public:
	Config(const char* path);
	~Config();
	bool get(const char* key, float& value);
};

#endif
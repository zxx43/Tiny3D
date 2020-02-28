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
	bool getFloat(const char* key, float& value);
	bool getInt(const char* key, int& value);
	bool getBool(const char* key, bool& value);
};

#endif
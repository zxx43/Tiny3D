#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include "../maths/Maths.h"
#include <vector>
#include <map>
#include <string>

#define DEFAULT_MAT "default_mat"
#define BLACK_MAT "black_mat"
#define MAX_MAT 1024

struct Material
{
	int id;
	std::string name;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	bool prepared;
	std::string tex1, tex2, tex3, tex4;
	bool srgb1, srgb2, srgb3, srgb4;
	vec4 texids;
	vec2 exTexids;
	bool singleFace, leaf;
	Material(const char* mtlName) {
		id = -1;
		name = mtlName;
		prepared = false;
		tex1 = "", tex2 = "", tex3 = "", tex4 = "";
		srgb1 = true, srgb2 = false, srgb3 = false, srgb4 = false;
		texids = vec4(-1, -1, -1, -1);
		exTexids = vec2(-1, -1);
		ambient = vec3(0.4, 0.4, 0.4);
		diffuse = vec3(0.6, 0.6, 0.6);
		specular = vec3(0, 0, 0);
		singleFace = false;
		leaf = false;
	}
}; 

struct RenderBuffer;
class MaterialManager {
public:
	static unsigned int MaterialIndex;
public:
	static MaterialManager* materials;
public:
	static void Init();
	static void Release();
private:
	std::vector<Material*> materialList;
	std::map<std::string, Material*> materialMap;
	float* pbrMapDatas;
	RenderBuffer* materialBuffer;
	unsigned int mapChannel;
private:
	RenderBuffer* createMaterials(int maxCount);
private:
	MaterialManager();
	~MaterialManager();
public:
	unsigned int add(Material* material);
	void remove(unsigned int i);
	Material* find(unsigned int i);
	int find(std::string name);
	unsigned int size();
	void updateMapDatas();
	float* getPBRDatas() { return pbrMapDatas; }
	void useMaterialBuffer(int location);
};

#endif

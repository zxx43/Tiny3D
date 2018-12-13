#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include "../maths/Maths.h"
#include <vector>
#include <map>
#include <string>

#define DEFAULT_MAT "default_mat"
#define BLACK_MAT "black_mat"

struct Material
{
	int id;
	std::string name;
	VECTOR3D ambient;
	VECTOR3D diffuse;
	VECTOR3D specular;
	bool useArray;
	std::string tex1, tex2, tex3;
	VECTOR4D texOfs1, texOfs2;
	VECTOR4D texSize;
	Material(const char* mtlName) {
		id = -1;
		name = mtlName;
		useArray = false;
		tex1 = "", tex2 = "", tex3 = "";
		texOfs1 = VECTOR4D(-1, -1, -1, -1);
		texOfs2 = VECTOR4D(-1, -1, -1, -1);
		texSize = VECTOR4D(-1, -1, -1, -1);
		ambient = VECTOR3D(0.4, 0.4, 0.4);
		diffuse = VECTOR3D(0.6, 0.6, 0.6);
		specular = VECTOR3D(0, 0, 0);
	}
}; 

class MaterialManager {
public:
	static MaterialManager* materials;
public:
	static void Init();
	static void Release();
private:
	std::vector<Material*> materialList;
	std::map<std::string, Material*> materialMap;
private:
	MaterialManager();
	~MaterialManager();
public:
	unsigned int add(Material* material);
	void remove(unsigned int i);
	Material* find(unsigned int i);
	int find(std::string name);
	unsigned int size();
};

#endif

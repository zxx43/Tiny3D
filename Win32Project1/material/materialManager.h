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
	VECTOR4D texture;
	Material(const char* mtlName) {
		id = -1;
		name = mtlName;
		texture = VECTOR4D(-1,-1,-1,-1);
		ambient = VECTOR3D(0.4, 0.4, 0.4);
		diffuse = VECTOR3D(0.6, 0.6, 0.6);
		specular = VECTOR3D(0, 0, 0);
	}
}; 

class MaterialManager {
private:
	std::vector<Material*> materialList;
	std::map<std::string, Material*> materialMap;
public:
	MaterialManager();
	~MaterialManager();
	unsigned int add(Material* material);
	void remove(unsigned int i);
	Material* find(unsigned int i);
	int find(std::string name);
	unsigned int size();
};

extern MaterialManager* materials;
void InitMaterialManager();
void ReleaseMaterialManager();

#endif

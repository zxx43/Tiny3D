#include "materialManager.h"
#include "../constants/constants.h"

MaterialManager* MaterialManager::materials = NULL;

MaterialManager::MaterialManager() {
	materialList.clear();
	materialMap.clear();
	Material* defaultMat = new Material(DEFAULT_MAT);
	add(defaultMat);
	Material* blackMat = new Material(BLACK_MAT);
	blackMat->ambient = VECTOR3D(0, 0, 0);
	blackMat->diffuse = VECTOR3D(0, 0, 0);
	add(blackMat);
}

MaterialManager::~MaterialManager() {
	for (unsigned int i = 0; i < materialList.size(); i++)
		delete materialList[i];
	materialList.clear();
	materialMap.clear();
}

unsigned int MaterialManager::add(Material* material) {
	materialList.push_back(material);
	materialMap[material->name] = material;
	int mid = materialList.size() - 1; // Start from 0
	material->id = mid;
	return mid;
}

void MaterialManager::remove(unsigned int i) {
	if (materialList.size() < i + 1) return;
	int oldMid = materialList[i]->id;
	std::string oldName = materialList[i]->name;
	delete materialList[i];
	Material* mtlEmp = new Material(oldName.c_str());
	mtlEmp->id = oldMid;
	materialList[i] = mtlEmp;
	materialMap[oldName] = mtlEmp;
}

Material* MaterialManager::find(unsigned int i) {
	if (materialList.size() < i + 1) return NULL;
	return materialList[i];
}

int MaterialManager::find(std::string name) {
	if (materialMap.count(name) <= 0) return 0;
	return materialMap[name]->id;
}

unsigned int MaterialManager::size() {
	return materialList.size();
}

void MaterialManager::Init() {
	if (!MaterialManager::materials)
		MaterialManager::materials = new MaterialManager();
}

void MaterialManager::Release() {
	if (MaterialManager::materials)
		delete MaterialManager::materials;
	MaterialManager::materials = NULL;
}
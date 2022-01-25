#include "materialManager.h"
#include "../constants/constants.h"
#include "../render/renderBuffer.h"

MaterialManager* MaterialManager::materials = NULL;
uint MaterialManager::MaterialIndex = 0;

MaterialManager::MaterialManager() {
	materialList.clear();
	materialMap.clear();
	Material* defaultMat = new Material(DEFAULT_MAT);
	add(defaultMat);
	pbrMapDatas = NULL;
	materialBuffer = NULL;
	mapChannel = 0;
}

MaterialManager::~MaterialManager() {
	for (unsigned int i = 0; i < materialList.size(); i++)
		delete materialList[i];
	materialList.clear();
	materialMap.clear();
	if (pbrMapDatas) free(pbrMapDatas); pbrMapDatas = NULL;
	if (materialBuffer) delete materialBuffer; materialBuffer = NULL;
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

void MaterialManager::updateMapDatas() {
	mapChannel = 8;
	if (pbrMapDatas) free(pbrMapDatas);
	pbrMapDatas = (float*)malloc(materialList.size() * sizeof(float) * mapChannel);
	for (int i = 0; i < materialList.size(); ++i) {
		Material* mat = materialList[i];
		pbrMapDatas[i * mapChannel + 0] = mat->texids.x;
		pbrMapDatas[i * mapChannel + 1] = mat->texids.y;
		pbrMapDatas[i * mapChannel + 2] = mat->texids.z;
		pbrMapDatas[i * mapChannel + 3] = mat->texids.w;
		pbrMapDatas[i * mapChannel + 4] = mat->ambient.GetX();
		pbrMapDatas[i * mapChannel + 5] = mat->diffuse.GetX();
		pbrMapDatas[i * mapChannel + 6] = mat->specular.GetX();
		pbrMapDatas[i * mapChannel + 7] = mat->singleFace ? 1.0 : 0.0;
	}

	if (!materialBuffer) materialBuffer = createMaterials(MAX_MAT);
	materialBuffer->updateBufferData(MaterialIndex, size(), pbrMapDatas);
}

RenderBuffer* MaterialManager::createMaterials(int maxCount) {
	RenderBuffer* buffer = new RenderBuffer(1, false);
	buffer->setBufferData(GL_SHADER_STORAGE_BUFFER, MaterialIndex, GL_FLOAT, maxCount, mapChannel, GL_DYNAMIC_DRAW, NULL);
	return buffer;
}

void MaterialManager::useMaterialBuffer(int location) { 
	if (materialBuffer) materialBuffer->setShaderBase(GL_SHADER_STORAGE_BUFFER, MaterialIndex, location); 
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
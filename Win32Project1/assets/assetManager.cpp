#include "assetManager.h"
#include "../mesh/box.h"
#include "../mesh/sphere.h"
#include "../mesh/board.h"
#include "../mesh/quad.h"
#include "../constants/constants.h"
using namespace std;

AssetManager* AssetManager::assetManager = NULL;

AssetManager::AssetManager() {
	textures = new ImageSet();
	meshes.clear();
	animations.clear();
}

AssetManager::~AssetManager() {
	map<string, Mesh*>::iterator itor;
	for (itor = meshes.begin(); itor != meshes.end(); itor++)
		delete itor->second;
	meshes.clear();
	map<string, Animation*>::iterator iter;
	for (iter = animations.begin(); iter != animations.end(); iter++)
		delete iter->second;
	animations.clear();
	if (textures) delete textures;
	textures = NULL;
}

void AssetManager::initTextureArray() {
	textures->initTextureArray(COMMON_TEXTURE);
}

void AssetManager::Init() {
	if (!AssetManager::assetManager) {
		AssetManager::assetManager = new AssetManager();

		// Load some basic meshes
		AssetManager::assetManager->meshes["box"] = new Box();
		AssetManager::assetManager->meshes["sphere"] = new Sphere(16, 16);
		AssetManager::assetManager->meshes["board"] = new Board();
		AssetManager::assetManager->meshes["quad"] = new Quad();
	}
}

void AssetManager::Release() {
	if (AssetManager::assetManager) 
		delete AssetManager::assetManager;
	AssetManager::assetManager = NULL;
}
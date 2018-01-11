#include "assetManager.h"
#include "../mesh/box.h"
#include "../mesh/sphere.h"
#include "../mesh/board.h"
#include "../mesh/quad.h"
#include "../constants/constants.h"
using namespace std;

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


AssetManager* assetManager = NULL;

void InitAssetManager() {
	assetManager = new AssetManager();

	// Load some basic meshes
	assetManager->meshes["box"] = new Box();
	assetManager->meshes["sphere"] = new Sphere(16, 16);
	assetManager->meshes["board"] = new Board();
	assetManager->meshes["quad"] = new Quad();
}

void ReleaseAssetManager() {
	if (assetManager) delete assetManager;
	assetManager = NULL;
}
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
	skyTexture = NULL;
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
	if (skyTexture) delete skyTexture;
	skyTexture = NULL;
}

void AssetManager::addTexture(const char* name) {
	textures->addTexture(name);
}

void AssetManager::initTextureArray() {
	textures->initTextureArray(COMMON_TEXTURE);
}

int AssetManager::findTexture(const char* name) {
	return textures->findTexture(name);
}

void AssetManager::setSkyTexture(CubeMap* tex) {
	if (tex == skyTexture) return;
	if (skyTexture) delete skyTexture;
	skyTexture = tex;
}

CubeMap* AssetManager::getSkyTexture() {
	return skyTexture;
}

void AssetManager::addMesh(const char* name, Mesh* mesh) {
	meshes[name] = mesh;
}

void AssetManager::addAnimation(const char* name, Animation* animation) {
	animations[name] = animation;
}

void AssetManager::Init() {
	if (!AssetManager::assetManager) {
		AssetManager::assetManager = new AssetManager();

		// Load some basic meshes
		AssetManager::assetManager->addMesh("box", new Box());
		AssetManager::assetManager->addMesh("sphere", new Sphere(16, 16));
		AssetManager::assetManager->addMesh("board", new Board());
		AssetManager::assetManager->addMesh("quad", new Quad());
		AssetManager::assetManager->addMesh("billboard", new Board(1, 1, 1, 0, 0.5));
		AssetManager::assetManager->meshes["billboard"]->setIsBillboard(true);
	}
}

void AssetManager::Release() {
	if (AssetManager::assetManager) 
		delete AssetManager::assetManager;
	AssetManager::assetManager = NULL;
}
#include "assetManager.h"
#include "../mesh/box.h"
#include "../mesh/sphere.h"
#include "../mesh/board.h"
#include "../mesh/quad.h"
#include "../constants/constants.h"
using namespace std;

AssetManager* AssetManager::assetManager = NULL;

AssetManager::AssetManager() {
	texAlt = new TextureAtlas();
	texArray = new ImageSet();
	skyTexture = NULL;
	reflectTexture = NULL;
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
	if (texAlt) delete texAlt;
	texAlt = NULL;
	if (texArray) delete texArray;
	texArray = NULL;
	if (skyTexture) delete skyTexture;
	skyTexture = NULL;
}

void AssetManager::addTexture2Alt(const char* name) {
	texAlt->addTexture(name);
}

void AssetManager::initTextureAtlas() {
	texAlt->createAtlas(COMMON_TEXTURE);
}

TexOffset* AssetManager::findTextureAtlasOfs(const char* name) {
	return texAlt->findTextureOfs(name);
}

void AssetManager::addTexture2Array(const char* name) {
	texArray->addTexture(name);
}

void AssetManager::initTextureArray() {
	texArray->initTextureArray(COMMON_TEXTURE);
}

int AssetManager::findTextureInArray(const char* name) {
	return texArray->findTexture(name);
}

void AssetManager::setSkyTexture(CubeMap* tex) {
	if (tex == skyTexture) return;
	if (skyTexture) delete skyTexture;
	skyTexture = tex;
}

CubeMap* AssetManager::getSkyTexture() {
	return skyTexture;
}

void AssetManager::setReflectTexture(Texture2D* tex) {
	reflectTexture = tex;
}

Texture2D* AssetManager::getReflectTexture() {
	return reflectTexture;
}

void AssetManager::addMesh(const char* name, Mesh* mesh, bool billboard) {
	meshes[name] = mesh;
	meshes[name]->setIsBillboard(billboard);
}

void AssetManager::addAnimation(const char* name, Animation* animation) {
	animations[name] = animation;
}

void AssetManager::createMaterialTextureAtlas(MaterialManager* mtls) {
	for (uint i = 0; i < mtls->size(); i++) {
		Material* mat = mtls->find(i);
		if (mat->tex1.length() > 0) {
			TexOffset* ofs1 = findTextureAtlasOfs(mat->tex1.data());
			if (ofs1) {
				mat->texOfs1.x = ofs1->x;
				mat->texOfs1.y = ofs1->y;
				mat->texSize.x = texAlt->perImgWidth;
				mat->texSize.y = texAlt->perImgHeight;
				mat->texSize.z = texAlt->pixW;
				mat->texSize.w = texAlt->pixH;
			}
		}
		if (mat->tex2.length() > 0) {
			TexOffset* ofs2 = findTextureAtlasOfs(mat->tex2.data());
			if (ofs2) {
				mat->texOfs1.z = ofs2->x;
				mat->texOfs1.w = ofs2->y;
			}
		}
		if (mat->tex3.length() > 0) {
			TexOffset* ofs3 = findTextureAtlasOfs(mat->tex3.data());
			if (ofs3) {
				mat->texOfs2.x = ofs3->x;
				mat->texOfs2.y = ofs3->y;
			}
		}
	}
}

void AssetManager::Init() {
	if (!AssetManager::assetManager) {
		AssetManager::assetManager = new AssetManager();

		// Load some basic meshes
		AssetManager::assetManager->addMesh("box", new Box());
		AssetManager::assetManager->addMesh("sphere", new Sphere(16, 16));
		AssetManager::assetManager->addMesh("board", new Board());
		AssetManager::assetManager->addMesh("quad", new Quad());
		AssetManager::assetManager->addMesh("billboard", new Board(1, 1, 1, 0, 0.5), true);
	}
}

void AssetManager::Release() {
	if (AssetManager::assetManager) 
		delete AssetManager::assetManager;
	AssetManager::assetManager = NULL;
}
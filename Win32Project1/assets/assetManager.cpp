#include "assetManager.h"
#include "../mesh/box.h"
#include "../mesh/sphere.h"
#include "../mesh/board.h"
#include "../mesh/quad.h"
#include "../mesh/terrain.h"
#include "../util/util.h"
using namespace std;

AssetManager* AssetManager::assetManager = NULL;

AssetManager::AssetManager() {
	texBld = new TextureBindless();
	skyTexture = NULL;
	envTexture = NULL;
	noise3DTexture = NULL;
	reflectTexture = NULL;
	heightTexture = NULL;
	heightNormalTex = NULL;
	distortionTex = -1;
	noiseTex = -1;
	roadTex = -1;
	meshes.clear();
	animations.clear();
	animationDatas.clear();
	frames = new FrameMgr();
}

AssetManager::~AssetManager() {
	map<string, Mesh*>::iterator itor;
	for (itor = meshes.begin(); itor != meshes.end(); ++itor)
		delete itor->second;
	meshes.clear();
	
	map<string, Animation*>::iterator iter;
	for (iter = animations.begin(); iter != animations.end(); ++iter)
		delete iter->second;
	animations.clear();

	map<string, AnimFrame*>::iterator it;
	for (it = animationDatas.begin(); it != animationDatas.end(); ++it)
		delete it->second;
	animationDatas.clear();

	delete frames;
	if (texBld) delete texBld; texBld = NULL;
	if (heightTexture) delete heightTexture; heightTexture = NULL;
	if (heightNormalTex) delete heightNormalTex; heightNormalTex = NULL;
	if (skyTexture) delete skyTexture;
	if (envTexture && envTexture != skyTexture) delete envTexture;
	skyTexture = NULL; envTexture = NULL;
	if (noise3DTexture) delete noise3DTexture; noise3DTexture = NULL;
}

void AssetManager::addTextureBindless(const char* name, bool srgb, int wrap) {
	texBld->addTexture(name, srgb, wrap);
}

void AssetManager::initTextureBindless(MaterialManager* mtls) {
	for (uint i = 0; i < mtls->size(); i++) {
		Material* mat = mtls->find(i);
		if (mat->prepared) continue;
		if (mat->tex1.length() > 0) {
			if (texBld->findTexture(mat->tex1.data()) < 0) texBld->addTexture(mat->tex1.data(), mat->srgb1);
			mat->texids.x = texBld->findTexture(mat->tex1.data());
		}
		if (mat->tex2.length() > 0) {
			if (texBld->findTexture(mat->tex2.data()) < 0) texBld->addTexture(mat->tex2.data(), mat->srgb2);
			mat->texids.y = texBld->findTexture(mat->tex2.data());
		}
		if (mat->tex3.length() > 0) {
			if (texBld->findTexture(mat->tex3.data()) < 0) texBld->addTexture(mat->tex3.data(), mat->srgb3);
			mat->texids.z = texBld->findTexture(mat->tex3.data());
		}
		if (mat->tex4.length() > 0) {
			if (texBld->findTexture(mat->tex4.data()) < 0) texBld->addTexture(mat->tex4.data(), mat->srgb4);
			mat->texids.w = texBld->findTexture(mat->tex4.data());
		}
		printf("mat %s: [%d]%s\n", mat->name.data(), (int)mat->texids.x, mat->tex1.data());
	}
	texBld->initData(COMMON_TEXTURE);
}

int AssetManager::findTextureBindless(const char* name) {
	return texBld->findTexture(name);
}

void AssetManager::setSkyTexture(CubeMap* tex) {
	if (tex == skyTexture) return;
	if (skyTexture) delete skyTexture;
	skyTexture = tex;
}

CubeMap* AssetManager::getSkyTexture() {
	return skyTexture;
}

void AssetManager::setEnvTexture(CubeMap* tex) {
	if (tex == envTexture) return;
	if (envTexture) delete envTexture;
	envTexture = tex;
}

CubeMap* AssetManager::getEnvTexture() {
	return envTexture;
}

void AssetManager::setNoise3D(CubeMap* tex) {
	if (tex == noise3DTexture) return;
	if (noise3DTexture) delete noise3DTexture;
	noise3DTexture = tex;
}

CubeMap* AssetManager::getNoise3D() {
	return noise3DTexture;
}

void AssetManager::setReflectTexture(Texture2D* tex) {
	reflectTexture = tex;
}

Texture2D* AssetManager::getReflectTexture() {
	return reflectTexture;
}

void AssetManager::addDistortionTex(const char* texName) {
	if (distortionTex < 0)
		addTextureBindless(texName, false);
	distortionTex = findTextureBindless(texName);
}

void AssetManager::addNoiseTex(const char* texName) {
	if (noiseTex < 0)
		addTextureBindless(texName, false, WRAP_MIRROR);
	noiseTex = findTextureBindless(texName);
}

void AssetManager::addRoadTex(const char* texName) {
	if (roadTex < 0)
		addTextureBindless(texName, false);
	roadTex = findTextureBindless(texName);
}

void AssetManager::createHeightTex() {
	map<string, Mesh*>::iterator it = meshes.find("terrain");
	if (it == meshes.end()) return;
	Terrain* mesh = (Terrain*)it->second;

	int size = MAP_SIZE / STEP_SIZE;
	byte* heightData = (byte*)malloc(size * size * 1 * sizeof(byte));
	byte* normalData = (byte*)malloc(size * size * 3 * sizeof(byte));
	for (uint i = 0; i < size * size; ++i) {
		heightData[i] = (byte)mesh->vertices3[i].GetY();
		vec3 normal = (mesh->normals[i].GetNormalized() + 1.0) * 0.5 * 255.0;
		SetUVec3(normal, normalData, i);
	}
	heightTexture = new Texture2D(size, size, false, TEXTURE_TYPE_COLOR, LOW_PRE, 1, NEAREST, true, heightData);
	heightNormalTex = new Texture2D(size, size, false, TEXTURE_TYPE_COLOR, LOW_PRE, 3, LINEAR, true, normalData);
	free(heightData);
	free(normalData);
}

void AssetManager::addMesh(const char* name, Mesh* mesh, bool billboard, bool drawShadow) {
	mesh->setName(name);
	meshes[name] = mesh;
	meshes[name]->setIsBillboard(billboard);
	meshes[name]->drawShadow = drawShadow;
}

Animation* AssetManager::exportAnimation(const char* name, Animation* animation) {
	animations[name] = animation;
	animation->setName(name);
	animation->exportAnims("animation");
	return animation;
}

void AssetManager::addAnimationData(const char* name, const char* path, Animation* animation) {
	AnimFrame* animData = new AnimFrame(name);
	frames->readAnimationData(path, animData);
	frames->addAnimationData(animData, animation);
	animationDatas[animData->getName()] = animData;
}

void AssetManager::initFrames() {
	frames->init();
}

void AssetManager::Init() {
	if (!AssetManager::assetManager) {
		InitImageLoaders(); // Init freeimage
		AssetManager::assetManager = new AssetManager();

		// Load some basic meshes
		AssetManager::assetManager->addMesh("box", new Box());
		AssetManager::assetManager->addMesh("sphere", new Sphere(16, 16));
		AssetManager::assetManager->addMesh("board", new Board());
		AssetManager::assetManager->addMesh("quad", new Quad());
		AssetManager::assetManager->addMesh("billboard", new Board(1, 1, 1, 0, 0), true, false);
	}
}

void AssetManager::Release() {
	if (AssetManager::assetManager) {
		ReleaseImageLoaders(); // Release freeimage
		delete AssetManager::assetManager;
	}
	AssetManager::assetManager = NULL;
}
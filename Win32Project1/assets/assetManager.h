#ifndef ASSETMANAGER_H_
#define ASSETMANAGER_H_

#define COMMON_TEXTURE "texture/common"

#include "../mesh/mesh.h"
#include "../animation/animation.h"
#include "../texture/imageset.h"

class AssetManager {
public:
	static AssetManager* assetManager;
public:
	static void Init();
	static void Release();
public:
	std::map<std::string, Mesh*> meshes;
	std::map<std::string, Animation*> animations;
	ImageSet* textures;
private:
	AssetManager();
	~AssetManager();
public:
	void initTextureArray();
};

#endif
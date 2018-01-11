#ifndef ASSETMANAGER_H_
#define ASSETMANAGER_H_

#define COMMON_TEXTURE "texture/common"

#include "../mesh/mesh.h"
#include "../animation/animation.h"
#include "../texture/imageset.h"

class AssetManager {
public:
	std::map<std::string, Mesh*> meshes;
	std::map<std::string, Animation*> animations;
	ImageSet* textures;
public:
	AssetManager();
	~AssetManager();
	void initTextureArray();
};

extern AssetManager* assetManager;
void InitAssetManager();
void ReleaseAssetManager();

#endif
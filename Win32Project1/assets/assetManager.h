#ifndef ASSETMANAGER_H_
#define ASSETMANAGER_H_

#define COMMON_TEXTURE "texture/common"

#include "../mesh/mesh.h"
#include "../animation/animation.h"
#include "../texture/texturebindless.h"
#include "../texture/cubemap.h"
#include "../texture/texture2d.h"

class AssetManager {
public:
	static AssetManager* assetManager;
public:
	static void Init();
	static void Release();
public:
	std::map<std::string, Mesh*> meshes;
	std::map<std::string, Animation*> animations;
	TextureBindless* texBld;
	CubeMap* skyTexture;
	Texture2D* reflectTexture;
private:
	AssetManager();
	~AssetManager();
public:
	void addMesh(const char* name, Mesh* mesh, bool billboard = false);
	void addAnimation(const char* name, Animation* animation);
	void addTextureBindless(const char* name, bool srgb);
	void initTextureBindless(MaterialManager* mtls);
	int findTextureBindless(const char* name);
	void setSkyTexture(CubeMap* tex);
	CubeMap* getSkyTexture();
	void setReflectTexture(Texture2D* tex);
	Texture2D* getReflectTexture();
};

#endif
#ifndef FBX_LOADER_H_
#define FBX_LOADER_H_

#include "animation.h"
#include "fbxutil.h"

class FBXLoader : public Animation {
private:
	FbxManager* manager;
	FbxImporter* importer;
	FbxScene* scene;
	FbxNode* root;
private:
	FbxTime frameTime;
public:
	int indexCount;
private: // Temp data
	int baseVertex, curFid;
	std::map<int, float> durationMap; // aid duration
	std::map<int, std::vector<int>> indexMap; // old index to new indices
	FbxArray<FbxString*> mAnimStackNameArray;
	FbxAMatrix parentMatrix;
private:
	FbxTexture* findTexture(FbxSurfaceMaterial* pSurfaceMaterial);
	FbxPose* getPose(int pid) { return scene->GetPose(pid); }
	void caculateParent(FbxNode* pNode, FbxTime pTime, FbxAMatrix& pParentGlobalPosition, FbxPose* pPose);
private:
	void init(const char* path);
	void loadMeshData(FbxNode* pNode);
	void loadMaterial(FbxNode* pNode, std::vector<uint>& mats);
	void loadMesh(FbxNode* pNode, std::vector<uint> mats);
	void loadBone(FbxNode* pNode);
	void loadAnimationData(FbxNode* pNode);
	void loadAnimation(FbxNode* pNode, FbxTime pTime, Frame* frame);
	void loadFrames(FbxNode* pNode, FbxTime pTime, Frame* frame);
	FbxAMatrix loadFrame(FbxNode* pNode, FbxCluster* pCluster, FbxTime pTime, FbxAMatrix pGlobalPosition, FbxPose* pPose);
public:
	FBXLoader(const char* path);
	virtual ~FBXLoader();
	virtual float getBoneFrame(int animIndex, float time, bool& end);
};

#endif

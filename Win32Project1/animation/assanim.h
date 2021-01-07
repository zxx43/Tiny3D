/*
 * assanim.h
 *
 *  Created on: 2017-6-24
 *      Author: a
 */

#ifndef ASSANIM_H_
#define ASSANIM_H_

#include "animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>

struct Entry {
	aiMesh* mesh;
	int baseVertex;
	int materialIndex;
};

struct BoneInfo {
	aiMatrix4x4 offset;
	aiMatrix4x4 transformation;
};

class AssAnim : public Animation {
private:
	Assimp::Importer importer;
	const aiScene* scene;
	Entry* entrys;
	std::map<std::string,int> boneMap;
	std::vector<BoneInfo*> boneInfos;
	aiMatrix4x4 rootToModelMat;
	std::map<std::string,aiNodeAnim*>* channelMaps;
	std::map<int, int> materialMap;
private:
	void loadModel();
	void loadMaterials();
	void loadMeshes(Entry* entry);
	void loadBones(aiMesh* mesh,int meshIndex);
	void pushWeightToVertex(int vertexid,int boneid,float weight);
	void initChannels();
	int findPositionIndex(aiNodeAnim* anim,float animTime);
	int findRotationIndex(aiNodeAnim* anim,float animTime);
	int findScaleIndex(aiNodeAnim* anim,float animTime);
	void calcPosition(aiNodeAnim* anim, float animTime, aiVector3D& position);
	void calcRotation(aiNodeAnim* anim, float animTime, aiQuaternion& rotation);
	void calcScale(aiNodeAnim* anim, float animTime, aiVector3D& scale);
	aiNodeAnim* findNodeAnim(int animIndex,std::string boneName);
	void readNode(int animIndex,float animTime,aiNode* node,const aiMatrix4x4& parentTransform);
	void prepareFrameData(int animIndex, aiAnimation* asAnimation, AnimFrame* animation);
public:
	AssAnim(const char* path);
	virtual ~AssAnim();

};

#endif /* ASSANIM_H_ */

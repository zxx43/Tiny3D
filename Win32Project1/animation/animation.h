/*
 * animation.h
 *
 *  Created on: 2017-6-24
 *      Author: a
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <vector>
#include <map>
#include <string>
#include "../maths/Maths.h"
#include "../material/materialManager.h"
#include <stdlib.h>
#include <string.h>

struct Entry {
	aiMesh* mesh;
	int baseVertex;
	int materialIndex;
};

struct BoneInfo {
	aiMatrix4x4 offset;
	aiMatrix4x4 transformation;
};

struct Frame {
	int boneCount;
	float* data;
	Frame(int bc) {
		boneCount = bc;
		data = (float*)malloc(boneCount * 12 * sizeof(float));
	}
	~Frame() {
		free(data);
	}
};

struct AnimFrame {
	std::vector<Frame*> frames;
	AnimFrame() {
		frames.clear();
	}
	~AnimFrame() {
		for (unsigned int i = 0; i < frames.size(); i++)
			delete frames[i];
		frames.clear();
	}
};

class Animation {
private:
	Assimp::Importer importer;
	const aiScene* scene;
	Entry* entrys;
	std::map<std::string,int> boneMap;
	std::vector<BoneInfo*> boneInfos;
	aiMatrix4x4 rootToModelMat;
	std::map<std::string,aiNodeAnim*>* channelMaps;

	void loadModel();
	void loadMaterials();
	void loadMeshes(Entry* entry);
	void loadBones(aiMesh* mesh,int meshIndex);
	void pushWeightToVertex(int vertexid,int boneid,float weight);
	void initChannels();
	int findPositionIndex(aiNodeAnim* anim,float animTime);
	int findRotationIndex(aiNodeAnim* anim,float animTime);
	int findScaleIndex(aiNodeAnim* anim,float animTime);
	void calcPosition(aiNodeAnim* anim,float animTime,aiVector3D& position);
	void calcRotation(aiNodeAnim* anim,float animTime,aiQuaternion& rotation);
	void calcScale(aiNodeAnim* anim,float animTime,aiVector3D& scale);
	aiNodeAnim* findNodeAnim(int animIndex,std::string boneName);
	void readNode(int animIndex,float animTime,aiNode* node,const aiMatrix4x4& parentTransform);

public:
	int faceCount,vertCount,boneCount;
	std::vector<VECTOR3D> aVertices;
	std::vector<VECTOR3D> aNormals;
	std::vector<VECTOR2D> aTexcoords;
	std::vector<VECTOR4D> aTextures;
	std::vector<VECTOR3D> aAmbients;
	std::vector<VECTOR3D> aDiffuses;
	std::vector<VECTOR3D> aSpeculars;
	std::vector<int> aIndices;
	std::vector<VECTOR4D> aBoneids;
	std::vector<VECTOR4D> aWeights;
	std::map<int,int> materialMap;
	float* boneTransformMats;

	int animCount;
	AnimFrame** animFrames;

	Animation(const char* path);
	~Animation();
	void bonesTransform(int animIndex,float time);
private:
	void prepareFrameData(int animIndex);
};

#endif /* ANIMATION_H_ */

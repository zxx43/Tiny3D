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
	void calcPosition(aiNodeAnim* anim, float animTime, aiVector3D& position);
	void calcRotation(aiNodeAnim* anim, float animTime, aiQuaternion& rotation);
	void calcScale(aiNodeAnim* anim, float animTime, aiVector3D& scale);
	aiNodeAnim* findNodeAnim(int animIndex,std::string boneName);
	void readNode(int animIndex,float animTime,aiNode* node,const aiMatrix4x4& parentTransform);

public:
	int faceCount,vertCount,boneCount;
	std::vector<vec3> aVertices;
	std::vector<vec3> aNormals;
	std::vector<vec3> aTangents;
	std::vector<vec2> aTexcoords;
	std::vector<Material*> aTextures;
	std::vector<vec3> aAmbients;
	std::vector<vec3> aDiffuses;
	std::vector<vec3> aSpeculars;
	std::vector<int> aIndices;
	std::vector<vec4> aBoneids;
	std::vector<vec4> aWeights;
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

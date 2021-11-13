#include "assanim.h"
#include <assimp/postprocess.h>
#include "../assets/assetManager.h"

AssAnim::AssAnim(const char* path) : Animation() {
	boneMap.clear();
	boneInfos.clear();
	materialMap.clear();

	scene=importer.ReadFile(path,
				aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	loadModel();

	for (int ai = 0; ai < scene->mNumAnimations; ai++) {
		aiAnimation* asAnimation = scene->mAnimations[ai];

		AnimFrame* animation = new AnimFrame(asAnimation->mName.C_Str());
		printf("assmip aname:%s\n", animation->getName().data());
		datasToExport.push_back(animation);
		prepareFrameData(ai, asAnimation, animation);
	}
}

AssAnim::~AssAnim() {
	delete[] entrys;

	boneMap.clear();
	for(unsigned int i=0;i<boneInfos.size();i++)
		free(boneInfos[i]);
	boneInfos.clear();

	for(unsigned int i=0;i<scene->mNumAnimations;i++)
		channelMaps[i].clear();
	delete[] channelMaps;

	importer.FreeScene();
	materialMap.clear();
}

void AssAnim::loadModel() {
	loadMaterials();
	int meshCount=scene->mNumMeshes;
	entrys=new Entry[meshCount];
	int base=0;
	for(int i=0;i<meshCount;i++) {
		aiMesh* mesh=scene->mMeshes[i];
		entrys[i].mesh=mesh;
		entrys[i].materialIndex=mesh->mMaterialIndex;
		entrys[i].baseVertex=base;
		base+=mesh->mNumVertices;
		loadMeshes(&entrys[i]);
	}

	aBoneids.resize(vertCount);
	aWeights.resize(vertCount);
	for(int i=0;i<vertCount;i++) {
		aBoneids[i]=vec4(0,0,0,0);
		aWeights[i]=vec4(0,0,0,0);
	}
	for(int i=0;i<meshCount;i++) {
		aiMesh* mesh=scene->mMeshes[i];
		loadBones(mesh,i);
	}

	rootToModelMat=scene->mRootNode->mTransformation;
	rootToModelMat=rootToModelMat.Inverse();

	channelMaps=new std::map<std::string,aiNodeAnim*>[scene->mNumAnimations];
	initChannels();
}

void AssAnim::initChannels() {
	for(unsigned int i=0;i<scene->mNumAnimations;i++) {
		aiAnimation* animation=scene->mAnimations[i];
		std::map<std::string,aiNodeAnim*> channelMap;
		channelMap.clear();
		for(unsigned int j=0;j<animation->mNumChannels;j++) {
			aiNodeAnim* nodeAnim=animation->mChannels[j];
			std::string boneName(nodeAnim->mNodeName.data);
			channelMap[boneName]=nodeAnim;
		}
		channelMaps[i]=channelMap;
	}
}

void AssAnim::loadBones(aiMesh* mesh,int meshIndex) {
	for(unsigned int i=0;i<mesh->mNumBones;i++) {
		aiBone* bone=mesh->mBones[i];
		std::string boneName=bone->mName.data;
		int boneIndex=0;
		if(boneMap.find(boneName)==boneMap.end()) {
			boneIndex=boneCount;
			boneCount++;

			BoneInfo* boneInfo=(BoneInfo*)malloc(sizeof(BoneInfo));
			boneInfo->offset=bone->mOffsetMatrix;
			boneInfos.push_back(boneInfo);
			boneMap[boneName]=boneIndex;
		} else
			boneIndex=boneMap[boneName];

		for(unsigned int j=0;j<bone->mNumWeights;j++) {
			aiVertexWeight boneWeight=bone->mWeights[j];
			int vertexid=entrys[meshIndex].baseVertex+boneWeight.mVertexId;
			float weight=boneWeight.mWeight;
			pushWeightToVertex(vertexid,boneIndex,weight);
		}
	}
}

void AssAnim::pushWeightToVertex(int vertexid,int boneid,float weight) {
	if(aWeights[vertexid].x==0) {
		aWeights[vertexid].x=weight;
		aBoneids[vertexid].x=boneid;
	} else if(aWeights[vertexid].y==0) {
		aWeights[vertexid].y=weight;
		aBoneids[vertexid].y=boneid;
	} else if(aWeights[vertexid].z==0) {
		aWeights[vertexid].z=weight;
		aBoneids[vertexid].z=boneid;
	} else if(aWeights[vertexid].w==0) {
		aWeights[vertexid].w=weight;
		aBoneids[vertexid].w=boneid;
	}
}

void AssAnim::loadMaterials() {
	int matCount=scene->mNumMaterials;
	for(int i=0;i<matCount;i++) {
		aiMaterial* mat=scene->mMaterials[i];
		aiString path,name;
		aiColor4D ambent(0, 0, 0, 1), diffuse(0, 0, 0, 1), specular(0, 0, 0, 1);
		if(mat->Get(AI_MATKEY_NAME, name)!=AI_SUCCESS) name="animation_mat";
		Material* mtl = new Material(name.data);
		if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			mtl->tex1 = convertTexPath(path.data).data();
		if (mat->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			mtl->tex2 = convertTexPath(path.data).data();
		if (mat->GetTexture(aiTextureType_SPECULAR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			mtl->tex3 = convertTexPath(path.data).data();
		if (mat->GetTexture(aiTextureType_AMBIENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			mtl->tex4 = convertTexPath(path.data).data();
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &ambent);
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);
		if (ambent.r < 0.2) ambent.r = 0.2;
		mtl->ambient.x = ambent.r; mtl->ambient.y = ambent.g; mtl->ambient.z = ambent.b;
		mtl->diffuse.x = diffuse.r; mtl->diffuse.y = diffuse.g; mtl->diffuse.z = diffuse.b;
		mtl->specular.x = specular.r; mtl->specular.y = specular.g; mtl->specular.z = specular.b;
		materialMap[i] = MaterialManager::materials->add(mtl);
	}
}

void AssAnim::loadMeshes(Entry* entry) {
	aiMesh* aMesh=entry->mesh;
	int aVertCount=aMesh->mNumVertices;
	vertCount+=aVertCount;
	for(int i=0;i<aVertCount;i++) {
		aiVector3D* vertex = &aMesh->mVertices[i];
		aiVector3D* normal = &aMesh->mNormals[i];
		aVertices.push_back(vec3(vertex->x, vertex->y, vertex->z));
		aNormals.push_back(vec3(normal->x, normal->y, normal->z));
		if (aMesh->HasTangentsAndBitangents()) {
			aiVector3D* tangent = &aMesh->mTangents[i];
			aTangents.push_back(vec3(tangent->x, tangent->y, tangent->z));
		}
		if(aMesh->HasTextureCoords(0)) {
			aiVector3D* texcoord=&aMesh->mTextureCoords[0][i];
			aTexcoords.push_back(vec2(texcoord->x,texcoord->y));
		} else
			aTexcoords.push_back(vec2(0,0));

		int mid = materialMap[entry->materialIndex];
		Material* mat = MaterialManager::materials->find(mid);
		if (!mat) {
			mid = 0;
			mat = MaterialManager::materials->find(mid);
		}
		aMids.push_back(mid);
		aTextures.push_back(mat);
		aAmbients.push_back(mat->ambient);
		aDiffuses.push_back(mat->diffuse);
		aSpeculars.push_back(mat->specular);
	}

	int aFaceCount=aMesh->mNumFaces;
	faceCount+=aFaceCount;
	for(int i=0;i<aFaceCount;i++) {
		aiFace face=aMesh->mFaces[i];
		int index1=entry->baseVertex+face.mIndices[0];
		int index2=entry->baseVertex+face.mIndices[1];
		int index3=entry->baseVertex+face.mIndices[2];

		aIndices.push_back(index1);
		aIndices.push_back(index2);
		aIndices.push_back(index3);
	}
}



int AssAnim::findPositionIndex(aiNodeAnim* anim,float animTime) {
	for(unsigned int i=0;i<anim->mNumPositionKeys-1;i++) {
		if(animTime<anim->mPositionKeys[i+1].mTime)
			return i;
	}
	return 0;
}

int AssAnim::findRotationIndex(aiNodeAnim* anim,float animTime) {
	for(unsigned int i=0;i<anim->mNumRotationKeys-1;i++) {
		if(animTime<anim->mRotationKeys[i+1].mTime)
			return i;
	}
	return 0;
}

int AssAnim::findScaleIndex(aiNodeAnim* anim,float animTime) {
	for(unsigned int i=0;i<anim->mNumScalingKeys-1;i++) {
		if(animTime<anim->mScalingKeys[i+1].mTime)
			return i;
	}
	return 0;
}

void AssAnim::calcPosition(aiNodeAnim* anim, float animTime, aiVector3D& position) {
	if (anim->mNumPositionKeys == 1) {
		position = anim->mPositionKeys[0].mValue;
		return;
	}

	int startId = findPositionIndex(anim, animTime);
	int endId = startId + 1;
	aiVectorKey startKey = anim->mPositionKeys[startId];
	aiVectorKey endKey = anim->mPositionKeys[endId];
	double dKeyTime = (endKey.mTime - startKey.mTime);
	double dTime = (double)animTime - startKey.mTime;
	double factor = dTime / dKeyTime;
	aiVector3D startPosition = startKey.mValue;
	aiVector3D endPosition = endKey.mValue;
	aiVector3D dPosition = endPosition - startPosition;
	aiVector3D sPosition(factor * dPosition.x, factor * dPosition.y, factor * dPosition.z);
	position = startPosition + sPosition;
}

void AssAnim::calcRotation(aiNodeAnim* anim,float animTime,aiQuaternion& rotation) {
	if(anim->mNumRotationKeys==1) {
		rotation = anim->mRotationKeys[0].mValue.Normalize();
		return;
	}

	int startId=findRotationIndex(anim,animTime);
	int endId=startId+1;
	aiQuatKey startKey=anim->mRotationKeys[startId];
	aiQuatKey endKey=anim->mRotationKeys[endId];
	double dKeyTime=endKey.mTime-startKey.mTime;
	double dTime=(double)animTime-startKey.mTime;
	double factor=dTime/dKeyTime;
	aiQuaternion startRotation=startKey.mValue;
	aiQuaternion endRotation=endKey.mValue;
	aiQuaternion::Interpolate(rotation,startRotation,endRotation,factor);
	rotation=rotation.Normalize();
}

void AssAnim::calcScale(aiNodeAnim* anim, float animTime, aiVector3D& scale) {
	if (anim->mNumScalingKeys == 1) {
		scale = anim->mScalingKeys[0].mValue;
		return;
	}

	int startId = findScaleIndex(anim, animTime);
	int endId = startId + 1;
	aiVectorKey startKey = anim->mScalingKeys[startId];
	aiVectorKey endKey = anim->mScalingKeys[endId];
	double dKeyTime = endKey.mTime - startKey.mTime;
	double dTime = (double)animTime - startKey.mTime;
	double factor = dTime / dKeyTime;
	aiVector3D startScale = startKey.mValue;
	aiVector3D endScale = endKey.mValue;
	aiVector3D dScale = endScale - startScale;
	aiVector3D sScale(factor * dScale.x, factor * dScale.y, factor * dScale.z);
	scale = startScale + sScale;
}

aiNodeAnim* AssAnim::findNodeAnim(int animIndex,std::string boneName) {
	std::map<std::string,aiNodeAnim*> channelMap=channelMaps[animIndex];
	if(channelMap.find(boneName)!=channelMap.end()) {
		aiNodeAnim* nodeAnim=channelMap[boneName];
		return nodeAnim;
	}
	return NULL;
}

void AssAnim::readNode(int animIndex,float animTime,aiNode* node,const aiMatrix4x4& parentTransform) {
	std::string boneName(node->mName.data);
	aiMatrix4x4 boneTransform=node->mTransformation;
	aiNodeAnim* boneAnim=findNodeAnim(animIndex,boneName);
	if(boneAnim) {
		aiVector3D scale;
		calcScale(boneAnim,animTime,scale);
		aiMatrix4x4 scaleMat;
		aiMatrix4x4::Scaling(scale,scaleMat);

		aiQuaternion rotation;
		calcRotation(boneAnim,animTime,rotation);
		aiMatrix4x4 rotateMat(rotation.GetMatrix());

		aiVector3D position;
		calcPosition(boneAnim,animTime,position);
		aiMatrix4x4 translateMat;
		aiMatrix4x4::Translation(position,translateMat);

		boneTransform=translateMat*rotateMat*scaleMat;
	}

	aiMatrix4x4 currentBoneTransform=parentTransform*boneTransform;
	if(boneMap.find(boneName)!=boneMap.end()) {
		int boneIndex=boneMap[boneName];
		BoneInfo* boneInfo=boneInfos[boneIndex];
		aiMatrix4x4 aTransform=rootToModelMat*currentBoneTransform*boneInfo->offset;
		boneInfo->transformation=aTransform;
	}

	for(unsigned int i=0;i<node->mNumChildren;i++) {
		aiNode* childNode=node->mChildren[i];
		readNode(animIndex,animTime,childNode,currentBoneTransform);
	}
}

void AssAnim::prepareFrameData(int animIndex, aiAnimation* asAnimation, AnimFrame* animation) {
	aiMatrix4x4 mat;
	MATRIX4X4 mat4;

	for (float tick = 0.0; tick < asAnimation->mDuration; tick += 0.01) {
		Frame* frame = new Frame(boneCount);
		int currIndex = 0;
		readNode(animIndex, tick, scene->mRootNode, mat);
		for (int bi = 0; bi < boneCount; bi++) {
			mat4.entries[0] = boneInfos[bi]->transformation.a1;
			mat4.entries[1] = boneInfos[bi]->transformation.b1;
			mat4.entries[2] = boneInfos[bi]->transformation.c1;
			mat4.entries[3] = boneInfos[bi]->transformation.d1;
			mat4.entries[4] = boneInfos[bi]->transformation.a2;
			mat4.entries[5] = boneInfos[bi]->transformation.b2;
			mat4.entries[6] = boneInfos[bi]->transformation.c2;
			mat4.entries[7] = boneInfos[bi]->transformation.d2;
			mat4.entries[8] = boneInfos[bi]->transformation.a3;
			mat4.entries[9] = boneInfos[bi]->transformation.b3;
			mat4.entries[10] = boneInfos[bi]->transformation.c3;
			mat4.entries[11] = boneInfos[bi]->transformation.d3;
			mat4.entries[12] = boneInfos[bi]->transformation.a4;
			mat4.entries[13] = boneInfos[bi]->transformation.b4;
			mat4.entries[14] = boneInfos[bi]->transformation.c4;
			mat4.entries[15] = boneInfos[bi]->transformation.d4;
			mat4.Transpose();
			for (int m = 0; m < 12; m++) 
				frame->data[currIndex++] = mat4.entries[m];
		}
		animation->frames.push_back(frame);
	}
	animation->setDuration(asAnimation->mDuration);
	animation->setTicksPerSecond(asAnimation->mTicksPerSecond);
}

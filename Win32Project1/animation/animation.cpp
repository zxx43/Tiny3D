#include "animation.h"
#include <assimp/postprocess.h>
#include "../assets/assetManager.h"

Animation::Animation(const char* path) {
	scene=importer.ReadFile(path,
				aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	vertCount=0;
	faceCount=0;
	boneCount=0;
	boneTransformMats=NULL;
	aVertices.clear();
	aNormals.clear();
	aTexcoords.clear();
	aTextures.clear();
	aAmbients.clear();
	aDiffuses.clear();
	aSpeculars.clear();
	aIndices.clear();
	aBoneids.clear();
	aWeights.clear();

	loadModel();
}

Animation::~Animation() {
	aVertices.clear();
	aNormals.clear();
	aTexcoords.clear();
	aTextures.clear();
	aAmbients.clear();
	aDiffuses.clear();
	aSpeculars.clear();
	aIndices.clear();
	aBoneids.clear();
	aWeights.clear();
	materialMap.clear();
	delete[] entrys;

	boneMap.clear();
	for(unsigned int i=0;i<boneInfos.size();i++)
		free(boneInfos[i]);
	boneInfos.clear();
	if(boneTransformMats) {
		delete[] boneTransformMats;
		boneTransformMats=NULL;
	}

	for(unsigned int i=0;i<scene->mNumAnimations;i++)
		channelMaps[i].clear();
	delete[] channelMaps;

	importer.FreeScene();
}

void Animation::loadModel() {
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
		aBoneids[i]=VECTOR4D(0,0,0,0);
		aWeights[i]=VECTOR4D(0,0,0,0);
	}
	for(int i=0;i<meshCount;i++) {
		aiMesh* mesh=scene->mMeshes[i];
		loadBones(mesh,i);
	}

	boneTransformMats=new float[boneCount*16];
	rootToModelMat=scene->mRootNode->mTransformation;
	rootToModelMat=rootToModelMat.Inverse();

	channelMaps=new std::map<std::string,aiNodeAnim*>[scene->mNumAnimations];
	initChannels();
}

void Animation::initChannels() {
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

void Animation::loadBones(aiMesh* mesh,int meshIndex) {
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

void Animation::pushWeightToVertex(int vertexid,int boneid,float weight) {
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

void Animation::loadMaterials() {
	int matCount=scene->mNumMaterials;
	for(int i=0;i<matCount;i++) {
		aiMaterial* mat=scene->mMaterials[i];
		aiString path,name;
		aiColor4D ambent(0, 0, 0, 1), diffuse(0, 0, 0, 1), specular(0, 0, 0, 1);
		if(mat->Get(AI_MATKEY_NAME, name)!=AI_SUCCESS) name="animation_mat";
		Material* mtl = new Material(name.data);
		if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			ImageSet* textures = assetManager->textures;
			if (textures->findTexture(path.data) < 0)
				textures->addTexture(path.data);
			int textureid = textures->findTexture(path.data);
			mtl->texture.x = textureid;
		}
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &ambent);
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);
		mtl->ambient.x = ambent.r; mtl->ambient.y = ambent.g; mtl->ambient.z = ambent.b;
		mtl->diffuse.x = diffuse.r; mtl->diffuse.y = diffuse.g; mtl->diffuse.z = diffuse.b;
		mtl->specular.x = specular.r; mtl->specular.y = specular.g; mtl->specular.z = specular.b;
		materialMap[i] = materials->add(mtl);
	}
}

void Animation::loadMeshes(Entry* entry) {
	aiMesh* aMesh=entry->mesh;
	int aVertCount=aMesh->mNumVertices;
	vertCount+=aVertCount;
	for(int i=0;i<aVertCount;i++) {
		aiVector3D* vertex=&aMesh->mVertices[i];
		aiVector3D* normal=&aMesh->mNormals[i];
		aVertices.push_back(VECTOR3D(vertex->x,vertex->y,vertex->z));
		aNormals.push_back(VECTOR3D(normal->x,normal->y,normal->z));
		if(aMesh->HasTextureCoords(0)) {
			aiVector3D* texcoord=&aMesh->mTextureCoords[0][i];
			aTexcoords.push_back(VECTOR2D(texcoord->x,texcoord->y));
		} else
			aTexcoords.push_back(VECTOR2D(0,0));

		int mid = materialMap[entry->materialIndex];
		Material* mat = materials->find(mid);
		if (!mat) mat = materials->find(0);
		aTextures.push_back(mat->texture);
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



int Animation::findPositionIndex(aiNodeAnim* anim,float animTime) {
	for(unsigned int i=0;i<anim->mNumPositionKeys-1;i++) {
		if(animTime<anim->mPositionKeys[i+1].mTime)
			return i;
	}
	return 0;
}

int Animation::findRotationIndex(aiNodeAnim* anim,float animTime) {
	for(unsigned int i=0;i<anim->mNumRotationKeys-1;i++) {
		if(animTime<anim->mRotationKeys[i+1].mTime)
			return i;
	}
	return 0;
}

int Animation::findScaleIndex(aiNodeAnim* anim,float animTime) {
	for(unsigned int i=0;i<anim->mNumScalingKeys-1;i++) {
		if(animTime<anim->mScalingKeys[i+1].mTime)
			return i;
	}
	return 0;
}

void Animation::calcPosition(aiNodeAnim* anim,float animTime,aiVector3D& position) {
	if(anim->mNumPositionKeys==1) {
		position=anim->mPositionKeys[0].mValue;
		return;
	}

	int startId=findPositionIndex(anim,animTime);
	int endId=startId+1;
	aiVectorKey startKey=anim->mPositionKeys[startId];
	aiVectorKey endKey=anim->mPositionKeys[endId];
	float dKeyTime=(float)(endKey.mTime-startKey.mTime);
	float dTime=animTime-(float)startKey.mTime;
	float factor=dTime/dKeyTime;
	aiVector3D startPosition=startKey.mValue;
	aiVector3D endPosition=endKey.mValue;
	aiVector3D dPosition=endPosition-startPosition;
	position=startPosition+factor*dPosition;
}

void Animation::calcRotation(aiNodeAnim* anim,float animTime,aiQuaternion& rotation) {
	if(anim->mNumRotationKeys==1) {
		rotation=anim->mRotationKeys[0].mValue;
		return;
	}

	int startId=findRotationIndex(anim,animTime);
	int endId=startId+1;
	aiQuatKey startKey=anim->mRotationKeys[startId];
	aiQuatKey endKey=anim->mRotationKeys[endId];
	float dKeyTime=(float)(endKey.mTime-startKey.mTime);
	float dTime=animTime-(float)startKey.mTime;
	float factor=dTime/dKeyTime;
	aiQuaternion startRotation=startKey.mValue;
	aiQuaternion endRotation=endKey.mValue;
	aiQuaternion::Interpolate(rotation,startRotation,endRotation,factor);
	rotation=rotation.Normalize();
}

void Animation::calcScale(aiNodeAnim* anim,float animTime,aiVector3D& scale) {
	if(anim->mNumScalingKeys==1) {
		scale=anim->mScalingKeys[0].mValue;
		return;
	}

	int startId=findScaleIndex(anim,animTime);
	int endId=startId+1;
	aiVectorKey startKey=anim->mScalingKeys[startId];
	aiVectorKey endKey=anim->mScalingKeys[endId];
	float dKeyTime=(float)(endKey.mTime-startKey.mTime);
	float dTime=animTime-(float)startKey.mTime;
	float factor=dTime/dKeyTime;
	aiVector3D startScale=startKey.mValue;
	aiVector3D endScale=endKey.mValue;
	aiVector3D dScale=endScale-startScale;
	scale=startScale+factor*dScale;
}

aiNodeAnim* Animation::findNodeAnim(int animIndex,std::string boneName) {
	std::map<std::string,aiNodeAnim*> channelMap=channelMaps[animIndex];
	if(channelMap.find(boneName)!=channelMap.end()) {
		aiNodeAnim* nodeAnim=channelMap[boneName];
		return nodeAnim;
	}
	return NULL;
}

void Animation::readNode(int animIndex,float animTime,aiNode* node,const aiMatrix4x4& parentTransform) {
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

void Animation::bonesTransform(int animIndex,float time) {
	aiAnimation* animation=scene->mAnimations[animIndex];
	float ticksPerSecond=(float)animation->mTicksPerSecond;
	float ticks=time*ticksPerSecond;
	float animTime=fmodf(ticks,(float)animation->mDuration);
	aiMatrix4x4 mat;
	readNode(animIndex,animTime,scene->mRootNode,mat);
	for(int i=0;i<boneCount;i++) {
		boneTransformMats[i*16]=boneInfos[i]->transformation.a1;
		boneTransformMats[i*16+1]=boneInfos[i]->transformation.b1;
		boneTransformMats[i*16+2]=boneInfos[i]->transformation.c1;
		boneTransformMats[i*16+3]=boneInfos[i]->transformation.d1;
		boneTransformMats[i*16+4]=boneInfos[i]->transformation.a2;
		boneTransformMats[i*16+5]=boneInfos[i]->transformation.b2;
		boneTransformMats[i*16+6]=boneInfos[i]->transformation.c2;
		boneTransformMats[i*16+7]=boneInfos[i]->transformation.d2;
		boneTransformMats[i*16+8]=boneInfos[i]->transformation.a3;
		boneTransformMats[i*16+9]=boneInfos[i]->transformation.b3;
		boneTransformMats[i*16+10]=boneInfos[i]->transformation.c3;
		boneTransformMats[i*16+11]=boneInfos[i]->transformation.d3;
		boneTransformMats[i*16+12]=boneInfos[i]->transformation.a4;
		boneTransformMats[i*16+13]=boneInfos[i]->transformation.b4;
		boneTransformMats[i*16+14]=boneInfos[i]->transformation.c4;
		boneTransformMats[i*16+15]=boneInfos[i]->transformation.d4;
	}
}

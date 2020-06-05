#include "fbxloader.h"

FBXLoader::FBXLoader(const char* path) : Animation() {
	manager = FbxManager::Create();
	importer = NULL;

	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	FbxString lPath = FbxGetApplicationDirectory();
	manager->LoadPluginsDirectory(lPath.Buffer());

	scene = FbxScene::Create(manager, "FBX Scene");

	indexCount = 0;
	baseVertex = 0;
	curFid = 0;
	durationMap.clear();
	indexMap.clear();
	mAnimStackNameArray.Clear();

	init(path);
	inverseYZ = true;
}

FBXLoader::~FBXLoader() {
	FbxArrayDelete(mAnimStackNameArray);
	if (importer) importer->Destroy();
	scene->Destroy();
	manager->Destroy();
	durationMap.clear();
	indexMap.clear();
}

void FBXLoader::init(const char* path) {
	importer = FbxImporter::Create(manager, "");
	importer->Initialize(path, -1, manager->GetIOSettings());
	if (!importer->Import(scene)) {
		printf("Import fbx error!\n");
		return;
	}
	animCount = importer->GetAnimStackCount();
	animFrames = new AnimFrame*[animCount];
	scene->FillAnimStackNameArray(mAnimStackNameArray);
	frameTime.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());
	
	root = scene->GetRootNode();
	loadMeshData(root);
	loadAnimationData(root);
}

void FBXLoader::loadMeshData(FbxNode* pNode) {
	if (pNode) {
		FbxNodeAttribute::EType attr;
		if (pNode->GetNodeAttribute()) {
			attr = pNode->GetNodeAttribute()->GetAttributeType();
			if (attr == FbxNodeAttribute::eMesh) {
				std::vector<uint> mats;
				loadMaterial(pNode, mats);
				loadMesh(pNode, mats);
				loadBone(pNode);
				baseVertex = vertCount;
			} 
		}
		for (int i = 0; i < pNode->GetChildCount(); ++i)
			loadMeshData(pNode->GetChild(i));
	}
}

void FBXLoader::loadAnimationData(FbxNode* pNode) {
	for (int aid = 0; aid < animCount; aid++) {
		FbxString* animName = mAnimStackNameArray[aid];
		FbxAnimStack* lCurrentAnimationStack = scene->FindMember<FbxAnimStack>(animName->Buffer());
		if (!lCurrentAnimationStack) continue;
		scene->SetCurrentAnimationStack(lCurrentAnimationStack);

		FbxTime start, stop;
		FbxTakeInfo* lCurrentTakeInfo = scene->GetTakeInfo(*animName);
		if (lCurrentTakeInfo) {
			start = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
			stop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
		}
		else {
			FbxTimeSpan lTimeLineTimeSpan;
			scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
			start = lTimeLineTimeSpan.GetStart();
			stop = lTimeLineTimeSpan.GetStop();
		}

		float duration = (stop - start).GetMilliSeconds() * 0.001;
		durationMap[aid] = duration;
		animFrames[aid] = new AnimFrame();

		for (FbxTime now = start; now < stop; now += frameTime) {
			Frame* frame = new Frame(boneCount);
			curFid = 0;
			parentMatrix.SetIdentity();
			loadAnimation(pNode, now, frame);
			animFrames[aid]->frames.push_back(frame);
		}
	}
}

void FBXLoader::loadAnimation(FbxNode* pNode, FbxTime pTime, Frame* frame) {
	if (pNode) {
		FbxNodeAttribute::EType attr;
		if (pNode->GetNodeAttribute()) {
			attr = pNode->GetNodeAttribute()->GetAttributeType();
			if (attr == FbxNodeAttribute::eMesh) {
				caculateParent(pNode, pTime, parentMatrix, NULL);
				loadFrames(pNode, pTime, frame);
			}
		}
		for (int i = 0; i < pNode->GetChildCount(); ++i)
			loadAnimation(pNode->GetChild(i), pTime, frame);
	}
}

void FBXLoader::loadMaterial(FbxNode* pNode, std::vector<uint>& mats) {
	int materialCount = pNode->GetMaterialCount();
	mats.resize(materialCount);
	for (int m = 0; m < materialCount; m++) {
		FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(m);

		Material* mtl = new Material(pSurfaceMaterial->GetName());
		if (pSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
			FbxSurfacePhong* phong = (FbxSurfacePhong*)pSurfaceMaterial;
			FbxDouble3 ambient = phong->Ambient;
			FbxDouble3 diffuse = phong->Diffuse;
			FbxDouble3 specular = phong->Specular;
			FbxTexture* texture = findTexture(phong);
			if (texture) {
				FbxFileTexture* texFile = FbxCast<FbxFileTexture>(texture);
				const char* texName = texFile->GetRelativeFileName();
				mtl->tex1 = convertTexPath(texName).data();
			}
			mtl->ambient = vec3(ambient[0], ambient[1], ambient[2]);
			mtl->diffuse = vec3(diffuse[0], diffuse[1], diffuse[2]);
			mtl->specular = vec3(specular[0], specular[1], specular[2]);
		} else if (pSurfaceMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)pSurfaceMaterial;
			FbxDouble3 ambient = lambert->Ambient;
			FbxDouble3 diffuse = lambert->Diffuse;
			FbxTexture* texture = findTexture(lambert);
			if (texture) {
				FbxFileTexture* texFile = FbxCast<FbxFileTexture>(texture);
				const char* texName = texFile->GetRelativeFileName();
				mtl->tex1 = convertTexPath(texName).data();
			}
			mtl->ambient = vec3(ambient[0], ambient[1], ambient[2]);
			mtl->diffuse = vec3(diffuse[0], diffuse[1], diffuse[2]);
			mtl->specular = vec3(0.0, 0.0, 0.0);
		}
		mats[m] = MaterialManager::materials->add(mtl);
	}
}

void FBXLoader::loadMesh(FbxNode* pNode, std::vector<uint> mats) {
	FbxMesh* pMesh = (FbxMesh*)pNode->GetNodeAttribute();

	int pointsCount = pMesh->GetControlPointsCount();
	int normalCount = pMesh->GetElementNormalCount();
	int tangentCount = pMesh->GetElementTangentCount();
	int uvCount = pMesh->GetElementUVCount();
	int polyCount = pMesh->GetPolygonCount();

	FbxVector4* lePoints = pMesh->GetControlPoints();

	int vertexId = 0, meshIndexCount = polyCount * 3;
	std::vector<vec3> innVertices; innVertices.resize(meshIndexCount);
	std::vector<vec3> innNormals; innNormals.resize(meshIndexCount);
	std::vector<vec3> innTangents; innTangents.resize(meshIndexCount);
	std::vector<vec2> innTexcoords; innTexcoords.resize(meshIndexCount);
	std::vector<Material*> innTextures; innTextures.resize(meshIndexCount);
	std::vector<vec3> innAmbients; innAmbients.resize(meshIndexCount);
	std::vector<vec3> innDiffuses; innDiffuses.resize(meshIndexCount);
	std::vector<vec3> innSpeculars; innSpeculars.resize(meshIndexCount);
	std::vector<vec4> innBoneids; innBoneids.resize(meshIndexCount);
	std::vector<vec4> innWeights; innWeights.resize(meshIndexCount);
	std::vector<int> innIndices; innIndices.resize(meshIndexCount);

	std::map<int, bool> texcoordMap; texcoordMap.clear();
	int dupCount = pointsCount;
	for (int polyIndex = 0; polyIndex < polyCount; polyIndex++) {
		int polySize = pMesh->GetPolygonSize(polyIndex);
		if (polySize != 3) {
			printf("Read Mesh Error: %d Not triangle!\n", polySize);
			system("pause");
			return;
		}
		for (int pointIndex = 0; pointIndex < polySize; pointIndex++) {
			int leIndex = pMesh->GetPolygonVertex(polyIndex, pointIndex);
			int innIndex = leIndex;
			
			for (int k = 0; k < uvCount; k++) {
				FbxGeometryElementUV* leTexcoord = pMesh->GetElementUV(k);
				FbxVector2 fuv;
				if (leTexcoord->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
					int mappingIndex = pMesh->GetTextureUVIndex(polyIndex, pointIndex);
					fuv = leTexcoord->GetDirectArray().GetAt(mappingIndex);
				}
				else if (leTexcoord->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
					int mappingIndex = leIndex;
					switch (leTexcoord->GetReferenceMode()) {
					case FbxGeometryElement::eDirect:
						fuv = leTexcoord->GetDirectArray().GetAt(mappingIndex);
						break;
					case FbxGeometryElement::eIndexToDirect:
						int id = leTexcoord->GetIndexArray().GetAt(mappingIndex);
						fuv = leTexcoord->GetDirectArray().GetAt(id);
						break;
					}
				}
				vec2 texcoord(fuv[0], fuv[1]);
				int oldIndex = baseVertex + innIndex;
				if (texcoordMap.find(innIndex) != texcoordMap.end() && innTexcoords[innIndex] != texcoord) {
					innIndex = dupCount++;
					int newIndex = baseVertex + innIndex;
					indexMap[oldIndex].push_back(newIndex);
				} else if (texcoordMap.find(innIndex) == texcoordMap.end()) {
					indexMap[oldIndex].clear();
					indexMap[oldIndex].push_back(oldIndex);
				}

				innTexcoords[innIndex] = texcoord;
				break;
			}
			texcoordMap[innIndex] = true;

			innBoneids[innIndex] = vec4(0.0, 0.0, 0.0, 0.0);
			innWeights[innIndex] = vec4(0.0, 0.0, 0.0, 0.0);

			int fbxIndex = baseVertex + innIndex;
			innIndices[polyIndex * polySize + pointIndex] = fbxIndex;

			FbxVector4 lePoint = lePoints[leIndex];
			vec3 vertex(lePoint[0], lePoint[1], lePoint[2]);
			innVertices[innIndex] = vertex;
			
			for (int k = 0; k < normalCount; k++) {
				FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal(k);
				FbxVector4 fnormal;
				int mappingIndex = leNormals->GetMappingMode() == FbxGeometryElement::eByPolygonVertex ? vertexId : leIndex;
				switch (leNormals->GetReferenceMode()) {
					case FbxGeometryElement::eDirect:
						fnormal = leNormals->GetDirectArray().GetAt(mappingIndex);
						break;
					case FbxGeometryElement::eIndexToDirect:
						int id = leNormals->GetIndexArray().GetAt(mappingIndex);
						fnormal = leNormals->GetDirectArray().GetAt(id);
						break;
				}
				vec3 normal(fnormal[0], fnormal[1], fnormal[2]);
				innNormals[innIndex] = normal;
				break;
			}
			for (int k = 0; k < tangentCount; k++) {
				FbxGeometryElementTangent* leTangents = pMesh->GetElementTangent(k);
				FbxVector4 ftangent;
				int mappingIndex = leTangents->GetMappingMode() == FbxGeometryElement::eByPolygonVertex ? vertexId : leIndex;
				switch (leTangents->GetReferenceMode()) {
					case FbxGeometryElement::eDirect:
						ftangent = leTangents->GetDirectArray().GetAt(mappingIndex);
						break;
					case FbxGeometryElement::eIndexToDirect:
						int id = leTangents->GetIndexArray().GetAt(mappingIndex);
						ftangent = leTangents->GetDirectArray().GetAt(id);
						break;
				}
				vec3 tangent(ftangent[0], ftangent[1], ftangent[2]);
				innTangents[innIndex] = tangent;
				break;
			}

			int mid = -1;
			FbxGeometryElementMaterial* leMaterial = pMesh->GetElementMaterial();
			if (leMaterial) {
				FbxLayerElementArrayTemplate<int>* materialIndices = &leMaterial->GetIndexArray();
				switch (leMaterial->GetMappingMode()) {
					case FbxGeometryElement::eByPolygon: {
						if (materialIndices->GetCount() == polyCount) {
							int materialIndex = materialIndices->GetAt(polyIndex);
							mid = mats[materialIndex];
						}
					}
					break;
					case FbxGeometryElement::eAllSame: {
						int materialIndex = materialIndices->GetAt(0);
						mid = mats[materialIndex];
					}
					break;
				}
			}

			Material* mat = (mid >= 0) ? MaterialManager::materials->find(mid) : NULL;
			if (!mat) mat = MaterialManager::materials->find(0);
			if (mat->ambient.x < 0.2) mat->ambient.x = 0.2;
			innTextures[innIndex] = mat;
			innAmbients[innIndex] = mat->ambient;
			innDiffuses[innIndex] = mat->diffuse;
			innSpeculars[innIndex] = mat->specular;

			vertexId++;
		}
	}
	indexCount += meshIndexCount;

	for (int i = 0; i < dupCount; i++) {
		aVertices.push_back(innVertices[i]);
		aNormals.push_back(innNormals[i]);
		aTangents.push_back(innTangents[i]);
		aTexcoords.push_back(innTexcoords[i]);
		aTextures.push_back(innTextures[i]);
		aAmbients.push_back(innAmbients[i]);
		aDiffuses.push_back(innDiffuses[i]);
		aSpeculars.push_back(innSpeculars[i]);
		aBoneids.push_back(innBoneids[i]);
		aWeights.push_back(innWeights[i]);
	}
	for(int i = 0; i < meshIndexCount; i++)
		aIndices.push_back(innIndices[i]);

	vertCount += dupCount;
	faceCount += polyCount;
}

void FBXLoader::loadBone(FbxNode* pNode) {
	FbxMesh* pMesh = (FbxMesh*)pNode->GetNodeAttribute();

	int deformerCount = pMesh->GetDeformerCount();
	for (int i = 0; i < deformerCount; i++) {
		FbxDeformer* deformer = pMesh->GetDeformer(i);
		if (deformer->GetDeformerType() != FbxDeformer::eSkin) continue;

		FbxSkin* skin = (FbxSkin*)deformer;
		int clusterCount = skin->GetClusterCount();
		for (int ci = 0; ci < clusterCount; ci++) {
			FbxCluster* cluster = skin->GetCluster(ci);
			int fbxIndCount = cluster->GetControlPointIndicesCount();
			int* fbxIndices = cluster->GetControlPointIndices();
			if (fbxIndCount <= 0) continue;
			float cid = boneCount++;
			double* pointWeights = cluster->GetControlPointWeights();
			for (int b = 0; b < fbxIndCount; b++) {
				int fbxIndex = baseVertex + fbxIndices[b];
				double pointWeight = pointWeights[b];

				for (int im = 0; im < indexMap[fbxIndex].size(); im++) {
					int index = indexMap[fbxIndex][im];
					if (aWeights[index].x < 0.0001) {
						aBoneids[index].x = cid;
						aWeights[index].x = pointWeight;
					} else if (aWeights[index].y < 0.0001) {
						aBoneids[index].y = cid;
						aWeights[index].y = pointWeight;
					} else if (aWeights[index].z < 0.0001) {
						aBoneids[index].z = cid;
						aWeights[index].z = pointWeight;
					} else if (aWeights[index].w < 0.0001) {
						aBoneids[index].w = cid;
						aWeights[index].w = pointWeight;
					}
				}
			}
		}
	}
}

FbxTexture* FBXLoader::findTexture(FbxSurfaceMaterial* pSurfaceMaterial) {
	int lTextureIndex = 0;
	FBXSDK_FOR_EACH_TEXTURE(lTextureIndex) {
		FbxProperty lProperty = pSurfaceMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
		int texCount = lProperty.GetSrcObjectCount<FbxTexture>();
		for (int ti = 0; ti < texCount; ti++) {
			FbxLayeredTexture* lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(ti);
			if (lLayeredTexture && lLayeredTexture->GetSrcObjectCount<FbxTexture>() > 0) {
				FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(0);
				return lTexture;
			} else if (!lLayeredTexture) {
				FbxTexture* lTexture = lProperty.GetSrcObject<FbxTexture>(ti);
				if (lTexture) return lTexture;
			}
		}
		break;
	}
	return NULL;
}

void FBXLoader::caculateParent(FbxNode* pNode, FbxTime pTime, FbxAMatrix& pParentGlobalPosition, FbxPose* pPose) {
	FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);
	pParentGlobalPosition = lGlobalPosition;
}

FbxAMatrix FBXLoader::loadFrame(FbxNode* pNode, FbxCluster* pCluster, FbxTime pTime, FbxAMatrix pGlobalPosition, FbxPose* pPose) {
	FbxAMatrix frame; frame.SetIdentity();
	if (pNode->GetNodeAttribute()) {
		FbxAMatrix lGeometryOffset = GetGeometry(pNode);
		FbxAMatrix lGlobalOffPosition = pGlobalPosition * lGeometryOffset;

		FbxMesh* lMesh = pNode->GetMesh();
		ComputeClusterDeformation(lGlobalOffPosition, lMesh, pCluster, frame, pTime, pPose);
	}
	return frame;
}

void FBXLoader::loadFrames(FbxNode* pNode, FbxTime pTime, Frame* frame) {
	FbxMesh* pMesh = (FbxMesh*)pNode->GetNodeAttribute();
	int deformerCount = pMesh->GetDeformerCount();

	for (int di = 0; di < deformerCount; di++) {
		FbxDeformer* deformer = pMesh->GetDeformer(di);
		if (deformer->GetDeformerType() != FbxDeformer::eSkin) continue;
		
		FbxSkin* skin = (FbxSkin*)deformer;
		int clusterCount = skin->GetClusterCount();
	
		for (int ci = 0; ci < clusterCount; ci++) {
			FbxCluster* cluster = skin->GetCluster(ci);
			int fbxIndCount = cluster->GetControlPointIndicesCount();
			if (fbxIndCount <= 0) continue;

			FbxPose* pose = NULL;
			FbxAMatrix fbxFrame = loadFrame(pNode, cluster, pTime, parentMatrix, pose);
			FbxVector4 r0 = fbxFrame.GetColumn(0);
			FbxVector4 r1 = fbxFrame.GetColumn(1);
			FbxVector4 r2 = fbxFrame.GetColumn(2);
			for (int ind = 0; ind < 4; ind++)
				frame->data[curFid++] = r0[ind];
			for (int ind = 0; ind < 4; ind++)
				frame->data[curFid++] = r1[ind];
			for (int ind = 0; ind < 4; ind++)
				frame->data[curFid++] = r2[ind];
		}
	}
}

float FBXLoader::getBoneFrame(int animIndex, float time, bool& end) {
	float ticksPerSecond = 15.0 / frameTime.GetMilliSeconds();
	float ticks = time * ticksPerSecond;
	float animTime = ticks;
	if (animTime > durationMap[animIndex]) {
		end = true;
		animTime = durationMap[animIndex];
	} else end = false;

	return animTime * 100.0;
}
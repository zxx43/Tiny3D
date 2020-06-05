#ifndef FBX_UTIL_H_
#define FBX_UTIL_H_

#ifndef FBXSDK_SHARED
#define FBXSDK_SHARED 1
#endif

#include <fbxsdk.h>

// Scale all the elements of a matrix.
inline void MatrixScale(FbxAMatrix& pMatrix, double pValue) {
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pMatrix[i][j] *= pValue;
		}
	}
}


// Add a value to all the elements in the diagonal of the matrix.
inline void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue) {
	pMatrix[0][0] += pValue;
	pMatrix[1][1] += pValue;
	pMatrix[2][2] += pValue;
	pMatrix[3][3] += pValue;
}


// Sum two matrices element by element.
inline void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix) {
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pDstMatrix[i][j] += pSrcMatrix[i][j];
		}
	}
}

FbxAMatrix GetGlobalPosition(FbxNode* pNode,
	const FbxTime& pTime,
	FbxPose* pPose = NULL,
	FbxAMatrix* pParentGlobalPosition = NULL);
FbxAMatrix GetPoseMatrix(FbxPose* pPose,
	int pNodeIndex);
FbxAMatrix GetGeometry(FbxNode* pNode);

void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition,
	FbxMesh* pMesh,
	FbxCluster* pCluster,
	FbxAMatrix& pVertexTransformMatrix,
	FbxTime pTime,
	FbxPose* pPose);

#endif // !FBX_UTIL_H_

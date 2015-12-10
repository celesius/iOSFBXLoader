//
//  FBXLoader.hpp
//  iOSFBXLoader
//
//  Created by vk on 15/11/16.
//  Copyright © 2015年 clover. All rights reserved.
//

#ifndef FBXLoader_hpp
#define FBXLoader_hpp


#include <stdio.h>
#include <fbxsdk.h>
#include <OpenGLES/gltypes.h>
#include "MaterialCache.hpp"

struct ModelBoneWeight
{
  uint8_t boneIndex[4];
  GLKVector4 boneWeight;
};

struct ModelVertex
{
    //齐次坐标
  GLKVector4 position;
  GLKVector3 normal;
  GLKVector2 uv0;
  uint8_t boneIndex[4];
  GLKVector4 boneWeight;
  
  bool operator == (const ModelVertex& v) const
  {
    return std::memcmp(this, &v, sizeof(ModelVertex)) == 0;
  }
};

struct ModelMesh
{
    std::string nodeName;
    std::string materialName;
    bool allByControlPoint;
    std::vector<ModelVertex> vertexList;
    //  std::vector<uint16_t> indexList;
    std::vector<int> indexList;
    std::vector<int> indexListControlByPolygon;
    
    GLKMatrix4 invMeshBaseposeMatrix;
    std::vector<std::string> boneNodeNameList;
    std::vector<GLKMatrix4> invBoneBaseposeMatrixList;
    bool haveMaterial;
    //jiangbo
};
/*
struct ColorChannel
{
    
    ColorChannel() : mTextureName(0)
    {
        mColor[0] = 0.0f;
        mColor[1] = 0.0f;
        mColor[2] = 0.0f;
        mColor[3] = 1.0f;
    }
    
    uint mTextureName;
    float mColor[4];
};
 */



struct ModelTexture {
    FbxFileTexture *fbxFileTexture;
    std::string texturePath;
    int width;
    int hight;
    unsigned char colorChannel;
    unsigned char *data;
};

class FBXLoader
{
public:
    ///////////////////////////////////////////////////Variables//////////////////////////////////////////
    float GetAnimationStartFrame() const
    {
        return this->animationStartFrame;
    }
    
    float GetAnimationEndFrame() const
    {
        return this->animationEndFrame;
    }
    int _numberOfMesh;
    bool _haveMaterial;
    
    /////////////////////////////////////////////////////Methods  //////////////////////////////////////////
    FBXLoader(const char* filepath);
    ~FBXLoader();
    ModelMesh getMesh(int index);
    std::string getTexturePath(int index);
    ModelMaterial getMaterial(int index);
    void updateMash();
    
    //std::vector<GLKVector4> modelPositionList;
    //std::vector<int> modelIndexList;
private:
    ///////////////////////////////////////////////////Variables//////////////////////////////////////////
    FbxManager* _sdkManager;
    FbxScene* _fbxScene;
    std::vector<ModelMesh> _meshList;
    std::vector<ModelMaterial> _materialList;
    std::vector<ModelTexture> _textureList;
    std::map<std::string, int> _materialIdDictionary;
    FbxArray<FbxString*> _mAnimStackNameArray;
    FbxAnimLayer * _CurrentAnimLayer;
    mutable FbxTime _mStart , _mStop;
    mutable FbxTime _mCache_Start, _mCache_Stop;
    mutable FbxTime _mCurrentTime;
    mutable FbxTime _mFrameTime;
    
    float animationStartFrame;
    float animationEndFrame;
    
    bool _allByControlPoint;
    /////////////////////////////////////////////////////Methods  //////////////////////////////////////////
    void initMeshList(FbxScene *scene);

    //std::vector<int> getModelMeshIndexList(FbxMesh *mesh, bool allByCtrlPoint, std::vector<int> & indexListByPolygon);
    
    ModelMesh parseMesh(FbxMesh *mesh, bool allByCrtlPoint);
    //ModelMaterial ParseMaterial(FbxSurfaceMaterial* material);
    bool thisMashIsAllByControlPoint(FbxMesh *pMesh);
    
    void initAnimationArray(FbxScene *scene);
    bool SetCurrentAnimStack(int pIndex, FbxScene *scene);
    void LoadCacheRecursive(FbxScene * pScene, FbxAnimLayer * pAnimLayer, const char * pFbxFileName, bool pSupportVBO);//done
    void LoadCacheRecursive(FbxNode * pNode, FbxAnimLayer * pAnimLayer, bool pSupportVBO);//done
    bool LoadTextureFromFile(const FbxString & pFilePath, unsigned int & pTextureObject);//done
    void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh,FbxCluster* pCluster, FbxAMatrix& pVertexTransformMatrix,FbxTime pTime, FbxPose* pPose);
    
    void DrawNodeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxAMatrix& pParentGlobalPosition, FbxPose* pPose);
    void DrawNode(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition, FbxPose* pPose);
    void DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,FbxAMatrix& pGlobalPosition, FbxPose* pPose);

    void ComputeSkinDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh,  FbxTime& pTime, FbxVector4* pVertexArray, FbxPose* pPose);
    void ComputeLinearDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray, FbxPose* pPose);
    void ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray, FbxPose* pPose);

    void MatrixScale(FbxAMatrix& pMatrix, double pValue);
    void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue);
    void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix);
    
    void UpdateVertexPosition( int modeMeshNumber, FbxMesh *pMesh, FbxVector4 *pVertexArray);
    
    void OnTimerClick() const;
    FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose = NULL, FbxAMatrix* pParentGlobalPosition = NULL);
	FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
	FbxAMatrix GetGeometry(FbxNode* pNode);
    
};



#endif /* FBXLoader_hpp */

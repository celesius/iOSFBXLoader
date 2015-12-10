//
//  FBXLoader.cpp
//  iOSFBXLoader
//
//  Created by vk on 15/11/16.
//  Copyright © 2015年 clover. All rights reserved.
//

#include "FBXLoader.hpp"
std::vector<int>  getModelMeshIndexList(FbxMesh *mesh, bool allByCtrlPoint, std::vector<int> & indexListByPolygon) {
    int polygonCount = mesh->GetPolygonCount();
    std::vector<int> indexList;
    indexList.reserve(polygonCount * 3);
    
    for (int i = 0; i < polygonCount; ++i)
    {
        indexList.push_back(mesh->GetPolygonVertex(i, 0));
        indexList.push_back(mesh->GetPolygonVertex(i, 1));
        indexList.push_back(mesh->GetPolygonVertex(i, 2));
        if (!allByCtrlPoint) {
            indexListByPolygon.push_back(i*3 + 0);
            indexListByPolygon.push_back(i*3 + 1);
            indexListByPolygon.push_back(i*3 + 2);
        }
    }
    return indexList;
}


std::vector<GLKVector4> getModelPositionList(FbxMesh* mesh, const std::vector<int>& indexList, bool allByCtrlPoint)
{
    //mesh中各顶点坐标
    std::vector<GLKVector4> positionList;
    
    if(allByCtrlPoint) {
        int lPolygonVertexCount = mesh->GetControlPointsCount();
        const FbxVector4 * lControlPoints = mesh->GetControlPoints();
        positionList.reserve(lPolygonVertexCount);
        for (int i=0; i< lPolygonVertexCount; i++) {
            auto controlPoint = mesh->GetControlPointAt(i);
            assert(controlPoint == lControlPoints[i]);
            positionList.push_back(GLKVector4Make(controlPoint[0], controlPoint[1], controlPoint[2], 1));
        }
    }
    else{
        positionList.reserve(indexList.size());
        for (auto index : indexList)
        {
            auto controlPoint = mesh->GetControlPointAt(index);
            positionList.push_back(GLKVector4Make(controlPoint[0], controlPoint[1], controlPoint[2], controlPoint[3]));
            // wは0.0のみ対応
            //assert(controlPoint[3] == 1.0);
        }
    }
    return positionList;
}

std::vector<GLKVector3> getModelNormalList(FbxMesh* mesh, const std::vector<int>& indexList, bool allByCtrlPoint)
{
    auto elementCount = mesh->GetElementNormalCount();
    
    //进入之前已经进行了判断
    //assert(elementCount == 1);
    
    auto element = mesh->GetElementNormal();
    auto mappingMode = element->GetMappingMode();
    auto referenceMode = element->GetReferenceMode();
    const auto& indexArray = element->GetIndexArray();
    const auto& directArray = element->GetDirectArray();
    
    // eDirctかeIndexDirectのみ対応
    assert((referenceMode == FbxGeometryElement::eDirect) || (referenceMode == FbxGeometryElement::eIndexToDirect));
    std::vector<GLKVector3> normalList;
    if(allByCtrlPoint) {
        FbxVector4 lCurrentNormal;
        int lPolygonVertexCount = mesh->GetControlPointsCount();
        normalList.reserve(lPolygonVertexCount);
        FbxGeometryElementNormal *lNormalElement = mesh->GetElementNormal(0);
        for(int i = 0; i < lPolygonVertexCount; i++) {
            int lNormalIndex = i;
            if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
            {
                lNormalIndex = lNormalElement->GetIndexArray().GetAt(i);
            }
            lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
            normalList.push_back(GLKVector3Make( lCurrentNormal[0],lCurrentNormal[1], lCurrentNormal[2] ));
        }
    }
    else{
        normalList.reserve(indexList.size());
        if (mappingMode == FbxGeometryElement::eByControlPoint)
        {
            // コントロールポイントでマッピング
            for (auto index : indexList)
            {
                auto normalIndex = (referenceMode == FbxGeometryElement::eDirect)
                ? index
                : indexArray.GetAt(index);
                auto normal = directArray.GetAt(normalIndex);
                normalList.push_back(GLKVector3Make(normal[0], normal[1], normal[2]));
                
                // wは1.0のみ対応
                //assert(normal[3] == 1.0);
            }
        }
        else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
        {
            // ポリゴンバーテックス（インデックス）でマッピング
            auto indexByPolygonVertex = 0;
            auto polygonCount = mesh->GetPolygonCount();
            for (int i = 0; i < polygonCount; ++i)
            {
                auto polygonSize = mesh->GetPolygonSize(i);
                
                for (int j = 0; j < polygonSize; ++j)
                {
                    auto normalIndex = (referenceMode == FbxGeometryElement::eDirect)
                    ? indexByPolygonVertex
                    : indexArray.GetAt(indexByPolygonVertex);
                    auto normal = directArray.GetAt(normalIndex);
                    
                    normalList.push_back(GLKVector3Make(normal[0], normal[1], normal[2]));
                    
                    // wは1.0のみ対応
                    //assert(normal[3] == 1.0); //jiangbo 11.27
                    
                    ++indexByPolygonVertex;
                }
                
            }
        }
        else
        {
            // それ以外のマッピングモードには対応しない
            assert(false);
        }
    }
    return normalList;
}

std::vector<GLKVector2> getModelUVList(FbxMesh* mesh, const std::vector<int>& indexList, int uvNo)
{
    std::vector<GLKVector2> uvList;
    
    auto elementCount = mesh->GetElementUVCount();
    if (uvNo + 1 > elementCount)
    {
        return uvList;
    }
    
    auto element = mesh->GetElementUV(uvNo);
    auto mappingMode = element->GetMappingMode();
    auto referenceMode = element->GetReferenceMode();
    const auto& indexArray = element->GetIndexArray();
    const auto& directArray = element->GetDirectArray();
    
    // eDirctかeIndexDirectのみ対応
    assert((referenceMode == FbxGeometryElement::eDirect) || (referenceMode == FbxGeometryElement::eIndexToDirect));
    
    uvList.reserve(indexList.size());
    
    if (mappingMode == FbxGeometryElement::eByControlPoint)
    {
        // コントロールポイントでマッピング
        for (auto index : indexList)
        {
            auto uvIndex = (referenceMode == FbxGeometryElement::eDirect)
            ? index
            : indexArray.GetAt(index);
            auto uv = directArray.GetAt(uvIndex);
            uvList.push_back(GLKVector2Make(uv[0], uv[1]));
        }
    }
    else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
    {
        // ポリゴンバーテックス（インデックス）でマッピング
        auto indexByPolygonVertex = 0;
        auto polygonCount = mesh->GetPolygonCount();
        for (int i = 0; i < polygonCount; ++i)
        {
            auto polygonSize = mesh->GetPolygonSize(i);
            
            for (int j = 0; j < polygonSize; ++j)
            {
                auto uvIndex = (referenceMode == FbxGeometryElement::eDirect)
                ? indexByPolygonVertex
                : indexArray.GetAt(indexByPolygonVertex);
                auto uv = directArray.GetAt(uvIndex);
                
                uvList.push_back(GLKVector2Make(uv[0], uv[1]));
                
                ++indexByPolygonVertex;
            }
            
        }
    }
    else
    {
        // それ以外のマッピングモードには対応しない
        assert(false);
    }
    
    return uvList;
}

void GetWeight(FbxMesh* mesh, const std::vector<int>& indexList, std::vector<ModelBoneWeight>& boneWeightList, std::vector<std::string>& boneNodeNameList, std::vector<GLKMatrix4>& invBaseposeMatrixList)
{
    auto skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
    if (skinCount == 0)
    {
        return;
    }
    
    // スキンが1しか対応しない
    assert(skinCount <= 1);
    
    auto controlPointsCount = mesh->GetControlPointsCount();
    
    printf(" controlPointsCount = %d in GetWeight\n",controlPointsCount);
    
    using TmpWeight = std::pair<int, float>;
    std::vector<std::vector<TmpWeight>> tmpBoneWeightList(controlPointsCount);
    
    auto skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
    auto clusterCount = skin->GetClusterCount();
    
    for (int i = 0; i < clusterCount; ++i)
    {
        auto cluster = skin->GetCluster(i);
        printf("%u",cluster->GetLinkMode());
        
        // eNormalizeしか対応しない
        //assert(cluster->GetLinkMode() == FbxCluster::eNormalize);
        
        boneNodeNameList.push_back(cluster->GetLink()->GetName());
        
        auto indexCount = cluster->GetControlPointIndicesCount();
        auto indices = cluster->GetControlPointIndices();
        auto weights = cluster->GetControlPointWeights();
        
        for (int j = 0; j < indexCount; ++j)
        {
            auto controlPointIndex = indices[j];
            tmpBoneWeightList[controlPointIndex].push_back({i, weights[j]});
        }
        
        // ペースポーズの逆行列を作成しておく
        GLKMatrix4 invBaseposeMatrix;
        
        auto baseposeMatrix = cluster->GetLink()->EvaluateGlobalTransform().Inverse();
        auto baseposeMatrixPtr = (double*)baseposeMatrix;
        for (int j = 0; j < 16; ++j)
        {
            invBaseposeMatrix.m[j] = (float)baseposeMatrixPtr[j];
        }
        
        invBaseposeMatrixList.push_back(invBaseposeMatrix);
    }
    
    // コントロールポイントに対応したウェイトを作成
    std::vector<ModelBoneWeight> boneWeightListControlPoints;
    for (auto& tmpBoneWeight : tmpBoneWeightList)
    {
        // ウェイトの大きさでソート
        std::sort(tmpBoneWeight.begin(), tmpBoneWeight.end(),
                  [](const TmpWeight& weightA, const TmpWeight& weightB){ return weightA.second > weightB.second; }
                  //[](const TmpWeight& weightA, const TmpWeight& weightB){ return weightA.second < weightB.second; }
                  );
        
        // 1頂点に4つより多くウェイトが割り振られているなら影響が少ないものは無視する
        while (tmpBoneWeight.size() > 4)
        {
            tmpBoneWeight.pop_back();
        }
        
        // 4つに満たない場合はダミーを挿入
        while (tmpBoneWeight.size() < 4)
        {
            tmpBoneWeight.push_back({0, 0.0f});
        }
        
        ModelBoneWeight weight;
        float total = 0.0f;
        for (int i = 0; i < 4; ++i)
        {
            weight.boneIndex[i] = tmpBoneWeight[i].first;
            weight.boneWeight.v[i] = tmpBoneWeight[i].second;
            
            total += tmpBoneWeight[i].second;
        }
        
        // ウェイトの正規化
        for (int i = 0; i < 4; ++i)
        {
            float tmp = weight.boneWeight.v[i];
            tmp *= 1 - total;
            weight.boneWeight.v[i] += tmp;
            //weight.boneWeight.v[i] /= total;
        }
        
        boneWeightListControlPoints.push_back(weight);
    }
    
    // インデックスで展開
    for (auto index : indexList)
    {
        boneWeightList.push_back(boneWeightListControlPoints[index]);
    }
}

FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial,
                               const char * pPropertyName,
                               const char * pFactorPropertyName,
                               uint & pTextureName)
{
    FbxDouble3 lResult(0, 0, 0);
    const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
    const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
    if (lProperty.IsValid() && lFactorProperty.IsValid())
    {
        lResult = lProperty.Get<FbxDouble3>();
        double lFactor = lFactorProperty.Get<FbxDouble>();
        if (lFactor != 1)
        {
            lResult[0] *= lFactor;
            lResult[1] *= lFactor;
            lResult[2] *= lFactor;
        }
    }
    
    if (lProperty.IsValid())
    {
        const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
        if (lTextureCount)
        {
            const FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>();
            if (lTexture && lTexture->GetUserDataPtr())
            {
                pTextureName = *(static_cast<uint *>(lTexture->GetUserDataPtr()));
            }
        }
    }
    
    return lResult;
}

bool fillTexture(FbxTexture *pTexture, ModelTexture &modelTexture) {
    FbxString textureType =  pTexture->GetTextureType();
    std::cout<<textureType<<std::endl;
    FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(pTexture);
    if (lFileTexture && !lFileTexture->GetUserDataPtr()) {
        const FbxString lFileName = lFileTexture->GetFileName();
        modelTexture.texturePath = lFileName;
        modelTexture.fbxFileTexture = lFileTexture;
        double ScaleU = lFileTexture->GetScaleU();
        double ScaleV = lFileTexture->GetScaleV();
        std::cout<<lFileName<<std::endl;
    }
    //FbxString textureFileName = pTexture->
    //FBXSDK_printf(" textureType = %c ",textureType.);
    
    return true;
}

FBXLoader::FBXLoader(const char* filepath) :_allByControlPoint(true) ,_haveMaterial(false)
{
    //Create the FBX SDK manager
    
	_mCache_Start = FBXSDK_TIME_INFINITE;
	_mCache_Stop  = FBXSDK_TIME_MINUS_INFINITE;
    _sdkManager = FbxManager::Create();
    if(!_sdkManager){
        FBXSDK_printf("errrrr _sdkManager");
        exit(1);
    }
    else{
        FBXSDK_printf("Autodesk FBX SDK version %s\n", _sdkManager->GetVersion());
    }
    // Create an IOSettings object.
    FbxIOSettings *ios = FbxIOSettings::Create(_sdkManager, IOSROOT);
    _sdkManager->SetIOSettings(ios);
    
    // ... Configure the FbxIOSettings object ...
    // Create an importer.
    FbxImporter *importer = FbxImporter::Create(_sdkManager, "");
    // Initialize the importer.
    if(!importer->Initialize(filepath,-1,_sdkManager->GetIOSettings())){
        FBXSDK_printf("err importer");
        FBXSDK_printf("Error returned: %s\n\n", importer->GetStatus().GetErrorString());
        exit(1);
    }
    
    FbxString lPath = FbxGetApplicationDirectory();
    std::cout<< lPath <<std::endl;
    _sdkManager->LoadPluginsDirectory(lPath.Buffer());
    
    _fbxScene = FbxScene::Create(_sdkManager, "iOSFBXScene");
    if(!_fbxScene) {
        FBXSDK_printf("EERRRR _fbxScene");
        exit(1);
    }
    
    if (!importer->Import(_fbxScene)) {
        FBXSDK_printf("err importer");
        exit(1);
    }
    importer->Destroy();
   
    //强制三角形
    FbxGeometryConverter geometryConverter(_sdkManager);
    geometryConverter.Triangulate(_fbxScene, true, true);
    initMeshList(_fbxScene);
    initAnimationArray(_fbxScene);
}

FBXLoader::~FBXLoader() {

}


std::string FBXLoader::getTexturePath(int index) {
    return _textureList[index].texturePath;
}

ModelMaterial FBXLoader::getMaterial(int index) {
    return _materialList[index];
}

void FBXLoader::initMeshList(FbxScene *scene) {
    int materialCount = scene->GetMaterialCount();
    int textureCount = scene->GetTextureCount();
    int meshCout = scene->GetMemberCount<FbxMesh>();
    FBXSDK_printf(" scene->GetMemberCount<FbxMesh> = %d\n ",meshCout);
    FBXSDK_printf(" node->GetMaterialCount  = %d \n",materialCount );
    FBXSDK_printf(" node->GetTextureCount   = %d \n",textureCount);
    _materialList.reserve(materialCount);
    _textureList.reserve(textureCount);
    /**
     *  解析贴图数据
     */
    for (int i = 0; i < textureCount; i ++) {
        ModelTexture mModelText;
        FbxTexture *lTexture = scene->GetTexture(i);
        FbxFileTexture::EUnifiedMappingType maptype =  lTexture->CurrentMappingType;
        FbxFileTexture::EBlendMode blendMode = lTexture->CurrentTextureBlendMode;
        //FbxFileTexture::EAlignMode alignMode = lTexture->cu
        
        bool isOK = fillTexture(lTexture, mModelText);
        _textureList.push_back(mModelText);
    }
    
    for (int i = 0; i < materialCount; ++i)
    {
       // auto fbxMaterial = scene->GetMaterial(i);
       // auto modelMaterial = this->ParseMaterial(fbxMaterial);
       // this->_materialList.push_back(modelMaterial);
       // this->_materialIdDictionary.insert({modelMaterial.materialName, i});
        ModelMaterial material;
        FbxSurfaceMaterial * lMaterial = scene->GetMaterial(i);
		if (lMaterial && !lMaterial->GetUserDataPtr())
		{
			FbxAutoPtr<MaterialCache> lMaterialCache(new MaterialCache);
			if (lMaterialCache->Initialize(lMaterial,material))
			{
				lMaterial->SetUserDataPtr(lMaterialCache.Release());
			}
            _materialList.push_back(material);
            _haveMaterial = true;
		}
    }
    
    _numberOfMesh = meshCout;
    _meshList.reserve(meshCout);
    for (int i=0; i<meshCout; i++) {
        FbxMesh *fbxMesh = scene->GetMember<FbxMesh>(i);
        //const char *name = fbxMesh->GetName();
        //std::string nameS;
        //printf(" ")
        //printf(" fbxMesh->GetName() = %s \n",name);
        _allByControlPoint = thisMashIsAllByControlPoint(fbxMesh);
        if(_allByControlPoint) {
            printf("mesh %d is All By ControlPoint \n",i);
        }
        //一个fbx中包含多个mesh
        ModelMesh mModelMesh = parseMesh(fbxMesh, _allByControlPoint);
        mModelMesh.allByControlPoint = _allByControlPoint;
        _meshList.push_back(mModelMesh);
        _allByControlPoint = true;
        //_meshList.push_back();
    }
	_mFrameTime.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());
}

/**
 *  判断一下这个mesh的顶点是否受control point控制
 *
 *  @param pMesh <#pMesh description#>
 *
 *  @return <#return value description#>
 */
bool FBXLoader::thisMashIsAllByControlPoint(FbxMesh *pMesh) {
    bool mHasNormal = pMesh->GetElementNormalCount() > 0;
    bool mHasUV = pMesh->GetElementUVCount() > 0;
    bool allByCrtlPoint = true;
    if(mHasNormal)
    {
        FbxGeometryElement::EMappingMode normalMappingMod = pMesh->GetElementNormal(0)->GetMappingMode();
        if (normalMappingMod == FbxGeometryElement::eNone) {
            mHasNormal = false;
        }
        if(mHasNormal && (normalMappingMod != FbxGeometryElement::eByControlPoint )) {
            allByCrtlPoint = false;
        }
    }
    if(allByCrtlPoint) {
        if(mHasUV) {
            FbxGeometryElement::EMappingMode uvMappingMod = pMesh->GetElementUV(0)->GetMappingMode();
            if(uvMappingMod == FbxGeometryElement::eNone) {
                mHasUV = false;
            }
            if(mHasUV && (uvMappingMod != FbxGeometryElement::eByControlPoint )) {
                allByCrtlPoint = false;
            }
        }
    }
    
    return allByCrtlPoint;
}

ModelMesh FBXLoader::parseMesh(FbxMesh *pMesh, bool allByCrtlPoint) {
    ModelMesh modelMesh;
    FbxNode *node = pMesh->GetNode();
    
    //开始填充modeleMesh;
    modelMesh.nodeName = node->GetName();
    modelMesh.materialName = node->GetMaterial(0)->GetName();
    FBXSDK_printf("node->GetName() = %s\n",node->GetName());
    FBXSDK_printf("node->GetMaterial(0)->GetName() = %s\n",node->GetMaterial(0)->GetName() );
    
    for (int i = 0; i<node->GetMaterialCount(); i++) {
    FBXSDK_printf(" %d material name = %s\n",i,node->GetMaterial(i)->GetName());
    }
    
    int triangleCount = pMesh->GetPolygonCount();
    int lPolygonVertexCount = pMesh->GetControlPointsCount();
    int verterCount = 0;
    
    std::vector<int>  & modelIndexList = modelMesh.indexList;
    modelIndexList = getModelMeshIndexList(pMesh,allByCrtlPoint,modelMesh.indexListControlByPolygon);
    FBXSDK_printf(" modelIndexList.size() = %lu\n", modelIndexList.size());
    std::vector<GLKVector4> modelPositionList = getModelPositionList(pMesh, modelIndexList, allByCrtlPoint);
    FBXSDK_printf(" modelPositionList.size() = %lu\n", modelPositionList.size());
    
    
    FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
    std::vector<GLKVector3> modelNormalList;
    if(pMesh->GetElementNormalCount() > 0) {
        FBXSDK_printf(" wellHave normal element and count = %d\n", pMesh->GetElementNormalCount());
        lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
        FBXSDK_printf("lNormalMappingMode = %d\n",lNormalMappingMode);
        modelNormalList = getModelNormalList(pMesh, modelIndexList, allByCrtlPoint);
    }
    
    FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
    std::vector<GLKVector2> modelUV0List;
    if(pMesh->GetElementUVCount() > 0) {
        FBXSDK_printf(" GetElementUVCount = %d\n" ,pMesh->GetElementUVCount());
        lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
        modelUV0List = getModelUVList(pMesh, modelIndexList, 0);
    }
    
    
    //std::vector<ModelBoneWeight> boneWeightList;
    //GetWeight(pMesh, modelIndexList, boneWeightList, modelMesh.boneNodeNameList, modelMesh.invBoneBaseposeMatrixList);
   
    if(allByCrtlPoint) {
        assert(modelPositionList.size() == modelNormalList.size());
        //assert(modelUV0List.size() == modelNormalList.size());
    } else {
        assert(modelIndexList.size() == modelPositionList.size());
        assert(modelIndexList.size() == modelNormalList.size());
        //assert(modelIndexList.size() == modelUV0List.size());
    }
    
    
    std::vector<ModelVertex> & modelVertexList = modelMesh.vertexList;
    modelVertexList.reserve(modelPositionList.size());
    
    for (int i = 0; i < modelPositionList.size(); ++i)
    {
        ModelVertex vertex;
        vertex.position = modelPositionList[i];
        vertex.normal = modelNormalList[i];
        vertex.uv0 = (modelUV0List.size() == 0) ? GLKVector2Make(0.0f, 0.0f): modelUV0List[i];
        /*
        if (boneWeightList.size() > 0)
        {
            for (int j = 0; j < 4; ++j)
            {
                vertex.boneIndex[j] = boneWeightList[i].boneIndex[j];
            }
            vertex.boneWeight = boneWeightList[i].boneWeight;
        }
        else
        {
            for (int j = 0; j < 4; ++j)
            {
                vertex.boneIndex[j] = 0;
            }
            vertex.boneWeight = GLKVector4Make(1, 0, 0, 0);
        }
        */
        
        modelVertexList.push_back(vertex);
    }
    
    printf(" %lu\n", modelVertexList.size());
    
    //FBXSDK_printf(" %d  ",node->Get ());
    /**
     *  若mesh中带有material
     */
    int nodeChildNum =  node->GetChildCount();
    for (int i = 0; i< nodeChildNum; i++) {
        FBXSDK_printf("  node->GetChildName(%d) = %s \n", i,node->GetChildName(i) );
    }
    
    if(pMesh->GetElementMaterial()) {
        FBXSDK_printf("  GetElementMaterial");
    }
    /*
     int  polygonVertexCount = pMesh->GetPolygonVertexCount();
     for(int i = 0; i<polygonVertexCount;i++) {
     FBXSDK_printf(" %d = %d \n ", i, pMesh->GetPolygonVertexIndex(i));
     }
     */
    
    /*
     if(node->GetMaterialCount() != 0) {
     
     }
     else {
     
     }
     */
    return modelMesh;
}
/*
ModelMaterial FBXLoader::ParseMaterial(FbxSurfaceMaterial* material)
{
    ModelMaterial modelMaterial;
    modelMaterial.materialName = material->GetName();
    
    printf(">> material: %s\n", modelMaterial.materialName.c_str());
    
    // CGFXのみ対応
    auto implementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);
    assert(implementation != nullptr);
    
    auto rootTable = implementation->GetRootTable();
    auto entryCount = rootTable->GetEntryCount();
    
    for (int i = 0; i < entryCount; ++i)
    {
        auto entry = rootTable->GetEntry(i);
        
        auto fbxProperty = material->FindPropertyHierarchical(entry.GetSource());
        if (!fbxProperty.IsValid())
        {
            fbxProperty = material->RootProperty.FindHierarchical(entry.GetSource());
        }
        
        auto textureCount = fbxProperty.GetSrcObjectCount<FbxTexture>();
        if (textureCount > 0)
        {
            std::string src = entry.GetSource();
            
            for (int j = 0; j < fbxProperty.GetSrcObjectCount<FbxFileTexture>(); ++j)
            {
                auto tex = fbxProperty.GetSrcObject<FbxFileTexture>(j);
                std::string texName = tex->GetFileName();
                texName = texName.substr(texName.find_last_of('/') + 1);
                
                if (src == "Maya|DiffuseTexture")
                {
                    modelMaterial.diffuseTextureName = texName;
                }
                else if (src == "Maya|NormalTexture")
                {
                    modelMaterial.normalTextureName = texName;
                }
                else if (src == "Maya|SpecularTexture")
                {
                    modelMaterial.specularTextureName = texName;
                }
                else if (src == "Maya|FalloffTexture")
                {
                    modelMaterial.falloffTextureName = texName;
                }
                else if (src == "Maya|ReflectionMapTexture")
                {
                    modelMaterial.reflectionMapTextureName = texName;
                }
            }
        }
    }
    
    printf("diffuseTexture: %s\n", modelMaterial.diffuseTextureName.c_str());
    printf("normalTexture: %s\n", modelMaterial.normalTextureName.c_str());
    printf("specularTexture: %s\n", modelMaterial.specularTextureName.c_str());
    printf("falloffTexture: %s\n", modelMaterial.falloffTextureName.c_str());
    printf("reflectionMapTexture: %s\n", modelMaterial.reflectionMapTextureName.c_str());
    
    return modelMaterial;
}

*/

ModelMesh FBXLoader::getMesh(int index) {
    return _meshList[index];
}

//Animation

void FBXLoader::initAnimationArray(FbxScene *scene) {
    scene->FillAnimStackNameArray(_mAnimStackNameArray);
    SetCurrentAnimStack(0, scene);
    
}

bool FBXLoader::SetCurrentAnimStack(int pIndex, FbxScene *scene)
{
    const int lAnimStackCount = _mAnimStackNameArray.GetCount();
    if (!lAnimStackCount || pIndex >= lAnimStackCount)
    {
        return false;
    }
    
    // select the base layer from the animation stack
    FbxAnimStack * lCurrentAnimationStack = scene->FindMember<FbxAnimStack>(_mAnimStackNameArray[pIndex]->Buffer());
    if (lCurrentAnimationStack == NULL)
    {
        // this is a problem. The anim stack should be found in the scene!
        return false;
    }
    
    // we assume that the first animation layer connected to the animation stack is the base layer
    // (this is the assumption made in the FBXSDK)
    _CurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
    scene->SetCurrentAnimationStack(lCurrentAnimationStack);
    
    FbxTakeInfo* lCurrentTakeInfo = scene->GetTakeInfo(*(_mAnimStackNameArray[pIndex]));
    if (lCurrentTakeInfo)
    {
        _mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
        _mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
    }
    else
    {
        // Take the time line value
        FbxTimeSpan lTimeLineTimeSpan;
        scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
        
        _mStart = lTimeLineTimeSpan.GetStart();
        _mStop  = lTimeLineTimeSpan.GetStop();
    }
   
    FbxTime importOffset = lCurrentTakeInfo->mImportOffset;
    
    this->animationStartFrame = (importOffset.Get() + _mStart.Get()) / FbxTime::GetOneFrameValue(FbxTime::eFrames60);
    this->animationEndFrame = (importOffset.Get() + _mStop.Get()) / FbxTime::GetOneFrameValue(FbxTime::eFrames60);
    
    // check for smallest start with cache start
    if(_mCache_Start < _mStart)
        _mStart = _mCache_Start;
    
    // check for biggest stop with cache stop
    if(_mCache_Stop  > _mStop)
        _mStop  = _mCache_Stop;
    
    // move to beginning
    _mCurrentTime = _mStart;
    return true;
}

void FBXLoader::OnTimerClick() const
{
	// Loop in the animation stack if not paused.
	if (_mStop > _mStart)
	{

		_mCurrentTime += _mFrameTime;

		if (_mCurrentTime > _mStop)
		{
			_mCurrentTime = _mStart;
		}
	}
}

void FBXLoader::updateMash() {
    OnTimerClick();
    float meshCout = _fbxScene->GetMemberCount<FbxMesh>();
    FBXSDK_printf(" meshCout = %f\n ",meshCout);
    FbxNode *fn = _fbxScene->GetRootNode();
    int mPoseIndex = -1;
    
    FbxPose * lPose = NULL;
	if (mPoseIndex != -1)
	{
		lPose = _fbxScene->GetPose(mPoseIndex);
	}
    
    FbxAMatrix lDummyGlobalPosition;
    DrawNodeRecursive(fn, _mCurrentTime, _CurrentAnimLayer, lDummyGlobalPosition, lPose);
    
}
/**
 *  更新mash的入口函数
 *
 *  @param pNode                 rootNode
 *  @param pTime                 当前帧时间
 *  @param pAnimLayer            解析fbx得到的动画层
 *  @param pParentGlobalPosition 未知
 *  @param pPose                 未知
 */
void FBXLoader::DrawNodeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxAMatrix& pParentGlobalPosition, FbxPose* pPose)
{

	FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);

	if (pNode->GetNodeAttribute())
	{
		// Geometry offset.
		// it is not inherited by the children.
		FbxAMatrix lGeometryOffset = GetGeometry(pNode);
		FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

		DrawNode(pNode, pTime, pAnimLayer, pParentGlobalPosition, lGlobalOffPosition, pPose);
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		DrawNodeRecursive(pNode->GetChild(lChildIndex), pTime, pAnimLayer, lGlobalPosition, pPose);
	}
}

void FBXLoader::DrawNode(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition, FbxPose* pPose)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			//DrawSkeleton(pNode, pParentGlobalPosition, pGlobalPosition);
		}
		// NURBS and patch have been converted into triangluation meshes.
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			DrawMesh(pNode, pTime, pAnimLayer, pGlobalPosition, pPose);
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull)
		{
		}
	}
	else
	{
		// Draw a Null for nodes without attribute.
	}

}

void FBXLoader::DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,FbxAMatrix& pGlobalPosition, FbxPose* pPose)
{

	FbxMesh* lMesh = pNode->GetMesh();
	const int lVertexCount = lMesh->GetControlPointsCount();
    printf(" controlPointsCount = %d in DrawMesh\n",lVertexCount );

	// No vertex to draw.
	if (lVertexCount == 0)
	{
		return;
	}

	//const VBOMesh * lMeshCache = static_cast<const VBOMesh *>(lMesh->GetUserDataPtr());

	// If it has some defomer connection, update the vertices position
	const bool lHasVertexCache = lMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
	(static_cast<FbxVertexCacheDeformer*>(lMesh->GetDeformer(0, FbxDeformer::eVertexCache)))->Active;
	const bool lHasShape = lMesh->GetShapeCount() > 0;
	const bool lHasSkin = lMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
	const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;

	FbxVector4* lVertexArray = NULL;
	
    //if (!lMeshCache || lHasDeformation)
    if ( lHasDeformation)
	{
		lVertexArray = new FbxVector4[lVertexCount];
		memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
	}
    

	if (lHasDeformation)
	{
		// Active vertex cache deformer will overwrite any other deformer
		if (lHasVertexCache)
		{
            printf("errr lHasVertexCache is true need ReadVertexCacheData\n");
			//ReadVertexCacheData(lMesh, pTime, lVertexArray);
		}
		else
		{
			if (lHasShape)
			{
				// Deform the vertex array with the shapes.
                printf("errr lHasShape is true need ComputeShapeDeformation\n");
				//ComputeShapeDeformation(lMesh, pTime, pAnimLayer, lVertexArray);
			}

			//we need to get the number of clusters
			const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
			int lClusterCount = 0;
			for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
			{
				lClusterCount += ((FbxSkin *)(lMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
			}
			if (lClusterCount)
			{
				// Deform the vertex array with the skin deformer.
				ComputeSkinDeformation(pGlobalPosition, lMesh, pTime, lVertexArray, pPose);
			}
		}

        UpdateVertexPosition(0, lMesh, lVertexArray);
        
		//if (lMeshCache)
		//	lMeshCache->UpdateVertexPosition(lMesh, lVertexArray);
	}

	//glPushMatrix();
	//glMultMatrixd((const double*)pGlobalPosition);
    /*
	if (lMeshCache)
	{
		lMeshCache->BeginDraw();
		const int lSubMeshCount = lMeshCache->GetSubMeshCount();
		for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex)
		{

			const FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lIndex);
			if (lMaterial)
			{
				const MaterialCache * lMaterialCache = static_cast<const MaterialCache *>(lMaterial->GetUserDataPtr());
				if (lMaterialCache)
				{
					lMaterialCache->SetCurrentMaterial();
				}
			}
			else
			{
				// Draw green for faces without material
				MaterialCache::SetDefaultMaterial();
			}


			lMeshCache->Draw(lIndex);
		}
		lMeshCache->EndDraw();
	}
	else
	{
		// OpenGL driver is too lower and use Immediate Mode
		//glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		const int lPolygonCount = lMesh->GetPolygonCount();
		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++)
		{
			const int lVerticeCount = lMesh->GetPolygonSize(lPolygonIndex);
			//glBegin(GL_LINE_LOOP);
			for (int lVerticeIndex = 0; lVerticeIndex < lVerticeCount; lVerticeIndex++)
			{
				glVertex3dv((double *)lVertexArray[lMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex)]);
			}
			//glEnd();
		}
	}
        */

	//glPopMatrix();

	delete [] lVertexArray;

}

// not mAllByControlPoint
void FBXLoader::UpdateVertexPosition( int modeMeshNumber, FbxMesh *pMesh, FbxVector4 *pVertices) {
    bool allByCtrlpoint = _meshList[modeMeshNumber].allByControlPoint;
    int lVertexCount = 0;
    if(allByCtrlpoint) {
		lVertexCount = pMesh->GetControlPointsCount();
        for (int i = 0; i < lVertexCount; i++) {
                _meshList[modeMeshNumber].vertexList[i].position = GLKVector4Make(static_cast<float>(pVertices[i][0]), static_cast<float>(pVertices[i][1]), static_cast<float>(pVertices[i][2]), 1);
        }
    }else {
        const int lPolygonCount = pMesh->GetPolygonCount();
        //lVertexCount = lPolygonCount * 3;//TRIANGLE_VERTEX_COUNT;
        //lVertices = new float[lVertexCount * VERTEX_STRIDE];
        for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
        {
            for (int lVerticeIndex = 0; lVerticeIndex < 3 ; ++lVerticeIndex)
            {
                const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
                _meshList[modeMeshNumber].vertexList[lVertexCount].position = GLKVector4Make(static_cast<float>(pVertices[lControlPointIndex][0]), static_cast<float>(pVertices[lControlPointIndex][1]), static_cast<float>(pVertices[lControlPointIndex][2]), 1);
                ++lVertexCount;
            }
        }
    }
}

void FBXLoader::ComputeSkinDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh,  FbxTime& pTime, FbxVector4* pVertexArray, FbxPose* pPose)
{

	FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType lSkinningType = lSkinDeformer->GetSkinningType();

	if(lSkinningType == FbxSkin::eLinear || lSkinningType == FbxSkin::eRigid)
	{
		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, pVertexArray, pPose);
	}
	else if(lSkinningType == FbxSkin::eDualQuaternion)
	{
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, pVertexArray, pPose);
	}
	else if(lSkinningType == FbxSkin::eBlend)
	{
		int lVertexCount = pMesh->GetControlPointsCount();

		FbxVector4* lVertexArrayLinear = new FbxVector4[lVertexCount];
		memcpy(lVertexArrayLinear, pMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

		FbxVector4* lVertexArrayDQ = new FbxVector4[lVertexCount];
		memcpy(lVertexArrayDQ, pMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayLinear, pPose);
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayDQ, pPose);

		// To blend the skinning according to the blend weights
		// Final vertex = DQSVertex * blend weight + LinearVertex * (1- blend weight)
		// DQSVertex: vertex that is deformed by dual quaternion skinning method;
		// LinearVertex: vertex that is deformed by classic linear skinning method;
		int lBlendWeightsCount = lSkinDeformer->GetControlPointIndicesCount();
		for(int lBWIndex = 0; lBWIndex<lBlendWeightsCount; ++lBWIndex)
		{
			double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[lBWIndex];
			pVertexArray[lBWIndex] = lVertexArrayDQ[lBWIndex] * lBlendWeight + lVertexArrayLinear[lBWIndex] * (1 - lBlendWeight);
		}
	}

}

void FBXLoader::ComputeLinearDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray, FbxPose* pPose)
{

  int polygonCount = pMesh->GetPolygonCount();
    printf("polygonCount  = %d\n",polygonCount);
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
    printf(" controlPointsCount = %d in ComputeLinearDeformation\n",lVertexCount);
	FbxAMatrix* lClusterDeformation = new FbxAMatrix[lVertexCount];
	memset(lClusterDeformation, 0, lVertexCount * sizeof(FbxAMatrix));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	if (lClusterMode == FbxCluster::eAdditive)
	{
		for (int i = 0; i < lVertexCount; ++i)
		{
			lClusterDeformation[i].SetIdentity();
		}
	}

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);

		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			FbxAMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{            
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				// Compute the influence of the link on the vertex.
				FbxAMatrix lInfluence = lVertexTransformMatrix;
				MatrixScale(lInfluence, lWeight);

				if (lClusterMode == FbxCluster::eAdditive)
				{    
					// Multiply with the product of the deformations on the vertex.
					MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
					lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
					// Add to the sum of the deformations on the vertex.
					MatrixAdd(lClusterDeformation[lIndex], lInfluence);

					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex			
		}//lClusterCount
	}
    //test code s
   
    std::vector<double>lClusterWeightVectore;
    for(int i = 0;i<lVertexCount;i++) {
        lClusterWeightVectore.push_back(lClusterWeight[i]);
    }
    
    
    //test code e

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) 
	{
		FbxVector4 lSrcVertex = pVertexArray[i];
		FbxVector4& lDstVertex = pVertexArray[i];
		double lWeight = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeight != 0.0) 
		{
			lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);
			if (lClusterMode == FbxCluster::eNormalize)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeight;
			}
			else if (lClusterMode == FbxCluster::eTotalOne)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeight);
				lDstVertex += lSrcVertex;
			}
		} 
	}

	delete [] lClusterDeformation;
	delete [] lClusterWeight;

}

void FBXLoader::MatrixScale(FbxAMatrix& pMatrix, double pValue)
{
    
    int i,j;
    
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pMatrix[i][j] *= pValue;
        }
    }
}

void FBXLoader::MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue)
{
    pMatrix[0][0] += pValue;
    pMatrix[1][1] += pValue;
    pMatrix[2][2] += pValue;
    pMatrix[3][3] += pValue;
}


void FBXLoader::MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix)
{
    
    int i,j;
    
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pDstMatrix[i][j] += pSrcMatrix[i][j];
        }
    }
    
}

void FBXLoader::ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray, FbxPose* pPose)
{

	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	FbxDualQuaternion* lDQClusterDeformation = new FbxDualQuaternion[lVertexCount];
	memset(lDQClusterDeformation, 0, lVertexCount * sizeof(FbxDualQuaternion));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			FbxAMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose);

			FbxQuaternion lQ = lVertexTransformMatrix.GetQ();
			FbxVector4 lT = lVertexTransformMatrix.GetT();
			FbxDualQuaternion lDualQuaternion(lQ, lT);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{ 
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
					continue;

				// Compute the influence of the link on the vertex.
				FbxDualQuaternion lInfluence = lDualQuaternion * lWeight;
				if (lClusterMode == FbxCluster::eAdditive)
				{    
					// Simply influenced by the dual quaternion.
					lDQClusterDeformation[lIndex] = lInfluence;

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
					if(lClusterIndex == 0)
					{
						lDQClusterDeformation[lIndex] = lInfluence;
					}
					else
					{
						// Add to the sum of the deformations on the vertex.
						// Make sure the deformation is accumulated in the same rotation direction. 
						// Use dot product to judge the sign.
						double lSign = lDQClusterDeformation[lIndex].GetFirstQuaternion().DotProduct(lDualQuaternion.GetFirstQuaternion());
						if( lSign >= 0.0 )
						{
							lDQClusterDeformation[lIndex] += lInfluence;
						}
						else
						{
							lDQClusterDeformation[lIndex] -= lInfluence;
						}
					}
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) 
	{
		FbxVector4 lSrcVertex = pVertexArray[i];
		FbxVector4& lDstVertex = pVertexArray[i];
		double lWeightSum = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeightSum != 0.0) 
		{
			lDQClusterDeformation[i].Normalize();
			lDstVertex = lDQClusterDeformation[i].Deform(lDstVertex);

			if (lClusterMode == FbxCluster::eNormalize)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeightSum;
			}
			else if (lClusterMode == FbxCluster::eTotalOne)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeightSum);
				lDstVertex += lSrcVertex;
			}
		} 
	}

	delete [] lDQClusterDeformation;
	delete [] lClusterWeight;

}

void FBXLoader::ComputeClusterDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh,FbxCluster* pCluster, FbxAMatrix& pVertexTransformMatrix,FbxTime pTime, FbxPose* pPose)
{

	FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;
	FbxAMatrix lAssociateGlobalInitPosition;
	FbxAMatrix lAssociateGlobalCurrentPosition;
	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lReferenceGeometry;
	FbxAMatrix lAssociateGeometry;
	FbxAMatrix lClusterGeometry;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;

	if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel())
	{
		pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
        lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel(), pTime, pPose);

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPosition;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = pGlobalPosition;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.
		pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	}

}

FbxAMatrix  FBXLoader::GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
{

	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if (pPose)
	{
		int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}
				else
				{
					if (pNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			lPositionFound = true;
		}
	}

	if (!lPositionFound)
	{
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	}

	return lGlobalPosition;
}

FbxAMatrix FBXLoader::GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

	return lPoseMatrix;
}

FbxAMatrix FBXLoader::GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}
/*
ModelMesh FBXLoader::getMesh(int index)
{
    
}
*/
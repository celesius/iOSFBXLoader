//
//  MaterialCache.hpp
//  iOSFBXLoader
//
//  Created by vk on 15/11/27.
//  Copyright © 2015年 clover. All rights reserved.
//

#ifndef MaterialCache_hpp
#define MaterialCache_hpp

#include <stdio.h>
#include <fbxsdk.h>
#include <OpenGLES/gltypes.h>

struct ModelMaterial
{
    GLKVector4  mEmissive;
    GLKVector4 mAmbient;
    GLKVector4 mDiffuse;
    GLKVector4 mSpecular;
    GLfloat mShinness;
    /*
    std::string materialName;
    std::string diffuseTextureName;
    std::string normalTextureName;
    std::string specularTextureName;
    std::string falloffTextureName;
    std::string reflectionMapTextureName;
    */
};

class MaterialCache
{
public:
    MaterialCache();
    ~MaterialCache();
    
    bool Initialize(const FbxSurfaceMaterial * pMaterial, ModelMaterial &lModelMaterial);
    
    // Set material colors and binding diffuse texture if exists.
    void SetCurrentMaterial() const;
    
    bool HasTexture() const { return mDiffuse.mTextureName != 0; }
    
    // Set default green color.
    static void SetDefaultMaterial();
    
    // Get specific property value and connected texture if any.
    // Value = Property value * Factor property value (if no factor property, multiply by 1).
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
    
private:
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
    ColorChannel mEmissive;
    ColorChannel mAmbient;
    ColorChannel mDiffuse;
    ColorChannel mSpecular;
    float mShinness;
};


#endif /* MaterialCache_hpp */

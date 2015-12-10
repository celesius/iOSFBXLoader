//
//  MaterialCache.cpp
//  iOSFBXLoader
//
//  Created by vk on 15/11/27.
//  Copyright © 2015年 clover. All rights reserved.
//

#include "MaterialCache.hpp"

MaterialCache::MaterialCache() : mShinness(0)
{
    
}

MaterialCache::~MaterialCache()
{
    
}

// Bake material properties.
bool MaterialCache::Initialize(const FbxSurfaceMaterial * pMaterial, ModelMaterial &lModelMaterial)
{
    const FbxDouble3 lEmissive = GetMaterialProperty(pMaterial,
                                                     FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, mEmissive.mTextureName);
    //mEmissive.mColor[0] = static_cast<GLfloat>(lEmissive[0]);
    //mEmissive.mColor[1] = static_cast<GLfloat>(lEmissive[1]);
    //mEmissive.mColor[2] = static_cast<GLfloat>(lEmissive[2]);

    lModelMaterial.mEmissive.r = static_cast<GLfloat>(lEmissive[0]);
    lModelMaterial.mEmissive.g = static_cast<GLfloat>(lEmissive[1]);
    lModelMaterial.mEmissive.b = static_cast<GLfloat>(lEmissive[2]);
    lModelMaterial.mEmissive.a = 1.0;
    
    const FbxDouble3 lAmbient = GetMaterialProperty(pMaterial,
                                                    FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, mAmbient.mTextureName);
    //mAmbient.mColor[0] = static_cast<GLfloat>(lAmbient[0]);
    //mAmbient.mColor[1] = static_cast<GLfloat>(lAmbient[1]);
    //mAmbient.mColor[2] = static_cast<GLfloat>(lAmbient[2]);
   
    lModelMaterial.mAmbient.r = static_cast<GLfloat>(lAmbient[0]);
    lModelMaterial.mAmbient.g = static_cast<GLfloat>(lAmbient[1]);
    lModelMaterial.mAmbient.b = static_cast<GLfloat>(lAmbient[2]);
    lModelMaterial.mAmbient.a = 1.0;
    
    const FbxDouble3 lDiffuse = GetMaterialProperty(pMaterial,
                                                    FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, mDiffuse.mTextureName);
    //mDiffuse.mColor[0] = static_cast<GLfloat>(lDiffuse[0]);
    //mDiffuse.mColor[1] = static_cast<GLfloat>(lDiffuse[1]);
    //mDiffuse.mColor[2] = static_cast<GLfloat>(lDiffuse[2]);
   
    lModelMaterial.mDiffuse.r = static_cast<GLfloat>(lDiffuse[0]);
    lModelMaterial.mDiffuse.g = static_cast<GLfloat>(lDiffuse[1]);
    lModelMaterial.mDiffuse.b = static_cast<GLfloat>(lDiffuse[2]);
    lModelMaterial.mDiffuse.a = 1.0;
    
    const FbxDouble3 lSpecular = GetMaterialProperty(pMaterial,
                                                     FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, mSpecular.mTextureName);
    //mSpecular.mColor[0] = static_cast<GLfloat>(lSpecular[0]);
    //mSpecular.mColor[1] = static_cast<GLfloat>(lSpecular[1]);
    //mSpecular.mColor[2] = static_cast<GLfloat>(lSpecular[2]);
    
    lModelMaterial.mSpecular.r = static_cast<GLfloat>(lSpecular[0]);
    lModelMaterial.mSpecular.g = static_cast<GLfloat>(lSpecular[1]);
    lModelMaterial.mSpecular.b = static_cast<GLfloat>(lSpecular[2]);
    lModelMaterial.mSpecular.a = 1.0;
    
    
    FbxProperty lShininessProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    if (lShininessProperty.IsValid())
    {
        double lShininess = lShininessProperty.Get<FbxDouble>();
        //mShinness = static_cast<GLfloat>(lShininess);
        lModelMaterial.mShinness = static_cast<GLfloat>(lShininess);
    }
    return true;
}
/*
void MaterialCache::SetCurrentMaterial() const
{
    glMaterialfv(GL_FRONT, GL_EMISSION, mEmissive.mColor);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbient.mColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiffuse.mColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecular.mColor);
    glMaterialf(GL_FRONT, GL_SHININESS, mShinness);
    
    glBindTexture(GL_TEXTURE_2D, mDiffuse.mTextureName);
}

void MaterialCache::SetDefaultMaterial()
{
    const GLfloat BLACK_COLOR[] = {0.0f, 0.0f, 0.0f, 1.0f};
    const GLfloat GREEN_COLOR[] = {0.0f, 1.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, BLACK_COLOR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, BLACK_COLOR);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, GREEN_COLOR);
    glMaterialfv(GL_FRONT, GL_SPECULAR, BLACK_COLOR);
    glMaterialf(GL_FRONT, GL_SHININESS, 0);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}
*/